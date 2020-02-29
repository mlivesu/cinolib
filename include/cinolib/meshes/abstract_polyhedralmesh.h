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
#ifndef CINO_ABSTRACT_POLYHEDRAL_MESH_H
#define CINO_ABSTRACT_POLYHEDRAL_MESH_H

#include <cinolib/meshes/abstract_mesh.h>
#include <cinolib/meshes/mesh_attributes.h>
#include <cinolib/ipair.h>

namespace cinolib
{

template<class M = Mesh_std_attributes,
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class F = Polygon_std_attributes,
         class P = Polyhedron_std_attributes>
class AbstractPolyhedralMesh : public AbstractMesh<M,V,E,P>
{
    protected:

        std::vector<std::vector<uint>> faces;              // list of faces (assumed CCW)
        std::vector<std::vector<bool>> polys_face_winding; // true if the face is CCW, false if it is CW

        std::vector<F> f_data;

        std::vector<std::vector<uint>> v2f; // vert to face adjacency
        std::vector<std::vector<uint>> e2f; // edge to face adjacency
        std::vector<std::vector<uint>> f2e; // face to edge adjacency
        std::vector<std::vector<uint>> f2f; // face to face adjacency (through edges)
        std::vector<std::vector<uint>> f2p; // face to poly adjacency
        std::vector<std::vector<uint>> p2v; // poly to vert adjacency

        std::vector<std::vector<uint>> face_triangles; // per face serialized triangulation (e.g., for rendering)

    public:

        typedef F F_type;

        explicit AbstractPolyhedralMesh() : AbstractMesh<M,V,E,P>() {}
        ~AbstractPolyhedralMesh() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void clear() override;

        void init(const std::vector<vec3d>             & verts,
                  const std::vector<std::vector<uint>> & faces,
                  const std::vector<std::vector<uint>> & polys,
                  const std::vector<std::vector<bool>> & polys_face_winding);

        void init(const std::vector<vec3d>             & verts,
                  const std::vector<std::vector<uint>> & polys);

