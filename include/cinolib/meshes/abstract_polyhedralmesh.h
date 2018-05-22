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
#ifndef CINO_ABSTRACT_POLYHEDRAL_MESH_H
#define CINO_ABSTRACT_POLYHEDRAL_MESH_H

#include <cinolib/meshes/abstract_mesh.h>
#include <cinolib/ipair.h>

namespace cinolib
{

template<class M, // mesh attributes
         class V, // vert attributes
         class E, // edge attributes
         class F, // face attributes
         class P> // polyhedron attributes
class AbstractPolyhedralMesh : public AbstractMesh<M,V,E,P>
{
    protected:

        std::vector<std::vector<uint>> faces;              // list of faces (assumed CCW)
        std::vector<std::vector<bool>> polys_face_winding; // true if the face is CCW, false if it is CW

        std::vector<bool> v_on_srf; // true if a vertex is exposed on the surface
        std::vector<bool> e_on_srf; // true if an edge is exposed on the surface
        std::vector<bool> f_on_srf; // true if a face is exposed on the surface

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
        virtual ~AbstractPolyhedralMesh() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void clear();
        void init (const std::vector<vec3d>             & verts,
                   const std::vector<std::vector<uint>> & faces,
                   const std::vector<std::vector<uint>> & polys,
                   const std::vector<std::vector<bool>> & polys_face_winding);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                void update_normals();
                void update_f_normals();
        virtual void update_f_normal(const uint fid) = 0;
                void update_f_tessellation();
                void update_f_tessellation(const uint fid);
                void update_v_normals();
                void update_v_normal(const uint vid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint verts_per_poly(const uint pid) const { return this->p2v.at(pid).size();   }
        virtual uint edges_per_poly(const uint pid) const { return this->p2e.at(pid).size();   }
        virtual uint faces_per_poly(const uint pid) const { return this->polys.at(pid).size(); }
        virtual uint verts_per_face(const uint fid) const { return this->faces.at(fid).size(); }
        virtual uint edges_per_face(const uint fid) const { return this->faces.at(fid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_srf_verts() const;
        uint num_srf_edges() const;
        uint num_srf_faces() const;
        uint num_srf_polys() const;
        uint num_faces()     const { return faces.size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<std::vector<uint>> & vector_faces() const { return faces; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                const std::vector<uint> & adj_v2f(const uint vid) const { return v2f.at(vid);         }
                const std::vector<uint> & adj_e2f(const uint eid) const { return e2f.at(eid);         }
                const std::vector<uint> & adj_f2v(const uint fid) const { return this->faces.at(fid); }
                const std::vector<uint> & adj_f2e(const uint fid) const { return f2e.at(fid);         }
                const std::vector<uint> & adj_f2f(const uint fid) const { return f2f.at(fid);         }
                const std::vector<uint> & adj_f2p(const uint fid) const { return f2p.at(fid);         }
                const std::vector<uint> & adj_p2f(const uint pid) const { return this->polys.at(pid); }
        virtual const std::vector<uint> & adj_p2v(const uint pid) const { return p2v.at(pid);         }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const F & face_data(const uint fid) const { return f_data.at(fid); }
              F & face_data(const uint fid)       { return f_data.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                void              vert_switch_id            (const uint vid0, const uint vid1);
                void              vert_remove               (const uint vid);
                void              vert_remove_unreferenced  (const uint vid);
                uint              vert_add                  (const vec3d & pos);
                bool              vert_is_on_srf            (const uint vid) const;
        virtual double            vert_mass                 (const uint vid) const;
                double            vert_volume               (const uint vid) const;
                std::vector<uint> vert_adj_srf_edges        (const uint vid) const;
                std::vector<uint> vert_ordered_srf_vert_ring(const uint vid) const;
                std::vector<uint> vert_ordered_srf_edge_ring(const uint vid) const;
                std::vector<uint> vert_ordered_srf_face_ring(const uint vid) const;
                void              vert_ordered_srf_one_ring (const uint vid,
                                                             std::vector<uint> & v_ring,
                                                             std::vector<uint> & e_ring,
                                                             std::vector<uint> & f_ring) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void              edge_switch_id          (const uint eid0, const uint eid1);
        uint              edge_add                (const uint vid0, const uint vid1);
        void              edge_remove             (const uint eid);
        void              edge_remove_unreferenced(const uint eid);
        bool              edge_is_on_srf          (const uint eid) const;
        std::vector<uint> edge_ordered_poly_ring  (const uint eid) const;
        std::vector<uint> edge_adj_srf_faces      (const uint eid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                vec3d             face_vert               (const uint fid, const uint off) const;
                uint              face_vert_id            (const uint fid, const uint off) const;
                uint              face_vert_offset        (const uint fid, const uint vid) const;
                uint              face_edge_id            (const uint fid, const uint vid0, const uint vid1) const;
                uint              face_edge_id            (const uint fid, const uint off) const;
                std::vector<uint> face_v2e                (const uint fid, const uint vid) const;
                bool              face_is_on_srf          (const uint fid) const;
                bool              face_contains_vert      (const uint fid, const uint vid) const;
                bool              face_contains_edge      (const uint fid, const uint eid) const;
                bool              face_winding_agrees_with(const uint fid, const uint vid0, const uint vid1) const;
                uint              face_adj_through_edge   (const uint fid, const uint eid, const uint pid) const;
                ipair             face_edges_from_vert    (const uint fid, const uint vid) const;
                uint              face_adj_srf_edge       (const uint fid, const uint eid, const uint vid) const;
                uint              face_opp_to_srf_edge    (const uint fid, const uint eid) const;
                uint              face_shared_edge        (const uint fid0, const uint fid1) const;
                vec3d             face_centroid           (const uint fid) const;
                double            face_mass               (const uint fid) const;
                double            face_area               (const uint fid) const;
                bool              faces_are_disjoint      (const uint fid0, const uint fid1) const;
                bool              faces_are_adjacent      (const uint fid0, const uint fid1) const;
                bool              faces_share_poly        (const uint fid0, const uint fid1) const;
                void              face_unmark_all         ();
                int               face_id                 (const std::vector<uint> & f) const;
                bool              face_is_tri             (const uint fid) const;
                bool              face_is_quad            (const uint fid) const;
        virtual void              face_set_color          (const Color & c);
        virtual void              face_set_alpha          (const float alpha);
                void              face_switch_id          (const uint fid0, const uint fid1);
                uint              face_add                (const std::vector<uint> & f);
                void              face_remove             (const uint fid);
                void              face_remove_unreferenced(const uint fid);
                std::vector<uint> face_tessellation       (const uint fid) const;
                std::vector<uint> face_verts_id           (const uint fid, const bool sort_by_vid = false) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual double            poly_volume             (const uint pid) const = 0;
        virtual double            poly_mass               (const uint pid) const;
                bool              poly_contains_face      (const uint pid, const uint fid) const;
                bool              poly_is_on_surf         (const uint pid) const;
                int               poly_shared_face        (const uint pid0, const uint pid1) const;
                uint              poly_face_id            (const uint pid, const uint off) const;
                bool              poly_face_is_CCW        (const uint pid, const uint fid) const;
                bool              poly_face_is_CW         (const uint pid, const uint fid) const;
                uint              poly_face_offset        (const uint pid, const uint fid) const;
                vec3d             poly_face_normal        (const uint pid, const uint fid) const;
                int               poly_adj_through_face   (const uint pid, const uint fid) const;
                std::vector<uint> poly_e2f                (const uint pid, const uint eid) const;
                std::vector<uint> poly_v2f                (const uint pid, const uint vid) const;
                void              poly_switch_id          (const uint pid0, const uint pid1);
                uint              poly_add                (const std::vector<uint> & flist, const std::vector<bool> & fwinding);
                void              poly_remove_unreferenced(const uint pid);
                void              poly_remove             (const uint pid);
                void              polys_remove            (const std::vector<uint> & pids);
};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_polyhedralmesh.cpp"
#endif

#endif // CINO_ABSTRACT_POLYHEDRAL_MESH_H
