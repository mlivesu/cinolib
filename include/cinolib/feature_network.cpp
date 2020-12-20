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
#include <cinolib/feature_network.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void feature_network(const AbstractPolygonMesh<M,V,E,P>   & m,
                           std::vector<std::vector<uint>> & network)
{
    std::vector<bool> visited(m.num_edges(),false);

    // utility to flood a feature curve (stops at feature corners)
    auto flood_feature = [&](const uint source) -> std::vector<uint>
    {
        std::vector<uint> feat_line;
        std::queue<uint> q;
        q.push(source);
        visited.at(source) = true;
        while(!q.empty())
        {
            uint eid = q.front();
            feat_line.push_back(eid);
            q.pop();
            for(uint vid : m.adj_e2v(eid))
            {
                std::vector<uint> next;
                for(uint nbr : m.adj_v2e(vid)) if(nbr!=eid && m.edge_data(nbr).flags[CREASE]) next.push_back(nbr);
                // flood only if it's not a corner
                if(next.size()==1 && !visited.at(next.front()))
                {
                    visited.at(next.front()) = true;
                    q.push(next.front());
                }
            }
        }
        return feat_line;
    };

    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(m.edge_data(eid).flags[CREASE] && !visited.at(eid))
        {
            network.emplace_back(flood_feature(eid));
        }
    }
}

}
