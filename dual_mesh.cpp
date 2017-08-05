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
#include <cinolib/dual_mesh.h>

namespace cinolib
{

CINO_INLINE
void dual_mesh(const Trimesh<>     & primal,
                     Polygonmesh<> & dual,
               const bool            with_clipped_cells)
{
    std::vector<vec3d>             dual_verts;
    std::vector<std::vector<uint>> dual_faces;
    dual_mesh(primal, dual_verts, dual_faces, with_clipped_cells);
    dual = Polygonmesh<>(dual_verts,dual_faces);
}


CINO_INLINE
void dual_mesh(const Trimesh<>                      & primal,
                     std::vector<vec3d>             & dual_verts,
                     std::vector<std::vector<uint>> & dual_faces,
               const bool                             with_clipped_cells)
{
    dual_verts.clear();
    dual_faces.clear();

    // Initialize vertices with face centroids
    dual_verts.resize(primal.num_polys());
    for(uint eid=0; eid<primal.num_polys(); ++eid)
    {
        dual_verts.at(eid) = primal.poly_centroid(eid);
    }

    // Add boundary vertices as well as boundary edges' midpoints
    std::map<uint,uint> e2verts;
    std::map<uint,uint> v2verts;
    for(uint vid=0; vid<primal.num_verts(); ++vid)
    {
        if (primal.vert_is_boundary(vid))
        {
            v2verts[vid] = dual_verts.size();
            dual_verts.push_back(primal.vert(vid));
        }
    }
    for(uint eid=0; eid<primal.num_edges(); ++eid)
    {
        if (primal.edge_is_boundary(eid))
        {
            e2verts[eid] = dual_verts.size();
            dual_verts.push_back(0.5 * (primal.edge_vert(eid,0) + primal.edge_vert(eid,1)));
        }
    }

    // Make polygons
    for(uint vid=0; vid<primal.num_verts(); ++vid)
    {
        bool clipped_cell = primal.vert_is_boundary(vid);
        if (clipped_cell && !with_clipped_cells) continue;

        std::vector<uint> f;
        std::vector<uint> f_ring = primal.vert_ordered_poly_ring(vid);
        for(uint fid : f_ring) f.push_back(fid);

        if (clipped_cell) // add boundary portion (vertex vid + boundary edges' midpoints)
        {
            std::vector<uint> e_star = primal.vert_ordered_edge_ring(vid);
            f.push_back(e2verts.at(e_star.back()));
            f.push_back(v2verts.at(vid));
            f.push_back(e2verts.at(e_star.front()));
        }

        dual_faces.push_back(f);
    }
}


}

