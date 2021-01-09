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
                           std::vector<std::vector<uint>> & network,
                     const FeatureNetworkOptions          & opt)
{
    std::vector<bool> visited(m.num_edges(),false);

    // find start points first
    std::unordered_set<uint> seeds;
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        std::vector<uint> incoming_creases;
        for(uint eid : m.adj_v2e(vid))
        {
            if(m.edge_data(eid).flags[CREASE]) incoming_creases.push_back(eid);
        }

        if(!incoming_creases.empty() && incoming_creases.size()!=2)
        {
            seeds.insert(vid);
        }
        else if(opt.split_lines_at_high_curvature_points && incoming_creases.size()==2)
        {
            uint  e0 = incoming_creases.front();
            uint  e1 = incoming_creases.back();
            vec3d u  = m.vert(vid) - m.vert(m.vert_opposite_to(e0,vid));
            vec3d v  = m.vert(m.vert_opposite_to(e1,vid)) - m.vert(vid);
            if(u.angle_deg(v)>opt.ang_thresh_deg)
            {
                seeds.insert(vid);
            }
        }
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // utility to flood a feature curve (stops at feature corners/endpoints or when a loop is closed)
    auto flood_feature = [&](const uint start, const uint eid) -> std::vector<uint>
    {
        std::vector<uint> feat_line;
        feat_line.push_back(start);
        assert(eid>=0);
        visited.at(eid) = true;
        std::queue<uint> q;
        q.push(m.vert_opposite_to(eid,start));
        while(!q.empty())
        {
            uint vid = q.front();
            feat_line.push_back(vid);
            q.pop();

            // stop if you hit a splitpoint along a curve
            if(CONTAINS(seeds,vid)) break;

            std::vector<uint> next_pool;
            for(uint nbr : m.adj_v2v(vid))
            {
                int eid = m.edge_id(vid,nbr);
                assert(eid>=0);
                if(m.edge_data(eid).flags[CREASE]) next_pool.push_back(eid);
            }
            if(next_pool.size()==2)
            {
                for(uint eid : next_pool)
                {
                    if(visited.at(eid)) continue;
                    visited.at(eid) = true;
                    q.push(m.vert_opposite_to(eid,feat_line.back()));
                }
            }
        }
        return feat_line;
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    for(uint vid : seeds)
    {
        for(uint eid : m.adj_v2e(vid))
        {
            if(!visited.at(eid) && m.edge_data(eid).flags[CREASE])
            {
                network.emplace_back(flood_feature(vid,eid));
            }
        }
    }
    // closed loops are not caught by the previous cycle
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(!visited.at(eid) && m.edge_data(eid).flags[CREASE])
        {
            network.emplace_back(flood_feature(m.edge_vert_id(eid,0),eid));
        }
    }
}

}
