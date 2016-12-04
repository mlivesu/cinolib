/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include <cinolib/meshes/tetmesh/isosurface.h>
#include <cinolib/meshes/tetmesh/marching_tets.h>
#include <cinolib/tetrahedralization.h>

namespace cinolib
{

CINO_INLINE
Isosurface::Isosurface(const Tetmesh & m, const float iso_value) : m_ptr(&m), iso_value(iso_value)
{
    marching_tets(m, iso_value, coords, tris, t_norms, split_info);
}

CINO_INLINE
Trimesh Isosurface::export_as_trimesh() const
{
    return Trimesh(coords, tris);
}

CINO_INLINE
void Isosurface::tessellate(std::vector<double> & new_coords,
                            std::vector<uint>   & new_tets,
                            std::vector<float>  & new_field) const
{
    assert(new_coords.empty() && new_tets.empty());

    new_coords = m_ptr->vector_coords();
    std::map<ipair,int> v_map;

    new_field = m_ptr->vector_v_float_scalar();

    // add vertices at split points
    for(auto obj : split_info)
    {
        v_map[obj.first] = new_coords.size()/3;

        int    v_a   = obj.first.first;
        int    v_b   = obj.first.second;
        double alpha = obj.second;
        vec3d  pos   = alpha*m_ptr->vertex(v_a) + (1.0-alpha)*m_ptr->vertex(v_b);
        float  f     = alpha*m_ptr->vertex_u_text(v_a) + (1.0-alpha)*m_ptr->vertex_u_text(v_b);

        new_coords.push_back(pos.x());
        new_coords.push_back(pos.y());
        new_coords.push_back(pos.z());

        new_field.push_back(f);
    }

    for(int tid=0; tid<m_ptr->num_tetrahedra(); ++tid)
    {
        std::vector<int>  edges_split;
        std::vector<int>  edges_not_split;
        std::map<int,int> edges_vids;

        for(int e=0; e<6; ++e)
        {
            int   v_a   = m_ptr->tet_vertex_id(tid, TET_EDGES[e][0]);
            int   v_b   = m_ptr->tet_vertex_id(tid, TET_EDGES[e][1]);
            ipair eid   = unique_pair(v_a,v_b);
            auto  query = v_map.find(eid);

            if (query != v_map.end())
            {
                edges_split.push_back(e);
                edges_vids[e] = query->second;
            }
            else edges_not_split.push_back(e);
        }

        switch (edges_split.size())
        {
            case 0 : // replicate the whole tet
            {
                new_tets.push_back(m_ptr->tet_vertex_id(tid, 0));
                new_tets.push_back(m_ptr->tet_vertex_id(tid, 1));
                new_tets.push_back(m_ptr->tet_vertex_id(tid, 2));
                new_tets.push_back(m_ptr->tet_vertex_id(tid, 3));
                break;
            }

            case 3 : // decompose the tet into a tet and a prism having triangular section
            {
                assert(edges_vids.size() == 3);
                auto vids_it  = edges_vids.begin();
                int  tet_tip  = m_ptr->tet_shared_vertex(tid, edges_split); assert(tet_tip!=-1);
                int  prism[6] =
                {
                    m_ptr->tet_vertex_id(tid, TET_EDGES[edges_split[0]][0]),
                    m_ptr->tet_vertex_id(tid, TET_EDGES[edges_split[1]][0]),
                    m_ptr->tet_vertex_id(tid, TET_EDGES[edges_split[2]][0]),
                    (  vids_it)->second,
                    (++vids_it)->second,
                    (++vids_it)->second,
                };
                if (prism[0] == tet_tip) prism[0] = m_ptr->tet_vertex_id(tid, TET_EDGES[edges_split[0]][1]);
                if (prism[1] == tet_tip) prism[1] = m_ptr->tet_vertex_id(tid, TET_EDGES[edges_split[1]][1]);
                if (prism[2] == tet_tip) prism[2] = m_ptr->tet_vertex_id(tid, TET_EDGES[edges_split[2]][1]);
                //
                tetrahedralize_prism(prism, new_tets);
                //
                new_tets.push_back(tet_tip);
                new_tets.push_back(prism[3]);
                new_tets.push_back(prism[4]);
                new_tets.push_back(prism[5]);
                //
                bool og_tet_was_flipped = m_ptr->tet_quality(tid) < 0;
                for(int i=1; i<=4; ++i)
                {
                    int   base_ptr = new_tets.size() - (4*i);
                    int   v0_ptr   = new_tets.at(base_ptr+0);
                    int   v1_ptr   = new_tets.at(base_ptr+1);
                    int   v2_ptr   = new_tets.at(base_ptr+2);
                    int   v3_ptr   = new_tets.at(base_ptr+3);
                    vec3d v0(new_coords.at(3*v0_ptr+0), new_coords.at(3*v0_ptr+1), new_coords.at(3*v0_ptr+2));
                    vec3d v1(new_coords.at(3*v1_ptr+0), new_coords.at(3*v1_ptr+1), new_coords.at(3*v1_ptr+2));
                    vec3d v2(new_coords.at(3*v2_ptr+0), new_coords.at(3*v2_ptr+1), new_coords.at(3*v2_ptr+2));
                    vec3d v3(new_coords.at(3*v3_ptr+0), new_coords.at(3*v3_ptr+1), new_coords.at(3*v3_ptr+2));
                    if (tet_scaled_jacobian(v0,v1,v2,v3) < 0 && !og_tet_was_flipped)
                    {
                        std::swap(new_tets.at(base_ptr+1),new_tets.at(base_ptr+3));
                    }
                }
                break;
            }

            case 4 : // decompose the tet into two prisms having triangular section
            {
                assert(edges_not_split.size() == 2);
                for(int e : edges_not_split)
                {
                    int top = TET_EDGES[e][0];
                    int bot = TET_EDGES[e][1];
                    std::vector<int> split_edges_top;
                    std::vector<int> split_edges_bot;
                    for(int i=0; i<3; ++i)
                    {
                        if (TET_INCIDENT_EDEGES[top][i] != e) split_edges_top.push_back(TET_INCIDENT_EDEGES[top][i]);
                        if (TET_INCIDENT_EDEGES[bot][i] != e) split_edges_bot.push_back(TET_INCIDENT_EDEGES[bot][i]);
                    }
                    assert(split_edges_top.size() == 2);
                    assert(split_edges_bot.size() == 2);
                    //
                    std::vector<int> tmp;
                    tmp.push_back(split_edges_top.front());
                    tmp.push_back(split_edges_bot.front());
                    if (m_ptr->tet_shared_vertex(tid, tmp) == -1)
                    {
                        std::swap(split_edges_top[0], split_edges_top[1]);
                        tmp.clear();
                        tmp.push_back(split_edges_top.front());
                        tmp.push_back(split_edges_bot.front());
                        assert(m_ptr->tet_shared_vertex(tid, tmp) != -1);
                    }
                    //
                    int prism[6] =
                    {
                        m_ptr->tet_vertex_id(tid, top),
                        edges_vids.at(split_edges_top[0]),
                        edges_vids.at(split_edges_top[1]),
                        m_ptr->tet_vertex_id(tid, bot),
                        edges_vids.at(split_edges_bot[0]),
                        edges_vids.at(split_edges_bot[1]),
                    };
                    //
                    tetrahedralize_prism(prism, new_tets);
                    //
                    bool og_tet_was_flipped = m_ptr->tet_quality(tid) < 0;
                    for(int i=1; i<=6; ++i)
                    {
                        int   base_ptr = new_tets.size() - (4*i);
                        int   v0_ptr   = new_tets.at(base_ptr+0);
                        int   v1_ptr   = new_tets.at(base_ptr+1);
                        int   v2_ptr   = new_tets.at(base_ptr+2);
                        int   v3_ptr   = new_tets.at(base_ptr+3);
                        vec3d v0(new_coords.at(3*v0_ptr+0), new_coords.at(3*v0_ptr+1), new_coords.at(3*v0_ptr+2));
                        vec3d v1(new_coords.at(3*v1_ptr+0), new_coords.at(3*v1_ptr+1), new_coords.at(3*v1_ptr+2));
                        vec3d v2(new_coords.at(3*v2_ptr+0), new_coords.at(3*v2_ptr+1), new_coords.at(3*v2_ptr+2));
                        vec3d v3(new_coords.at(3*v3_ptr+0), new_coords.at(3*v3_ptr+1), new_coords.at(3*v3_ptr+2));
                        if (tet_scaled_jacobian(v0,v1,v2,v3) < 0 && !og_tet_was_flipped)
                        {
                            std::swap(new_tets.at(base_ptr+1),new_tets.at(base_ptr+3));
                        }
                    }
                }
                break;
            }

            default:
            {
                std::cerr << edges_split.size() << " edges attraversati!!!!!" << std::endl;
                assert(false);
            }
        }
    }
}

}
