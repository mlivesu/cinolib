/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2024: Marco Livesu                                              *
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
#include <cinolib/stripe_embedding/stripe_embedding.h>
#include <cinolib/stripe_embedding/flip_checks.h>
#include <cinolib/split_separating_simplices.h>
#include <cinolib/geometry/n_sided_poygon.h>
#include <cinolib/how_many_seconds.h>

namespace cinolib
{

void embed_strip(SE_data & data, const std::vector<uint> & chain, const uint pivot)
{
    assert(data.embedded.at(chain.front()));
    assert(data.embedded.at(chain.back()));

    double delta_x = data.coords_d[2*chain.front()  ] - data.coords_d[2*chain.back()  ];
    double delta_y = data.coords_d[2*chain.front()+1] - data.coords_d[2*chain.back()+1];

    for(uint i=1; i<chain.size(); ++i)
    {
        int eid = data.m.edge_id(chain[i],chain[i-1]);
        assert(eid>=0);
        data.edge_chain_id.at(eid) = data.fresh_id;
        if(i==chain.size()-1) break; // the last one is already embedded

        assert( data.embedded.at(chain[i-1]));
        assert(!data.embedded.at(chain[i  ]));
        data.embedded.at(chain[i]) = true;
        data.embedded_verts++;

        double t = (chain.size()-i-1)/double(chain.size());
        data.coords_d.at(2*chain[i]  ) = data.coords_d[2*chain.back()  ] + t*delta_x;
        data.coords_d.at(2*chain[i]+1) = data.coords_d[2*chain.back()+1] + t*delta_y;

        if(data.use_rationals)
        {
            CGAL_Q t = (chain.size()-i-1)/double(chain.size());
            data.coords_q.at(2*chain[i]  ) = data.coords_q[2*chain.front()  ]*t + data.coords_q[2*chain.back()  ]*(1-t);
            data.coords_q.at(2*chain[i]+1) = data.coords_q[2*chain.front()+1]*t + data.coords_q[2*chain.back()+1]*(1-t);
        }

        if(data.use_MPFR)
        {
            mpfr::mpreal t = (chain.size()-i-1)/double(chain.size());
            data.coords_m.at(2*chain[i]  ) = data.coords_m[2*chain.front()  ]*t + data.coords_m[2*chain.back()  ]*(1-t);
            data.coords_m.at(2*chain[i]+1) = data.coords_m[2*chain.front()+1]*t + data.coords_m[2*chain.back()+1]*(1-t);
        }
    }
    ++data.fresh_id;

    if(data.store_stripes) // just for visuals
    {
        data.stripes_offset.push_back(data.stripes.size());
        data.stripes.push_back(pivot);
        for(uint vid : chain) data.stripes.push_back(vid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void init(SE_data & data)
{
    if(!data.m.mesh_is_manifold())
    {
        std::cout << "ERROR: the input mesh is non manifold" << std::endl;
        exit(-1);
    }

    if(data.m.Euler_characteristic()!=1)
    {
        std::cout << "Mesh has no boundary. Removing first triangle to make one" << std::endl;
        data.m.poly_remove(0);
    }

    if(data.use_MPFR) mpfr::mpreal::set_default_prec(data.MPFR_precision);

    // split non collapsible edges
    for(uint eid=0; eid<data.m.num_edges(); ++eid)
    {
        if(!data.m.edge_is_topologically_collapsible(eid))
        {
            data.m.edge_split(eid);
            data.edge_splits++;
        }
    }

    // ensure the graph is 2-connected
    data.edge_splits += split_separating_simplices(data.m);
    assert(split_separating_simplices(data.m)==0);

    data.embedded.resize(data.m.num_verts(),false);   // true if a vertex has been embedded
    data.edge_chain_id.resize(data.m.num_edges(),-1);
    data.m.vert_order_all_one_rings();

    data.coords_d.resize(data.m.num_verts()*2,0);
    if(data.use_rationals) data.coords_q.resize(data.m.num_verts()*2,0);
    if(data.use_MPFR)      data.coords_m.resize(data.m.num_verts()*2,0);

    // if there are no boundary conditions, map to the polygon prescribed in data.target_domain
    data.boundary = data.m.get_ordered_boundary_vertices();
    if(data.bc.empty())
    {
        std::vector<vec3d> target = n_sided_polygon(data.boundary.size(), data.target_domain);
        for(uint i=0; i<data.boundary.size(); ++i) data.bc[data.boundary.at(i)] = target.at(i);
    }
    for(auto & it : data.bc)
    {
        uint  vid = it.first;
        vec3d pos = it.second;

        assert(!data.embedded[vid]);
        data.embedded.at(vid) = true;
        data.embedded_verts++;
        data.coords_d.at(2*vid  ) = pos.x();
        data.coords_d.at(2*vid+1) = pos.y();

        if(data.use_rationals)
        {
            data.coords_q.at(2*vid  ) = pos.x();
            data.coords_q.at(2*vid+1) = pos.y();
        }
        if(data.use_MPFR)
        {
            data.coords_m.at(2*vid  ) = pos.x();
            data.coords_m.at(2*vid+1) = pos.y();
        }
    }

    // push ears to inizialize stripe decomposition
    for(uint i=0; i<data.boundary.size(); ++i)
    {
        int eid = data.m.edge_id(data.boundary.at(i), data.boundary.at((i+1)%data.boundary.size()));
        data.edge_chain_id.at(eid) = 0;

        uint & v0 = data.boundary.at(i);
        uint & v1 = data.boundary.at((i+1)%data.boundary.size());
        uint & v2 = data.boundary.at((i+2)%data.boundary.size());
        if(!flipped_d(data,v0,v1,v2)) data.q.push(v1);
    }
    assert(!data.q.empty());

    data.initialized = true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

int chain_starting_index(SE_data & data, const uint pivot)
{
    for(uint i=0; i<data.m.adj_v2v(pivot).size(); ++i)
    {
        uint curr = data.m.adj_v2v(pivot).at(i);
        uint next = data.m.adj_v2v(pivot).at((i+1)%data.m.adj_v2v(pivot).size());
        if(data.embedded.at(curr) && !data.embedded.at(next)) return i;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool colinearity_test_passed(SE_data & data, const std::vector<uint> & chain)
{
    for(uint eid0 : data.m.adj_v2e(*chain.begin()))
    for(uint eid1 : data.m.adj_v2e(*chain.rbegin()))
    {
        // TODO: FOR THE SQUARE NEED COLINEARITY CHECK ALSO FOR LABEL ZERO!
        if(data.edge_chain_id.at(eid0)>0 && data.edge_chain_id.at(eid1)>0)
        {
            if(data.edge_chain_id.at(eid0)==data.edge_chain_id.at(eid1)) return false;
        }
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<uint> make_chain(SE_data & data, const uint pivot)
{
    std::vector<uint> chain;
    int i = chain_starting_index(data,pivot);
    if(i<0) return chain;

    auto & ring = data.m.adj_v2v(pivot);
    chain.push_back(ring.at(i));
    do
    {
        i = (i+1)%ring.size();
        chain.push_back(ring.at(i));
    }
    while(!data.embedded.at(chain.back()));
    return chain;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool expand_strips_around_pivot(SE_data & data, const uint pivot)
{
    std::vector<uint> chain = make_chain(data,pivot);
    if(chain.empty() || !colinearity_test_passed(data,chain)) return false;

    embed_strip(data,chain,pivot);
    for(uint vid : chain) data.q.push(vid);
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void stripe_embedding(SE_data & data)
{
    std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

    if(!data.initialized) init(data);

    uint count = 0;
    while(!data.q.empty())
    {
        uint vid = data.q.front();
        data.q.pop();
        if(expand_strips_around_pivot(data,vid)) ++count;
        ++data.iters;        
        if(chain_starting_index(data,vid)>=0) data.q.push(vid);
        if(data.stop || (data.step_by_step && count==data.step_size)) break;
    }

    if(data.embedded_verts==data.m.num_verts())
    {
        data.converged = true;
        for(uint pid=0; pid<data.m.num_polys(); ++pid)
        {
            uint v0 = data.m.poly_vert_id(pid,0);
            uint v1 = data.m.poly_vert_id(pid,1);
            uint v2 = data.m.poly_vert_id(pid,2);
            if(                      flipped_d(data,v0,v1,v2)) data.flips_d++;
            if(data.use_rationals && flipped_q(data,v0,v1,v2)) data.flips_q++;
            if(data.use_MPFR      && flipped_m(data,v0,v1,v2)) data.flips_m++;
        }
    }

    std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
    data.runtime = how_many_seconds(tic,toc);
}

}
