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
#include <cinolib/dual_mesh.h>

namespace cinolib
{

CINO_INLINE
void dual_mesh(const Trimesh       & primal,
                     Polygonmesh<> & dual,
               const bool            with_clipped_cells)
{
    std::vector<vec3d>             dual_verts;
    std::vector<std::vector<uint>> dual_faces;
    dual_mesh(primal, dual_verts, dual_faces, with_clipped_cells);
    dual = Polygonmesh<>(dual_verts,dual_faces);
}


CINO_INLINE
void dual_mesh(const Trimesh                        & primal,
                     std::vector<vec3d>             & dual_verts,
                     std::vector<std::vector<uint>> & dual_faces,
               const bool                             with_clipped_cells)
{
    dual_verts.clear();
    dual_faces.clear();

    // Initialize vertices with face centroids
    dual_verts.resize(primal.num_triangles());
    for(uint tid=0; tid<primal.num_triangles(); ++tid)
    {
        dual_verts.at(tid) = primal.element_barycenter(tid);
    }

    // Add boundary vertices as well as boundary edges' midpoints
    std::map<uint,uint> e2verts;
    std::map<uint,uint> v2verts;
    for(int vid=0; vid<primal.num_vertices(); ++vid)
    {
        if (primal.vertex_is_boundary(vid))
        {
            v2verts[vid] = dual_verts.size();
            dual_verts.push_back(primal.vertex(vid));
        }
    }
    for(int eid=0; eid<primal.num_edges(); ++eid)
    {
        if (primal.edge_is_boundary(eid))
        {
            e2verts[eid] = dual_verts.size();
            dual_verts.push_back(0.5 * (primal.edge_vertex(eid,0) + primal.edge_vertex(eid,1)));
        }
    }

    // Make polygons
    for(int vid=0; vid<primal.num_vertices(); ++vid)
    {
        bool clipped_cell = primal.vertex_is_boundary(vid);
        if (clipped_cell && !with_clipped_cells) continue;

        std::vector<uint> f;
        std::vector<uint> ring = primal.adj_vtx2tri_ordered(vid);
        for(uint tid : ring) f.push_back(tid);

        if (clipped_cell) // add boundary portion (vertex vid + boundary edges' midpoints)
        {
            std::vector<uint> e_star = primal.adj_vtx2edg_ordered(vid);
            f.push_back(e2verts.at(e_star.back()));
            f.push_back(v2verts.at(vid));
            f.push_back(e2verts.at(e_star.front()));
        }

        dual_faces.push_back(f);
    }
}


}

