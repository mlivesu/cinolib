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

template<class M, class V, class E, class F, class P>
CINO_INLINE
void dual_mesh(const AbstractPolyhedralMesh<M,V,E,F,P> & primal,
                     Polyhedralmesh<M,V,E,F,P>         & dual,
               const bool                                with_clipped_cells)
{
    std::vector<vec3d>             dual_verts;
    std::vector<std::vector<uint>> dual_faces;
    std::vector<std::vector<uint>> dual_polys;
    std::vector<std::vector<bool>> dual_polys_winding;
    dual_mesh(primal, dual_verts, dual_faces, dual_polys, dual_polys_winding, with_clipped_cells);
    dual = Polyhedralmesh<M,V,E,F,P>(dual_verts, dual_faces, dual_polys, dual_polys_winding);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void dual_mesh(const AbstractPolyhedralMesh<M,V,E,F,P> & primal,
                     std::vector<vec3d>                & dual_verts,
                     std::vector<std::vector<uint>>    & dual_faces,
                     std::vector<std::vector<uint>>    & dual_polys,
                     std::vector<std::vector<bool>>    & dual_polys_winding,
               const bool                                with_clipped_cells)
{
    dual_verts.clear();
    dual_faces.clear();
    dual_polys.clear();
    dual_polys_winding.clear();

    // Initialize vertices with face centroids
    dual_verts.resize(primal.num_polys());
    for(uint pid=0; pid<primal.num_polys(); ++pid)
    {
        dual_verts.at(pid) = primal.poly_centroid(pid);
    }

    std::map<std::vector<uint>,uint> f_map;

    // For clipped dual cells: add boundary vertices,
    // boundary edges midpoints and surface face midpoints
    //
    //std::map<uint,uint> v2verts;
    //std::map<uint,uint> e2verts;
    std::map<uint,uint> f2verts;
    //for(uint vid=0; vid<primal.num_verts(); ++vid)
    //{
    //    if (primal.vert_is_on_srf(vid))
    //    {
    //        v2verts[vid] = dual_verts.size();
    //        dual_verts.push_back(primal.vert(vid));
    //    }
    //}
    //for(uint eid=0; eid<primal.num_edges(); ++eid)
    //{
    //    if (primal.edge_is_on_srf(eid))
    //    {
    //        e2verts[eid] = dual_verts.size();
    //        dual_verts.push_back(primal.edge_sample_at(eid, 0.5));
    //    }
    //}
    for(uint fid=0; fid<primal.num_faces(); ++fid)
    {
        if (primal.face_is_on_srf(fid))
        {
            f2verts[fid] = dual_verts.size();
            dual_verts.push_back(primal.face_centroid(fid));
        }
    }

    // Make dual polyhedral cells
    for(uint vid=0; vid<primal.num_verts(); ++vid)
    {
        bool clipped_cell = primal.vert_is_on_srf(vid);
        if (clipped_cell && !with_clipped_cells) continue;

        std::vector<uint> p;
        std::vector<bool> p_winding;

        for(uint eid : primal.adj_v2e(vid))
        {
            std::vector<uint> f = primal.edge_ordered_poly_ring(eid);

            if (primal.edge_is_on_srf(eid))
            {
                assert(primal.edge_adj_srf_faces(eid).size()==2);
                uint srf_beg = primal.edge_adj_srf_faces(eid).front();
                uint srf_end = primal.edge_adj_srf_faces(eid).back();
                uint p_beg   = f.front();
                uint p_end   = f.back();
                if (!primal.poly_contains_face(p_beg, srf_beg)) std::swap(srf_beg, srf_end);
                assert(primal.poly_contains_face(p_beg, srf_beg));
                assert(primal.poly_contains_face(p_end, srf_end));
                f.push_back(f2verts.at(srf_end));
                //f.push_back(e2verts.at(eid));
                f.push_back(f2verts.at(srf_beg));
            }

            std::vector<uint> sorted_f = f;
            sort(sorted_f.begin(), sorted_f.end());
            auto query = f_map.find(sorted_f);

            if (query == f_map.end())
            {
                uint fresh_id = dual_faces.size();
                f_map[sorted_f] = fresh_id;
                dual_faces.push_back(f);

                p.push_back(fresh_id);
                p_winding.push_back(true);
            }
            else
            {
                p.push_back(query->second);
                p_winding.push_back(false);
            }
        }

        if (clipped_cell)
        {
            std::vector<uint> f;
            for(uint fid : primal.vert_ordered_srf_face_ring(vid))
            {
                f.push_back(f2verts.at(fid));
            }
            std::vector<uint> sorted_f = f;
            sort(sorted_f.begin(), sorted_f.end());
            auto query = f_map.find(sorted_f);

            if (query == f_map.end())
            {
                uint fresh_id = dual_faces.size();
                f_map[sorted_f] = fresh_id;
                dual_faces.push_back(f);

                p.push_back(fresh_id);
                p_winding.push_back(true);
            }
            else
            {
                p.push_back(query->second);
                p_winding.push_back(false);
            }
        }

        dual_polys.push_back(p);
        dual_polys_winding.push_back(p_winding);
    }
}

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

    // For clipped dual cells: add boundary vertices
    // and boundary edges midpoints
    //
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
            dual_verts.push_back(primal.edge_sample_at(eid, 0.5));
        }
    }

    // Make dual polygonal cells
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

