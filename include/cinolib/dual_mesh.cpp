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
            std::vector<uint> v_ring; // sorted list of adjacent surfaces vertices
            std::vector<uint> e_ring; // sorted list of surface edges incident to vid
            std::vector<uint> f_ring; // sorted list of adjacent surface faces
            primal.vert_ordered_srf_one_ring(vid, v_ring, e_ring, f_ring);

            // make sure surface rings are CCW ordered
//            uint e0 = e_ring.at(0);
//            uint e1 = e_ring.at(1);
//            uint f0 = f_ring.at(0);
//            assert(primal.face_contains_edge(f0,e0) && primal.face_contains_edge(f0,e1));
//            uint v0 = primal.vert_opposite_to(e0,vid);
//            uint v1 = primal.vert_opposite_to(e1,vid);
//            if(!primal.face_verts_are_CCW(f0,v1,v0))
//            {
//                REVERSE_VEC(e_ring);
//                REVERSE_VEC(f_ring);
//            }

            bool has_crease = false;
            // make sure you start tracing the face from a crease (if any)
            for(uint i=0; i<e_ring.size(); ++i)
            {
                if(primal.edge_data(e_ring.at(i)).flags[CREASE])
                {
                    has_crease = true;
                    if(i==0) break;
                    assert(primal.face_contains_edge(*f_ring.begin(), *e_ring.begin()));
                    assert(primal.face_contains_edge(*f_ring.begin(), *(e_ring.begin()+1)));
                    std::rotate(e_ring.begin(), e_ring.begin()+i, e_ring.end());
                    std::rotate(f_ring.begin(), f_ring.begin()+i, f_ring.end());
                    assert(primal.face_contains_edge(*f_ring.begin(), *e_ring.begin()));
                    assert(primal.face_contains_edge(*f_ring.begin(), *(e_ring.begin()+1)));
                    break;
                }
            }

            if(has_crease)
            {
                std::cout << "\n\nring around v" << vid << "(" << primal.vert_data(vid).flags[CORNER_CREASE] << ")" << std::endl;
                for(uint i=0; i<e_ring.size(); ++i)
                {
                    std::cout << "e" << e_ring[i] << "(" << primal.edge_data(e_ring[i]).flags[CREASE] << ")   f" << f_ring[i] << std::endl;
                }
            }

            // rotate around the ring, and close a face, splitting each time you hit a crease edge
            int  corner = (primal.vert_data(vid).flags[CORNER_CREASE]) ? pv2dv.at(vid) : -1;
            auto e_it   = e_ring.begin();
            auto f_it   = f_ring.begin();
            do
            {
                std::vector<uint> new_face;

                // if vid is a feature corner, add it to the dual
                if(corner>=0)
                {
                    new_face.push_back(corner);
                    if(has_crease)std::cout << "v" << vid << std::endl;
                }

                // if the face starts from a crease, add a vertex for primal edge
                if(primal.edge_data(*e_it).flags[CREASE])
                {
                    new_face.push_back(pe2dv.at(*e_it));
                    if(has_crease)std::cout << "e" << *e_it << std::endl;
                }

                do
                {
                    new_face.push_back(pf2dv.at(*f_it));
                    if(has_crease)std::cout << "f" << *f_it << std::endl;
                    ++e_it;
                    ++f_it;
                }
                while(e_it!=e_ring.end() && !primal.edge_data(*e_it).flags[CREASE]);

                // if the previous loop stopped at a crease, add a vertex for primal edge
                if(e_it!=e_ring.end())
                {
                    new_face.push_back(pe2dv.at(*e_it));
                    if(has_crease)std::cout << "e" << *e_it << std::endl;
                }
                else if(primal.edge_data(*e_ring.begin()).flags[CREASE])
                {
                    new_face.push_back(pe2dv.at(*e_ring.begin()));
                    if(has_crease)std::cout << "e" << *e_ring.begin() << std::endl;
                }

                faces.push_back(new_face);
            }
            while(e_it!=e_ring.end());
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

