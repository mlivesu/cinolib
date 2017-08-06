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
#include <cinolib/isosurface.h>
#include <cinolib/marching_tets.h>
#include <cinolib/tetrahedralization.h>

namespace cinolib
{

CINO_INLINE
Isosurface::Isosurface(const Tetmesh<> &m, const float iso_value) : m_ptr(&m), iso_value(iso_value)
{
    marching_tets(m, iso_value, coords, tris, t_norms, split_info);
}

CINO_INLINE
Trimesh<> Isosurface::export_as_trimesh() const
{
    return Trimesh<>(coords, tris);
}

CINO_INLINE
void Isosurface::tessellate(std::vector<double> & new_coords,
                            std::vector<uint>   & new_cells,
                            std::vector<float>  & new_field) const
{
    new_cells.clear();
    new_coords = m_ptr->vector_coords();
    new_field  = m_ptr->serialize_uvw(U_param);

    // add vertices at split points
    //
    std::map<ipair,int> v_map;
    for(auto obj : split_info)
    {
        v_map[obj.first] = new_coords.size()/3;

        uint   v_a   = obj.first.first;
        uint   v_b   = obj.first.second;
        double alpha = obj.second;
        vec3d  pos   = alpha*m_ptr->vert(v_a) + (1.0-alpha)*m_ptr->vert(v_b);
        float  f     = alpha*m_ptr->vert_data(v_a).uvw[0] + (1.0-alpha)*m_ptr->vert_data(v_b).uvw[0];

        new_coords.push_back(pos.x());
        new_coords.push_back(pos.y());
        new_coords.push_back(pos.z());
        new_field.push_back(f);
    }

    for(uint cid=0; cid<m_ptr->num_cells(); ++cid)
    {
        std::vector<uint>   edges_split;
        std::vector<uint>   edges_not_split;
        std::map<uint,uint> edges_vids;

        for(uint e=0; e<m_ptr->edges_per_cell(); ++e)
        {
            uint  v_a   = m_ptr->cell_vert_id(cid, TET_EDGES[e][0]);
            uint  v_b   = m_ptr->cell_vert_id(cid, TET_EDGES[e][1]);
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
                new_cells.push_back(m_ptr->cell_vert_id(cid, 0));
                new_cells.push_back(m_ptr->cell_vert_id(cid, 1));
                new_cells.push_back(m_ptr->cell_vert_id(cid, 2));
                new_cells.push_back(m_ptr->cell_vert_id(cid, 3));
                break;
            }

            case 3 : // decompose the tet into a tet and a prism having triangular section
            {
                assert(edges_vids.size() == 3);
                auto vids_it  = edges_vids.begin();
                int  tet_tip  = m_ptr->cell_shared_vert(cid, edges_split); assert(tet_tip!=-1);
                uint prism[6] =
                {
                    m_ptr->cell_vert_id(cid, TET_EDGES[edges_split[0]][0]),
                    m_ptr->cell_vert_id(cid, TET_EDGES[edges_split[1]][0]),
                    m_ptr->cell_vert_id(cid, TET_EDGES[edges_split[2]][0]),
                    (  vids_it)->second,
                    (++vids_it)->second,
                    (++vids_it)->second,
                };
                if ((int)prism[0] == tet_tip) prism[0] = m_ptr->cell_vert_id(cid, TET_EDGES[edges_split[0]][1]);
                if ((int)prism[1] == tet_tip) prism[1] = m_ptr->cell_vert_id(cid, TET_EDGES[edges_split[1]][1]);
                if ((int)prism[2] == tet_tip) prism[2] = m_ptr->cell_vert_id(cid, TET_EDGES[edges_split[2]][1]);
                //
                tetrahedralize_prism(prism, new_cells);
                //
                new_cells.push_back(tet_tip);
                new_cells.push_back(prism[3]);
                new_cells.push_back(prism[4]);
                new_cells.push_back(prism[5]);
                //
                fix_subtet_orientation(cid, 4, new_coords, new_cells);
                break;
            }

            case 4 : // decompose the tet into two prisms having triangular section
            {
                assert(edges_not_split.size() == 2);
                for(uint e : edges_not_split)
                {
                    uint top = TET_EDGES[e][0];
                    uint bot = TET_EDGES[e][1];
                    std::vector<uint> split_edges_top;
                    std::vector<uint> split_edges_bot;
                    for(uint i=0; i<3; ++i)
                    {
                        if (TET_INCIDENT_EDEGES[top][i] != e) split_edges_top.push_back(TET_INCIDENT_EDEGES[top][i]);
                        if (TET_INCIDENT_EDEGES[bot][i] != e) split_edges_bot.push_back(TET_INCIDENT_EDEGES[bot][i]);
                    }
                    assert(split_edges_top.size() == 2);
                    assert(split_edges_bot.size() == 2);
                    //
                    std::vector<uint> tmp;
                    tmp.push_back(split_edges_top.front());
                    tmp.push_back(split_edges_bot.front());
                    if (m_ptr->cell_shared_vert(cid, tmp) == -1)
                    {
                        std::swap(split_edges_top[0], split_edges_top[1]);
                        tmp.clear();
                        tmp.push_back(split_edges_top.front());
                        tmp.push_back(split_edges_bot.front());
                        assert(m_ptr->cell_shared_vert(cid, tmp) != -1);
                    }
                    //
                    uint prism[6] =
                    {
                        m_ptr->cell_vert_id(cid, top),
                        edges_vids.at(split_edges_top[0]),
                        edges_vids.at(split_edges_top[1]),
                        m_ptr->cell_vert_id(cid, bot),
                        edges_vids.at(split_edges_bot[0]),
                        edges_vids.at(split_edges_bot[1]),
                    };
                    //
                    tetrahedralize_prism(prism, new_cells);
                }
                fix_subtet_orientation(cid, 6, new_coords, new_cells);
                break;
            }

            default:
            {
                std::cerr << edges_split.size() << " edges traversed!!!!!" << std::endl;
                assert(false);
            }
        }
    }
}

CINO_INLINE
void Isosurface::fix_subtet_orientation(const uint                  cid,
                                        const uint                  n_subtets,
                                        const std::vector<double> & coords,
                                              std::vector<uint>   & cells) const
{
    bool og_tet_was_flipped = m_ptr->cell_data(cid).quality;

    for(uint i=1; i<=n_subtets; ++i)
    {
        uint  base_ptr = cells.size() - (4*i);
        uint  v0_ptr   = cells.at(base_ptr+0);
        uint  v1_ptr   = cells.at(base_ptr+1);
        uint  v2_ptr   = cells.at(base_ptr+2);
        uint  v3_ptr   = cells.at(base_ptr+3);
        vec3d v0(coords.at(3*v0_ptr+0), coords.at(3*v0_ptr+1), coords.at(3*v0_ptr+2));
        vec3d v1(coords.at(3*v1_ptr+0), coords.at(3*v1_ptr+1), coords.at(3*v1_ptr+2));
        vec3d v2(coords.at(3*v2_ptr+0), coords.at(3*v2_ptr+1), coords.at(3*v2_ptr+2));
        vec3d v3(coords.at(3*v3_ptr+0), coords.at(3*v3_ptr+1), coords.at(3*v3_ptr+2));
        if (tet_scaled_jacobian(v0,v1,v2,v3) < 0 && !og_tet_was_flipped)
        {
            std::swap(cells.at(base_ptr+1),cells.at(base_ptr+3));
        }
    }
}

}
