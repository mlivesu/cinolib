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
#include <cinolib/mst.h>

namespace cinolib
{

// Implementations of Prim's algorithm for Minimum Spanning Tree Computation

template<class M, class V, class E, class P>
CINO_INLINE
void MST_on_dual_mask_on_edges(const AbstractPolygonMesh<M,V,E,P> & m,
                               const std::vector<float>           & weights,
                               const std::vector<bool>            & mask, // if mask[e] = true, the tree cannot connect the two polys that share it
                                     std::vector<bool>            & tree)
{
    std::vector<int>   prev(m.num_polys(), -1);
    std::vector<float> cost(m.num_polys(), inf_float);
    std::vector<bool>  dequeued(m.num_polys(), false);
    cost.at(0) = 0.f; // start with poly #0

    // enqueue all elements
    std::set<std::pair<float,uint>> q;
    for(uint pid=0; pid<m.num_polys(); ++pid) q.insert(std::make_pair(cost.at(pid), pid));

    // initialize an empty MST
    tree = std::vector<bool>(m.num_edges(), false);

    while(!q.empty())
    {
        uint pid = q.begin()->second;
        q.erase(q.begin());
        dequeued.at(pid) = true;

        if(prev.at(pid)!=-1) // add an edge to the MST
        {
            int eid = m.edge_shared(prev.at(pid), pid);
            tree.at(eid) = true;
        }

        for(uint nbr : m.adj_p2p(pid))
        {
            if(!dequeued.at(nbr)) // element is still in the queue
            {
                int eid = m.edge_shared(pid, nbr);
                if(mask.at(eid)) continue;
                if(cost.at(nbr) > weights.at(eid))
                {
                    // remove previous entry from queue
                    auto it = q.find(std::make_pair(cost.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);

                    // update values and queue
                    cost.at(nbr) = weights.at(eid);
                    prev.at(nbr) = pid;
                    q.insert(std::make_pair(weights.at(eid),nbr));
                }
            }
        }
    }
}

}
