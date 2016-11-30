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
void Isosurface::tessellate(Tetmesh & m) const
{
    std::vector<double> coords = m.vector_coords();
    std::vector<u_int>  tets ; //= m.vector_tets();
    std::map<ipair,int> v_map;

    // add vertices at split points
    for(auto obj : split_info)
    {
        v_map[obj.first] = coords.size()/3;

        int    v_a   = obj.first.first;
        int    v_b   = obj.first.second;
        double alpha = obj.second;
        vec3d  pos   = alpha*m.vertex(v_a) + (1.0-alpha)*m.vertex(v_b);

        coords.push_back(pos.x());
        coords.push_back(pos.y());
        coords.push_back(pos.z());
    }

    for(int tid=0; tid<m.num_tetrahedra(); ++tid)
    {
        int vids[] =
        {
            m.tet_vertex_id(tid, 0),
            m.tet_vertex_id(tid, 1),
            m.tet_vertex_id(tid, 2),
            m.tet_vertex_id(tid, 3)
        };

        float func[] =
        {
            m.vertex_u_text(vids[0]),
            m.vertex_u_text(vids[1]),
            m.vertex_u_text(vids[2]),
            m.vertex_u_text(vids[3])
        };

        u_char c = 0x0;
        if (iso_value >= func[0]) c |= C_1000;
        if (iso_value >= func[1]) c |= C_0100;
        if (iso_value >= func[2]) c |= C_0010;
        if (iso_value >= func[3]) c |= C_0001;

        // handle corner cases (i.e. when the iso-surface passes EXACTLY on a vertex/edge/face)...
        switch (c)
        {
            /* there are some other cases to take into account!
             * like iso-srf passing on a vertex and two edges....
            */

            // iso-surface passes on a vertex : do nothing
            case C_1000 : { if (func[0] == iso_value) c = C_0000; break; }
            case C_0100 : { if (func[1] == iso_value) c = C_0000; break; }
            case C_0010 : { if (func[2] == iso_value) c = C_0000; break; }
            case C_0001 : { if (func[3] == iso_value) c = C_0000; break; }

            // iso-surface passes on a edge : do nothing
            case C_0101 : { if (func[1] == iso_value && func[3] == iso_value) c = C_0000; break; }
            case C_1010 : { if (func[0] == iso_value && func[2] == iso_value) c = C_0000; break; }
            case C_0011 : { if (func[2] == iso_value && func[3] == iso_value) c = C_0000; break; }
            case C_1100 : { if (func[0] == iso_value && func[1] == iso_value) c = C_0000; break; }
            case C_1001 : { if (func[0] == iso_value && func[3] == iso_value) c = C_0000; break; }
            case C_0110 : { if (func[1] == iso_value && func[2] == iso_value) c = C_0000; break; }

            // iso-surface passes on a face : do nothing
            case C_1110 : { if (func[0] == iso_value && func[1] == iso_value && func[2] == iso_value) c = C_0000; break; }
            case C_1101 : { if (func[0] == iso_value && func[1] == iso_value && func[3] == iso_value) c = C_0000; break; }
            case C_1011 : { if (func[0] == iso_value && func[2] == iso_value && func[3] == iso_value) c = C_0000; break; }
            case C_0111 : { if (func[1] == iso_value && func[2] == iso_value && func[3] == iso_value) c = C_0000; break; }

            default : break;            
        }

        switch (c)
        {

            /*
             * TODO:
             * - testa anche il caso in cui la superfice stacca un quad dal tetra e non un tri
             *   (usando il campo X anzichè Y dovrebbe succedere....)
             * - pensa a come compattare il codice per comprendere tutti le possibili permutazioni
             *   dei vertici per i due casi base (4 split/6 split)
            */

            case C_1110 :
            {
                ipair e0 = unique_pair(vids[TET_EDGES[4][0]], vids[TET_EDGES[4][1]]); assert(CONTAINS(v_map,e0));
                ipair e1 = unique_pair(vids[TET_EDGES[3][0]], vids[TET_EDGES[3][1]]); assert(CONTAINS(v_map,e1));
                ipair e2 = unique_pair(vids[TET_EDGES[5][0]], vids[TET_EDGES[5][1]]); assert(CONTAINS(v_map,e2));                
                //
                int prism[] =
                {
                    m.tet_vertex_id(tid,0),
                    m.tet_vertex_id(tid,1),
                    m.tet_vertex_id(tid,2),
                    v_map[e0],
                    v_map[e1],
                    v_map[e2]
                };
                //
                tets.push_back(prism[3]);
                tets.push_back(prism[4]);
                tets.push_back(prism[5]);
                tets.push_back(m.tet_vertex_id(tid,3));
                //
                tetrahedralize_prism(prism, tets);
                break;
            }

            case C_1010 :
            {
                ipair e0 = unique_pair(vids[TET_EDGES[1][0]], vids[TET_EDGES[1][1]]); assert(CONTAINS(v_map,e0));
                ipair e1 = unique_pair(vids[TET_EDGES[2][0]], vids[TET_EDGES[2][1]]); assert(CONTAINS(v_map,e1));
                ipair e2 = unique_pair(vids[TET_EDGES[4][0]], vids[TET_EDGES[4][1]]); assert(CONTAINS(v_map,e2));
                ipair e3 = unique_pair(vids[TET_EDGES[5][0]], vids[TET_EDGES[5][1]]); assert(CONTAINS(v_map,e2));
                //
                int prism_1[] =
                {
                    m.tet_vertex_id(tid,3),
                    v_map[e2],
                    v_map[e3],
                    m.tet_vertex_id(tid,1),
                    v_map[e1],
                    v_map[e0],
                };
                //
                int prism_2[] =
                {
                    m.tet_vertex_id(tid,0),
                    v_map[e1],
                    v_map[e2],
                    m.tet_vertex_id(tid,2),
                    v_map[e0],
                    v_map[e3],
                };
                //
                tetrahedralize_prism(prism_1, tets);
                tetrahedralize_prism(prism_2, tets);
                break;
            }

            default : break;
        }
    }

    m = Tetmesh(coords, tets);
}

}
