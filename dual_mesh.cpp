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

//template<class M, class V, class E, class F, class P>
//CINO_INLINE
//void dual_mesh(const AbstractPolyhedralMesh<M,V,E,F,P> & primal,
//                     std::vector<vec3d>                & dual_verts,
//                     std::vector<std::vector<uint>>    & dual_faces,
//                     std::vector<std::vector<uint>>    & dual_polys,
//                     std::vector<std::vector<uint>>    & dual_polys_winding,
//               const bool                                with_clipped_cells)
//{
//    dual_verts.clear();
//    dual_faces.clear();
//    dual_polys.clear();
//    dual_polys_winding.clear();

//    // Initialize vertices with face centroids
//    dual_verts.resize(primal.num_polys());
//    for(uint pid=0; pid<primal.num_polys(); ++pid)
//    {
//        dual_verts.at(pid) = primal.poly_centroid(pid);
//    }

//    // Make polyhedra
//    for(uint vid=0; vid<primal.num_verts(); ++vid)
//    {
//        bool clipped_cell = primal.vert_is_boundary(vid);
//        if (clipped_cell && !with_clipped_cells) continue;

//        std::vector<uint> f;
//        std::vector<uint> f_ring = primal.edge_ordered_poly_ring(vid); // TODO!!!
//        for(uint fid : f_ring) f.push_back(fid);
//    }
//}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dual_mesh(const AbstractPolygonMesh<M,V,E,P> & primal,
                     Polygonmesh<M,V,E,P>         & dual,
               const bool                           with_clipped_cells)
{
    std::vector<vec3d>             dual_verts;
    std::vector<std::vector<uint>> dual_faces;
    dual_mesh(primal, dual_verts, dual_faces, with_clipped_cells);
    dual = Polygonmesh<M,V,E,P>(dual_verts,dual_faces);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dual_mesh(const AbstractPolygonMesh<M,V,E,P>   & primal,
                     std::vector<vec3d>             & dual_verts,
                     std::vector<std::vector<uint>> & dual_polys,
               const bool                             with_clipped_cells)
{
    dual_verts.clear();
    dual_polys.clear();

    // Initialize vertices with face centroids
    dual_verts.resize(primal.num_polys());
    for(uint pid=0; pid<primal.num_polys(); ++pid)
    {
        dual_verts.at(pid) = primal.poly_centroid(pid);
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

        std::vector<uint> poly = primal.vert_ordered_poly_ring(vid);

        if (clipped_cell) // add boundary portion (vertex vid + boundary edges' midpoints)
        {
            std::vector<uint> e_star = primal.vert_ordered_edge_ring(vid);
            poly.push_back(e2verts.at(e_star.back()));
            poly.push_back(v2verts.at(vid));
            poly.push_back(e2verts.at(e_star.front()));
        }

        dual_polys.push_back(poly);
    }
}


}

