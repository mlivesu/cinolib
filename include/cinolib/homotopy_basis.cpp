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
#include <cinolib/shortest_path_tree.h>
#include <cinolib/mst.h>

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

    shortest_path_tree(m, root, tree);

    // Compute the cotree as the Maximum Spanning Tree of the dual of M,
    // without considering dual edges that cross edges of primal tree.
    //
    // I'm using a classical Minimum Spanning Tree algorithm (Prim's) with negative weights
    std::vector<float> edge_weights(m.num_edges(),0);
    std::vector<bool>  edge_mask(m.num_edges()); // restrict Dijkstra to the edges in tree only
    for(uint eid=0; eid<m.num_edges(); ++eid) edge_mask.at(eid) = !tree.at(eid);
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(tree.at(eid)) continue;
        std::vector<uint> tmp;
        edge_weights.at(eid) -= m.edge_length(eid);
        edge_weights.at(eid) -= dijkstra_mask_on_edges(m, m.edge_vert_id(eid,0), root, edge_mask, tmp);
        edge_weights.at(eid) -= dijkstra_mask_on_edges(m, m.edge_vert_id(eid,1), root, edge_mask, tmp);
    }
    MST_on_dual_mask_on_edges(m, edge_weights, tree, cotree); // use tree as edge mask

    // Find the edges neither in tree, nor in cotree
    std::vector<uint> generators;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(!tree.at(eid) && !cotree.at(eid)) generators.push_back(eid);
    }
    assert(m.genus()*2 == (int)generators.size());

    // Start from each such edge, and close a loop with its two endpoints
    basis.clear();
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
