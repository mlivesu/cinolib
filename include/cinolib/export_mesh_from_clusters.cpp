/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/export_mesh_from_clusters.h>

namespace cinolib
{

// NOTE: this assumes clusters form simply connected polygons (i.e. no holes)
//
template<class M, class V, class E, class P>
CINO_INLINE
void export_mesh_from_clusters(const AbstractPolygonMesh<M,V,E,P> & m,
                                     Polygonmesh<M,V,E,P>         & m_out)
{
    std::map<int,std::unordered_set<uint>> l2e; // maps labels to edges (only edges on border count)

    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        std::unordered_set<int> labels;
        for(uint pid : m.adj_e2p(eid)) labels.insert(m.poly_data(pid).label);

        if(m.edge_valence(eid)==labels.size())
        {
            for(int l : labels) l2e[l].insert(eid);
        }
        else
        {
            // WARNING: if you stumbled here it means you have a non manifold edge with multiple
            // incident faces having same label. It might not be the best idea to polygonize the
            // clusters of such a mesh....
            assert(m.edge_is_manifold(eid));
        }
    }

    uint fresh_id = 0;
    std::map<uint,uint> v_map;

    std::vector<std::vector<uint>> polys;
    std::vector<vec3d>             verts;

    for(auto cluster : l2e)
    {
        std::unordered_set<uint> edges = cluster.second;
        uint seed = *edges.begin();

        std::vector<uint> poly;
        poly.push_back(m.edge_vert_id(seed,0));
        poly.push_back(m.edge_vert_id(seed,1));

        do
        {
            uint prev = poly.at(poly.size()-2);
            uint curr = poly.back();

            for(uint eid : m.adj_v2e(curr))
            {
                if(m.edge_contains_vert(eid,prev)) continue;
                if(CONTAINS(edges, eid)) poly.push_back(m.vert_opposite_to(eid, curr));
            }
        }
        while(poly.front()!=poly.back());
        poly.pop_back();

        for(uint & vid : poly)
        {
            auto query = v_map.find(vid);
            if(query==v_map.end())
            {
                verts.push_back(m.vert(vid));
                v_map[vid] = fresh_id;
                vid = fresh_id;
                ++fresh_id;
            }
            else vid = query->second;
        }

        // TODO: fix winding
        polys.push_back(poly);
    }

    m_out = Polygonmesh<M,V,E,P>(verts,polys);
}

}
