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
#include <cinolib/homotopy_basis.h>
#include <cinolib/homotopy_basis_detach_loops.h>
#include <cinolib/dijkstra.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis(Trimesh<M,V,E,P>               & m,
                    const uint                       root,
                    std::vector<std::vector<uint>> & basis,
                    const bool                       detach_loops,   // refine the mesh to make sure each edge is contained in at most one basis loop
                    const bool                       by_edge_splits) // true: use the edge_split strategy; false: use the vert_split strategy
{
    std::vector<bool> tree, cotree;
    homotopy_basis(m, root, basis, tree, cotree);
    if(detach_loops)
    {
        if(by_edge_splits) homotopy_basis_detach_loops_by_edge_split(m, root, basis);
        else               homotopy_basis_detach_loops_by_vert_split(m, root, basis);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis(AbstractPolygonMesh<M,V,E,P>   & m,
                    const uint                      root,
                    std::vector<std::vector<uint>> & basis,
                    std::vector<bool>              & tree,
                    std::vector<bool>              & cotree)
{
    assert(root<m.num_verts());

    basis.clear();
    tree   = std::vector<bool>(m.num_edges(), false);
    cotree = std::vector<bool>(m.num_edges(), false);

    m.vert_unmark_all();
    m.poly_unmark_all();

    std::queue<uint> q;
    q.push(root);
    m.vert_data(root).marked = true;

    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        for(uint nbr : m.adj_v2v(vid))
        {
            if(!m.vert_data(nbr).marked)
            {
                m.vert_data(nbr).marked = true;
                q.push(nbr);
                int eid = m.edge_id(vid, nbr);
                assert(eid>=0);
                tree.at(eid) = true;
            }
        }
    }

    assert(q.empty());
    uint pid = m.adj_v2p(root).front();
    q.push(pid);
    m.poly_data(pid).marked = true;

    while(!q.empty())
    {
        uint pid = q.front();
        q.pop();

        for(uint nbr : m.adj_p2p(pid))
        {
            if(!m.poly_data(nbr).marked)
            {
                int eid = m.edge_shared(pid, nbr);
                assert(eid>=0);
                if(!tree.at(eid))
                {
                    q.push(nbr);
                    m.poly_data(nbr).marked = true;
                    cotree.at(eid) = true;
                }
            }
        }
    }

    // find edges neither in tree, nor in cotree
    std::vector<uint> generators;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(!tree.at(eid) && !cotree.at(eid)) generators.push_back(eid);
    }
    assert(m.genus()*2 == (int)generators.size());

    // to restrict Dijkstra to the edges in tree only
    std::vector<bool> edge_mask(m.num_edges());
    for(uint eid=0; eid<m.num_edges(); ++eid) edge_mask.at(eid) = !tree.at(eid);

    // start from each edge in loop generator and close a loop with its two endpoints
    for(uint eid : generators)
    {
        std::vector<uint> e0_to_root, e1_to_root;
        dijkstra_mask_on_edges(m, m.edge_vert_id(eid,0), root, edge_mask, e0_to_root);
        dijkstra_mask_on_edges(m, m.edge_vert_id(eid,1), root, edge_mask, e1_to_root);
        e1_to_root.pop_back();
        std::reverse(e1_to_root.begin(), e1_to_root.end());
        std::copy(e1_to_root.begin(), e1_to_root.end(), std::back_inserter(e0_to_root));
        basis.push_back(e0_to_root);
    }
}

}
