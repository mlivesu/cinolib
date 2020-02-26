/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
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
#include <cinolib/coarse_layout.h>
#include <cinolib/bfs.h>
#include <queue>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void compute_coarse_quad_layout(Quadmesh<M,V,E,P> & m)
{
    m.vert_unmark_all();
    m.edge_unmark_all();
    m.poly_unmark_all();

    // find singular vertices
    std::queue<uint> sing_verts;
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        if (m.vert_is_singular(vid)) sing_verts.push(vid);
    }
    uint nv = sing_verts.size();

    // trace separatrices
    std::vector<bool> on_domain_border(m.num_edges(), false);
    while(!sing_verts.empty())
    {
        uint vid = sing_verts.front();
        sing_verts.pop();

        m.vert_data(vid).flags[MARKED] = true;

        for(uint eid : m.adj_v2e(vid))
        {
            if(m.edge_data(eid).flags[MARKED]) continue; // this path was already visited
            m.edge_data(eid).flags[MARKED] = true;

            for(uint curr : m.edge_chain(eid, m.vert_opposite_to(eid,vid)))
            {
                m.edge_data(curr).flags[MARKED] = true;
                on_domain_border.at(curr) = true;
            }
        }
    }

    // flood polys
    int patch_id = 0;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        if (m.poly_data(pid).flags[MARKED]) continue; // already visited
        std::unordered_set<uint> patch;
        bfs_on_dual_w_edge_barriers(m, pid, on_domain_border, patch);
        for(uint p : patch)
        {
            m.poly_data(p).label  = patch_id;
            m.poly_data(p).flags[MARKED] = true;
        }
        ++patch_id;
    }

    std::cout << "coarse quad layout:" << std::endl;
    std::cout << "\t" << nv            << " singular vertices found" << std::endl;
    std::cout << "\t" << patch_id      << " patches detected       " << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void compute_coarse_hex_layout(Hexmesh<M,V,E,P> & m)
{
    m.vert_unmark_all();
    m.edge_unmark_all();
    m.face_unmark_all();
    m.poly_unmark_all();

    // find singular vertices
    std::queue<uint> sing_edges;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {        
        if (m.edge_is_singular(eid)) sing_edges.push(eid);
    }

    uint ne = sing_edges.size();
    uint nv = 0;
    for(uint vid=0; vid<m.num_verts(); ++vid) if(m.vert_is_singular(vid)) ++nv;

    // trace interfaces
    std::vector<bool> on_domain_border(m.num_faces(), false);
    while(!sing_edges.empty())
    {
        uint eid = sing_edges.front();
        sing_edges.pop();

        m.edge_data(eid).flags[MARKED] = true;

        for(uint fid : m.adj_e2f(eid))
        {
            if(m.face_data(fid).flags[MARKED]) continue; // this interfaces was already created
            m.face_data(fid).flags[MARKED] = true;

            for(uint curr : m.face_sheet(fid))
            {
                m.face_data(curr).flags[MARKED] = true;
                on_domain_border.at(curr) = true;
            }
        }
    }

    // flood polys
    int patch_id = 0;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        if (m.poly_data(pid).flags[MARKED]) continue; // already visited
        std::unordered_set<uint> patch;
        bfs_on_dual_w_face_barriers(m, pid, on_domain_border, patch);
        for(uint p : patch)
        {
            m.poly_data(p).label  = patch_id;
            m.poly_data(p).flags[MARKED] = true;
        }
        ++patch_id;
    }

    std::cout << "coarse hex layout:" << std::endl;
    std::cout << "\t" << nv           << " singular vertices found" << std::endl;
    std::cout << "\t" << ne           << " singular edges    found" << std::endl;
    std::cout << "\t" << patch_id     << " patches  detected      " << std::endl;
}

}
