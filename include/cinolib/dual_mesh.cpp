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
#include <cinolib/dual_mesh.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void dual_mesh(AbstractPolyhedralMesh<M,V,E,F,P> & primal,
               Polyhedralmesh<M,V,E,F,P>         & dual,
               const bool                          with_clipped_cells)
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
void dual_mesh(AbstractPolyhedralMesh<M,V,E,F,P> & primal,
               std::vector<vec3d>                & dual_verts,
               std::vector<std::vector<uint>>    & dual_faces,
               std::vector<std::vector<uint>>    & dual_polys,
               std::vector<std::vector<bool>>    & dual_polys_winding,
               const bool                          with_clipped_cells)
{
    // convenient renaming of standard flags
    enum
    {
        CREASE        = MARKED,
        LINE_CREASE   = UNUSED_0,
        CORNER_CREASE = UNUSED_1,
    };

    dual_verts.clear();
    dual_faces.clear();
    dual_polys.clear();
    dual_polys_winding.clear();

    // add one dual vertex for each primal poly
    dual_verts.resize(primal.num_polys());
    for(uint pid=0; pid<primal.num_polys(); ++pid)
    {
        dual_verts.at(pid) = primal.poly_centroid(pid);
    }

    // vertex maps for clipped dual cells
    std::unordered_map<uint,uint> pv2dv; // primal vert to dual vert : for crease corners
    std::unordered_map<uint,uint> pe2dv; // primal edge to dual vert : for crease lines
    std::unordered_map<uint,uint> pf2dv; // primal face to dual vert : for surface faces
    // verts
    for(uint vid=0; vid<primal.num_verts(); ++vid)
    {
        if(!primal.vert_is_on_srf(vid)) continue;
        uint n_creases = 0;
        for(uint eid : primal.vert_adj_srf_edges(vid))
        {
            if(primal.edge_data(eid).flags[CREASE]) ++n_creases;
        }
        if(n_creases==2)
        {
            primal.vert_data(vid).flags[LINE_CREASE] = true;
        }
        else if(n_creases> 2)
        {
            primal.vert_data(vid).flags[CORNER_CREASE] = true;
            pv2dv[vid] = dual_verts.size();
            dual_verts.push_back(primal.vert(vid));
        }
    }
    // edges
    for(uint eid=0; eid<primal.num_edges(); ++eid)
    {
        if(primal.edge_is_on_srf(eid) && primal.edge_data(eid).flags[CREASE])
        {
            pe2dv[eid] = dual_verts.size();
            dual_verts.push_back(primal.edge_sample_at(eid, 0.5));
        }
    }
    // faces
    for(uint fid=0; fid<primal.num_faces(); ++fid)
    {
        if(primal.face_is_on_srf(fid))
        {
            pf2dv[fid] = dual_verts.size();
            dual_verts.push_back(primal.face_centroid(fid));
        }
    }

    // build polyhedral cells
    std::map<std::vector<uint>,uint> f_map;
    for(uint vid=0; vid<primal.num_verts(); ++vid)
    {
        bool clipped = primal.vert_is_on_srf(vid);
        if(clipped && !with_clipped_cells) continue;

        std::vector<uint>              poly;
        std::vector<bool>              poly_winding;
        std::vector<std::vector<uint>> faces;

        // each edge defines a dual face...
        for(uint eid : primal.adj_v2e(vid))
        {
            std::vector<uint> face = primal.edge_ordered_poly_ring(eid);
            // for surface edges, add the centroid of the two faces incident at it, in the right order
            if(primal.edge_is_on_srf(eid))
            {
                assert(primal.edge_adj_srf_faces(eid).size() == 2);
                uint srf_beg = primal.edge_adj_srf_faces(eid).front();
                uint srf_end = primal.edge_adj_srf_faces(eid).back();
                uint p_beg = face.front();
                uint p_end = face.back();
                if(!primal.poly_contains_face(p_beg, srf_beg)) std::swap(srf_beg, srf_end);
                assert(primal.poly_contains_face(p_beg, srf_beg));
                assert(primal.poly_contains_face(p_end, srf_end));
                face.push_back(pf2dv.at(srf_end));
                face.push_back(pf2dv.at(srf_beg));
            }
            faces.push_back(face);
        }

        // clipped cells need extra faces
        if(clipped)
        {
            // for regular surface vertices just add a face with all the
            // centroids of the surface faces incident to the vertex
            if(!primal.vert_data(vid).flags[LINE_CREASE]   &&
               !primal.vert_data(vid).flags[CORNER_CREASE])
            {
                auto face = primal.vert_ordered_srf_face_ring(vid);
                for(uint & id : face) id = pf2dv.at(id);
                faces.push_back(face);
            }
            // for crease vertices you such a face must be split along crease lines...
            else
            {                
                std::vector<uint> vid_faces = primal.vert_ordered_srf_face_ring(vid);
                std::vector<uint> vid_faces_double = vid_faces;
                for(uint f : vid_faces) vid_faces_double.push_back(f);
                uint f1, f2, e12;
                uint i;
                uint found = 0;
                for(i=0; i<vid_faces.size(); ++i)
                {
                    f1 = vid_faces.at(i);
                    f2 = vid_faces.at((i+1)%vid_faces.size());
                    e12 = primal.face_shared_edge(f1,f2);
                    if(primal.edge_data(e12).flags[CREASE])
                    {
                        found++;
                        break;
                    }
                }
                assert(found == 1);

                std::vector<uint> face;
                face.push_back(pe2dv.at(e12));
                for(uint ii=i+1; ii<i+1+vid_faces.size(); ++ii)
                {
                    f1 = vid_faces_double.at(ii);
                    f2 = vid_faces_double.at((ii + 1) % vid_faces_double.size());
                    e12 = primal.face_shared_edge(f1, f2);
                    face.push_back(pf2dv.at(f1));
                    if(pe2dv.find(e12) != pe2dv.end())
                    {
                        found++;
                        face.push_back(pe2dv.at(e12));
                        // if I am a corner, add the vertex as well
                        if(CONTAINS(pv2dv,vid)) face.push_back(pv2dv.at(vid));
                        faces.push_back(face);
                        face.clear();
                        face.push_back(pe2dv.at(e12));
                    }
                }
                assert(found > 2);
            }
        }

        for(std::vector<uint> & face : faces)
        {
            std::vector<uint> sorted_f = face;
            sort(sorted_f.begin(), sorted_f.end());
            auto query = f_map.find(sorted_f);
            if(query == f_map.end())
            {
                uint fresh_id = dual_faces.size();
                f_map[sorted_f] = fresh_id;
                dual_faces.push_back(face);

                poly.push_back(fresh_id);
                poly_winding.push_back(true);
            }
            else
            {
                poly.push_back(query->second);
                poly_winding.push_back(false);
            }
        }

        dual_polys.push_back(poly);
        dual_polys_winding.push_back(poly_winding);
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

        std::vector<uint> poly = primal.vert_ordered_polys_star(vid);

        if (clipped_cell) // add boundary portion (vertex vid + boundary edges' midpoints)
        {
            std::vector<uint> e_star = primal.vert_ordered_edges_star(vid);
            poly.push_back(e2verts.at(e_star.back()));
            poly.push_back(v2verts.at(vid));
            poly.push_back(e2verts.at(e_star.front()));
        }

        dual_polys.push_back(poly);
    }
}

}