        void init(const std::vector<vec3d>             & verts,
                  const std::vector<std::vector<uint>> & polys,
                  const std::vector<int>               & vert_labels,
                  const std::vector<int>               & poly_labels);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double mesh_srf_area() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int Euler_characteristic() const override;
        int genus() const override;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                void update_normals() override;
                void update_f_normals();
        virtual void update_f_normal(const uint fid) = 0;
                void update_f_tessellation();
                void update_f_tessellation(const uint fid);
                void update_v_normals();
                void update_v_normal(const uint vid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint verts_per_poly(const uint pid) const override { return this->p2v.at(pid).size();   }
        virtual uint faces_per_poly(const uint pid) const          { return this->polys.at(pid).size(); }
        virtual uint verts_per_face(const uint fid) const          { return this->faces.at(fid).size(); }
        virtual uint edges_per_face(const uint fid) const          { return this->faces.at(fid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_srf_verts() const;
        uint num_srf_edges() const;
        uint num_srf_faces() const;
        uint num_srf_polys() const;
        uint num_faces()     const { return faces.size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<std::vector<uint>> & vector_faces() const { return faces; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<uint> & adj_v2f(const uint vid) const          { return v2f.at(vid);         }
              std::vector<uint> & adj_v2f(const uint vid)                { return v2f.at(vid);         }
        const std::vector<uint> & adj_e2f(const uint eid) const          { return e2f.at(eid);         }
              std::vector<uint> & adj_e2f(const uint eid)                { return e2f.at(eid);         }
        const std::vector<uint> & adj_f2v(const uint fid) const          { return this->faces.at(fid); }
              std::vector<uint> & adj_f2v(const uint fid)                { return this->faces.at(fid); }
        const std::vector<uint> & adj_f2e(const uint fid) const          { return f2e.at(fid);         }
              std::vector<uint> & adj_f2e(const uint fid)                { return f2e.at(fid);         }
        const std::vector<uint> & adj_f2f(const uint fid) const          { return f2f.at(fid);         }
              std::vector<uint> & adj_f2f(const uint fid)                { return f2f.at(fid);         }
        const std::vector<uint> & adj_f2p(const uint fid) const          { return f2p.at(fid);         }
              std::vector<uint> & adj_f2p(const uint fid)                { return f2p.at(fid);         }
        const std::vector<uint> & adj_p2f(const uint pid) const          { return this->polys.at(pid); }
              std::vector<uint> & adj_p2f(const uint pid)                { return this->polys.at(pid); }
        const std::vector<uint> & adj_p2v(const uint pid) const override { return p2v.at(pid);         }
              std::vector<uint> & adj_p2v(const uint pid)       override { return p2v.at(pid);         }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const F & face_data(const uint fid) const { return f_data.at(fid); }
              F & face_data(const uint fid)       { return f_data.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // useful for GUIs with mouse picking
        uint pick_face(const vec3d & p) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void               vert_switch_id             (const uint vid0, const uint vid1);
        void               vert_remove                (const uint vid);
        void               vert_remove_unreferenced   (const uint vid);
        uint               vert_add                   (const vec3d & pos);
        bool               vert_is_on_srf             (const uint vid) const;
        double             vert_mass                  (const uint vid) const override;
        double             vert_volume                (const uint vid) const;
        bool               vert_is_manifold           (const uint vid) const;
        void               vert_local_unmark_near_face(const uint fid);
        std::vector<uint>  vert_verts_link            (const uint vid) const; // see https://en.wikipedia.org/wiki/Simplicial_complex#Closure,_star,_and_link for adefinition of link and star
        std::vector<uint>  vert_edges_link            (const uint vid) const;
        std::vector<uint>  vert_faces_link            (const uint vid) const;
        std::vector<ipair> vert_adj_visible_faces     (const uint vid, const vec3d dir, const double ang_thresh = 60.0);
        std::vector<uint>  vert_adj_srf_verts         (const uint vid) const;
        std::vector<uint>  vert_adj_srf_edges         (const uint vid) const;
        std::vector<uint>  vert_adj_srf_faces         (const uint vid) const;
        std::vector<uint>  vert_ordered_srf_vert_ring (const uint vid) const;
        std::vector<uint>  vert_ordered_srf_edge_ring (const uint vid) const;
        std::vector<uint>  vert_ordered_srf_face_ring (const uint vid) const;
        void               vert_ordered_srf_one_ring  (const uint vid,
                                                       std::vector<uint> & v_ring,
                                                       std::vector<uint> & e_ring,
                                                       std::vector<uint> & f_ring) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void              edge_switch_id             (const uint eid0, const uint eid1);
        uint              edge_add                   (const uint vid0, const uint vid1);
        bool              edge_is_manifold           (const uint eid) const;
        void              edge_remove                (const uint eid);
        void              edge_remove_unreferenced   (const uint eid);
        void              edge_local_unmark_near_face(const uint fid);
        bool              edge_is_on_srf             (const uint eid) const;
        bool              edge_is_incident_to_srf    (const uint eid) const;
        std::vector<uint> edge_ordered_poly_ring     (const uint eid) const;
        std::vector<uint> edge_adj_srf_faces         (const uint eid) const;
        std::vector<uint> edge_verts_link            (const uint eid) const;
        std::vector<uint> edge_edges_link            (const uint eid) const;
        std::vector<uint> edge_faces_link            (const uint eid) const;
        uint              edge_split                 (const uint eid, const vec3d & p);
        double            edge_dihedral_angle        (const uint eid) const override;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                vec3d              face_vert                  (const uint fid, const uint off) const;
                std::vector<vec3d> face_verts                 (const uint fid) const;
                std::vector<uint>  face_verts_id              (const uint fid, const bool sort_by_vid = false) const;
                uint               face_vert_id               (const uint fid, const uint off) const;
                uint               face_vert_offset           (const uint fid, const uint vid) const;
                uint               face_edge_id               (const uint fid, const uint vid0, const uint vid1) const;
                uint               face_edge_id               (const uint fid, const uint off) const;
                std::vector<uint>  face_v2e                   (const uint fid, const uint vid) const;
                std::vector<uint>  face_v2v                   (const uint fid, const uint vid) const;
                bool               face_is_on_srf             (const uint fid) const;
                bool               face_contains_vert         (const uint fid, const uint vid) const;
                bool               face_contains_edge         (const uint fid, const uint eid) const;
                bool               face_winding_agrees_with   (const uint fid, const uint vid0, const uint vid1) const;
                ipair              face_edges_from_vert       (const uint fid, const uint vid) const;
                uint               face_adj_srf_edge          (const uint fid, const uint eid, const uint vid) const;
                uint               face_opp_to_srf_edge       (const uint fid, const uint eid) const;
                uint               face_shared_edge           (const uint fid0, const uint fid1) const;
                vec3d              face_centroid              (const uint fid) const;
                double             face_mass                  (const uint fid) const;
                double             face_area                  (const uint fid) const;
                bool               faces_are_disjoint         (const uint fid0, const uint fid1) const;
                bool               faces_are_adjacent         (const uint fid0, const uint fid1) const;
                bool               faces_share_poly           (const uint fid0, const uint fid1) const;
                void               face_unmark_all            ();
                void               face_local_unmark_near_vert(const uint vid);
                void               face_local_unmark_near_edge(const uint eid);
                void               face_local_unmark_near_face(const uint fid);
                void               face_local_unmark_near_poly(const uint pid);
                int                face_id                    (const std::vector<uint> & f) const;
                bool               face_is_tri                (const uint fid) const;
                bool               face_is_quad               (const uint fid) const;
        virtual void               face_set_color             (const Color & c);
        virtual void               face_set_alpha             (const float alpha);
                void               face_switch_id             (const uint fid0, const uint fid1);
                uint               face_add                   (const std::vector<uint> & f);
                void               face_remove                (const uint fid);
                void               face_remove_unreferenced   (const uint fid);
                std::vector<uint>  face_tessellation          (const uint fid) const;
                bool               face_is_visible            (const uint fid, uint & pid_beneath) const;
                void               face_apply_labels          (const std::vector<int> & labels);
                void               face_apply_label           (const int label);
                bool               face_verts_are_CCW         (const uint fid, const uint curr, const uint prev) const;
                uint               face_split_along_new_edge  (const uint fid, const uint vid0, const uint vid1);
                uint               face_split_in_triangles    (const uint fid, const vec3d & p);
                bool               face_has_no_duplicate_verts(const uint fid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual double             poly_volume                 (const uint pid) const = 0;
                double             poly_mass                   (const uint pid) const override;
                bool               poly_contains_face          (const uint pid, const uint fid) const;
                bool               poly_is_on_surf             (const uint pid) const;
                int                poly_id                     (const std::vector<uint> & flist) const;
                int                poly_shared_face            (const uint pid0, const uint pid1) const;
                uint               poly_face_id                (const uint pid, const uint off) const;
                void               poly_face_flip_winding      (const uint pid, const uint fid);
                bool               poly_face_winding           (const uint pid, const uint fid) const;
                void               poly_flip_winding           (const uint pid);
                bool               poly_face_is_CCW            (const uint pid, const uint fid) const;
                bool               poly_face_is_CW             (const uint pid, const uint fid) const;
                void               poly_local_unmark_near_face (const uint fid);
                uint               poly_face_offset            (const uint pid, const uint fid) const;
                vec3d              poly_face_normal            (const uint pid, const uint fid) const;
                int                poly_adj_through_face       (const uint pid, const uint fid) const;
                std::vector<uint>  poly_v2f                    (const uint pid, const uint vid) const;
                std::vector<uint>  poly_e2f                    (const uint pid, const uint eid) const;
                std::vector<uint>  poly_f2f                    (const uint pid, const uint fid) const;
                void               poly_switch_id              (const uint pid0, const uint pid1);
                uint               poly_add                    (const std::vector<uint> & flist, const std::vector<bool> & fwinding);
        virtual uint               poly_add                    (const std::vector<uint> & vlist);
                void               poly_remove_unreferenced    (const uint pid);
                void               poly_remove                 (const uint pid);
                void               polys_remove                (const std::vector<uint> & pids);
                uint               poly_face_adj_through_edge  (const uint pid, const uint fid, const uint eid) const;
                bool               poly_faces_share_orientation(const uint pid, const uint fid0, const uint fid1) const;
                bool               poly_fix_orientation        (const uint pid, const uint fid);
                bool               poly_fix_orientation        ();
                int                poly_Euler_characteristic   (const uint pid) const;
                int                poly_genus                  (const uint pid) const;
                bool               poly_is_spherical           (const uint pid) const;
                void               poly_export_element         (const uint pid, std::vector<vec3d> & verts, std::vector<std::vector<uint>> & faces) const override;
                std::vector<uint>  poly_faces_id               (const uint pid, const bool sort_by_fid = false) const;
                std::vector<bool>  poly_faces_winding          (const uint pid) const;
                uint               poly_split_along_new_face   (const uint pid, const std::vector<uint> & f);

};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_polyhedralmesh.cpp"
#endif

#endif // CINO_ABSTRACT_POLYHEDRAL_MESH_H
