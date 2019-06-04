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
#include <cinolib/shortest_path_tree.h>
#include <cinolib/dijkstra.h>

namespace cinolib
{

/* Given a mesh and source vertex, computes the tree of shortest paths connecting
 * any vertex in the mesh with the root. The algorithm internally uses Dijkstra to
 * compute point to point distances, and generates the tree as described in
 *
 * https://en.wikipedia.org/wiki/Shortest-path_tree
*/

template<class M, class V, class E, class P>
CINO_INLINE
void shortest_path_tree(AbstractPolygonMesh<M,V,E,P> & m, const uint root, std::vector<bool> & tree)
{
    // if true, the edge is on the tree
    tree = std::vector<bool>(m.num_edges(), false);

    std::vector<double> dist;
    dijkstra_exhaustive(m, root, dist);

    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        if(vid==root) continue;

        // there may be multiple shortest paths from root to vid.
        // I store them all, and consistently choose the one with
        // lowest ID. This should avoid the generation of loops
        // (https://en.wikipedia.org/wiki/Shortest-path_tree)
        std::vector<uint> parent;
        for(uint nbr : m.adj_v2v(vid))
        {
            int eid = m.edge_id(vid, nbr); assert(eid>=0);
            if(dist.at(vid) == m.edge_length(eid) + dist.at(nbr))
            {
                parent.push_back(nbr);
            }
        }
        assert(!parent.empty());
        int eid = m.edge_id(vid, *std::min_element(parent.begin(), parent.end()));
        tree.at(eid) = true;
    }
}

}
