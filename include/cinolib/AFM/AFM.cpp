/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/AFM/AFM.h>
#include <cinolib/AFM/flip_checks.h>
#include <cinolib/AFM/advance_move.h>
#include <cinolib/geometry/n_sided_poygon.h>
#include <cinolib/split_separating_simplices.h>
#include <cinolib/how_many_seconds.h>
#include <cinolib/dijkstra.h>

namespace cinolib
{

CINO_INLINE
void AFM(AFM_data & data)
{
    if(!data.initialized) AFM_init(data);

    uint step_count = 0;

    while(!data.front.empty())
    {
        // pop the first edge in the front
        uint v0 = data.front.front();
        data.front.pop_front();
        uint v1 = data.front.front();
        data.front.pop_front();

        // pick the edge ID
        int eid = data.m0.edge_id(v0,v1);
        // if the edge does not exist anymore or it's not marked, it was already processed
        if(eid<0 || !data.m0.edge_data(eid).flags[MARKED]) continue;

        // pick the ID of the triangle to conquer to advance the front
        uint pid = data.m0.adj_e2p(eid).front();
        if(data.m0.poly_data(pid).flags[MARKED])
        {
            pid = data.m0.adj_e2p(eid).back();
            assert(!data.m0.poly_data(pid).flags[MARKED]);
        }

        // count front edges in pid, for classification
        uint front_edges = data.m0.edge_data(data.m0.poly_edge_id(pid,0)).flags[MARKED] +
                           data.m0.edge_data(data.m0.poly_edge_id(pid,1)).flags[MARKED] +
                           data.m0.edge_data(data.m0.poly_edge_id(pid,2)).flags[MARKED];

        auto tic = std::chrono::steady_clock::now();

        switch(front_edges)
        {
            case 1:
            {
                if(advance_by_triangle_split(data, pid, v0, v1))
                {
                    ++data.moves_tot;
                    ++data.moves_split;
                    ++step_count;
                }
                break;
            }

            case 2: // advance by edge flip or split
            {
                if(advance_by_edge_flip(data, pid))
                {
                    ++data.moves_tot;
                    ++data.moves_flip;
                    ++step_count;
                }
                break;
            }

            case 3: throw("hit a triangle with 3 front edges ... this is not supposed to happen");
            case 0: throw("hit a triangle with 0 front edges ... this is not supposed to happen");
        }

        auto toc = std::chrono::steady_clock::now();

        if(data.abort_if_too_slow && how_many_seconds(tic,toc)>data.max_time_per_step)
        {
            std::cout << "Time per single iteration is above " << data.max_time_per_step << "s\nEXIT" << std::endl;
            data.timeout = true;
            break;
        }

        if(data.step_by_step && step_count==data.step_size) return;

        if(data.stop)
        {
            data.stop = false;
            break;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void AFM_init(AFM_data & data)
{
    data.initialized = true;

    if(!data.m0.mesh_is_manifold())
    {
        std::cout << "ERROR: the input mesh is non manifold" << std::endl;
        exit(-1);
    }

    data.tris_in = data.m0.num_polys();

    // ensure the graph is 2-connected
    uint count = split_separating_simplices(data.m0);
    std::cout << count << " simplices were split to avoid inner elements with only boundary vertices" << std::endl;
    assert(split_separating_simplices(data.m0)==0);

    // add vertices all together to preserve id correspondences between all meshes
    data.m1.clear();
    for(uint vid=0; vid<data.m0.num_verts(); ++vid) data.m1.vert_add(vec3d(0,0,0));

    // find the source of the front as the innest point in m0
    // such a point always exists because of the previous refinement
    std::vector<uint> border = data.m0.get_ordered_boundary_vertices();
    std::vector<double> dist;
    dijkstra_exhaustive(data.m0, border, dist);
    data.origin = std::distance(dist.begin(), std::max_element(dist.begin(), dist.end()));
    if(data.m0.vert_is_boundary(data.origin)) throw("This is not supposed to happen");

    // map boundary to the target n-gon of choice
    std::vector<vec3d> target = n_sided_polygon(border.size(), data.target_domain);
    for(uint i=0; i<border.size(); ++i) data.m1.vert(border[i]) = target[i];

    // initialize front (also marking front edges and vertices in m0)
    data.m0.poly_set_flag(MARKED,false);
    data.m0.vert_set_flag(MARKED,false);
    data.m0.edge_set_flag(MARKED,false); // marked edges belong to the current front
    for(auto i=border.begin(),j=i+1; j!=border.end(); ++i,++j)
    {
        data.front.push_back(*i);
        data.front.push_back(*j);

        data.m0.vert_data(*i).flags[MARKED] = true;
        data.m0.vert_data(*j).flags[MARKED] = true;

        data.m1.poly_add(*i,*j,data.origin);

        int eid = data.m0.edge_id(*i,*j);
        assert(eid>=0);
        data.m0.edge_data(eid).flags[MARKED] = true;
    }
    data.front.push_back(*border.rbegin());
    data.front.push_back(*border.begin());
    data.m1.poly_add(*border.rbegin(),*border.begin(), data.origin);
    int eid = data.m0.edge_id(*border.rbegin(), *border.begin());
    assert(eid>=0);
    data.m0.edge_data(eid).flags[MARKED] = true;

    data.m1.edge_mark_boundaries();

    // initialize rational coordinates
    if(!rationals_are_working()) throw("Rational numbers are not working!");
    data.exact_coords.resize(data.m1.num_verts()*3);
    for(uint vid=0; vid<data.m1.num_verts(); ++vid)
    {
        data.exact_coords[3*vid  ] = data.m1.vert(vid).x();
        data.exact_coords[3*vid+1] = data.m1.vert(vid).y();
        data.exact_coords[3*vid+2] = data.m1.vert(vid).z();
    }
}

}
