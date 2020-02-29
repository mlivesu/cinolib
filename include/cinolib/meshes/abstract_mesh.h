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
#ifndef CINO_ABSTRACT_MESH_H
#define CINO_ABSTRACT_MESH_H

#include <set>
#include <vector>
#include <sys/types.h>

#include <cinolib/geometry/aabb.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/color.h>
#include <cinolib/symbols.h>
#include <cinolib/ipair.h>

typedef enum
{
    TRIMESH       ,
    QUADMESH      ,
    POLYGONMESH   ,
    TETMESH       ,
    HEXMESH       ,
    POLYHEDRALMESH,
}
MeshType;


namespace cinolib
{

template<class M, // mesh attributes
         class V, // vert attributes
         class E, // edge attributes
         class P> // polygon/polyhedra attributes
class AbstractMesh
{
    protected:

        AABB bb;

        std::vector<vec3d>             verts;
        std::vector<uint>              edges;
        std::vector<std::vector<uint>> polys; // either polygons or polyhedra

        M              m_data;
        std::vector<V> v_data;
        std::vector<E> e_data;
        std::vector<P> p_data;

        std::vector<std::vector<uint>> v2v; // vert to vert adjacency
        std::vector<std::vector<uint>> v2e; // vert to edge adjacency
        std::vector<std::vector<uint>> v2p; // vert to poly adjacency
        std::vector<std::vector<uint>> e2p; // edge to poly adjacency        
        std::vector<std::vector<uint>> p2e; // poly to edge adjacency
        std::vector<std::vector<uint>> p2p; // poly to poly adjacency

    public:

        typedef M M_type;
        typedef V V_type;
        typedef E E_type;
        typedef P P_type;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit AbstractMesh() {}
        virtual ~AbstractMesh() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual MeshType mesh_type() const = 0;
                bool     mesh_is_surface() const;
                bool     mesh_is_volumetric() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void clear();
        virtual void load(const char * filename) = 0;
        virtual void save(const char * filename) const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                void update_bbox();
        virtual void update_normals() = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void  translate(const vec3d & delta);
        void  center_bbox();
        void  rotate(const vec3d & axis, const double angle);
        void  scale(const double scale_factor);
        void  normalize_bbox();
        vec3d centroid() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual int genus() const = 0;
        virtual int Euler_characteristic() const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint verts_per_poly(const uint pid) const = 0;
        virtual uint edges_per_poly(const uint pid) const { return this->p2e.at(pid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_verts() const { return verts.size();     }
        uint num_edges() const { return edges.size() / 2; }
        uint num_polys() const { return polys.size();     }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const AABB                           & bbox()          const { return bb;    }
        const std::vector<vec3d>             & vector_verts()  const { return verts; }
              std::vector<vec3d>             & vector_verts()        { return verts; }
        const std::vector<uint>              & vector_edges()  const { return edges; }
              std::vector<uint>              & vector_edges()        { return edges; }
        const std::vector<std::vector<uint>> & vector_polys()  const { return polys; }
              std::vector<std::vector<uint>> & vector_polys()        { return polys; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<vec3d> vector_vert_normals()       const;
        std::vector<Color> vector_vert_colors()        const;
        std::vector<int>   vector_vert_labels()        const;
        std::vector<Color> vector_edge_colors()        const;
        std::vector<int>   vector_edge_labels()        const;
        std::vector<vec3d> vector_poly_normals()       const;
        std::vector<Color> vector_poly_colors()        const;
        std::vector<int>   vector_poly_labels()        const;
        std::vector<Color> vector_poly_unique_colors() const;
        std::vector<int>   vector_poly_unique_labels() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<double> serialize_uvw  (const int mode) const;
        void                deserialize_uvw(const std::vector<vec3d> & uvw);
        void                copy_xyz_to_uvw(const int mode);
        void                copy_uvw_to_xyz(const int mode);
        void                swap_xyz_uvw   (const bool normals = true, const bool bbox = true);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                const std::vector<uint> & adj_v2v(const uint vid) const { return v2v.at(vid); }
                      std::vector<uint> & adj_v2v(const uint vid)       { return v2v.at(vid); }
                const std::vector<uint> & adj_v2e(const uint vid) const { return v2e.at(vid); }
                      std::vector<uint> & adj_v2e(const uint vid)       { return v2e.at(vid); }
                const std::vector<uint> & adj_v2p(const uint vid) const { return v2p.at(vid); }
                      std::vector<uint> & adj_v2p(const uint vid)       { return v2p.at(vid); }
                      std::vector<uint>   adj_e2v(const uint eid) const;
                      std::vector<uint>   adj_e2e(const uint eid) const;
                const std::vector<uint> & adj_e2p(const uint eid) const { return e2p.at(eid); }
                      std::vector<uint> & adj_e2p(const uint eid)       { return e2p.at(eid); }
                const std::vector<uint> & adj_p2e(const uint pid) const { return p2e.at(pid); }
                      std::vector<uint> & adj_p2e(const uint pid)       { return p2e.at(pid); }
                const std::vector<uint> & adj_p2p(const uint pid) const { return p2p.at(pid); }
                      std::vector<uint> & adj_p2p(const uint pid)       { return p2p.at(pid); }
        virtual const std::vector<uint> & adj_p2v(const uint pid) const = 0;
        virtual       std::vector<uint> & adj_p2v(const uint pid)       = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const M & mesh_data()               const { return m_data;         }
              M & mesh_data()                     { return m_data;         }
        const V & vert_data(const uint vid) const { return v_data.at(vid); }
              V & vert_data(const uint vid)       { return v_data.at(vid); }
        const E & edge_data(const uint eid) const { return e_data.at(eid); }
              E & edge_data(const uint eid)       { return e_data.at(eid); }
        const P & poly_data(const uint pid) const { return p_data.at(pid); }
              P & poly_data(const uint pid)       { return p_data.at(pid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // useful for GUIs with mouse picking
        uint pick_vert(const vec3d & p) const;
        uint pick_edge(const vec3d & p) const;
        uint pick_poly(const vec3d & p) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

          const vec3d          & vert                       (const uint vid) const { return verts.at(vid); }
                vec3d          & vert                       (const uint vid)       { return verts.at(vid); }
                void             vert_weights_uniform       (const uint vid, std::vector<std::pair<uint,double>> & wgts) const;
                std::set<uint>   vert_n_ring                (const uint vid, const uint n) const;
                bool             verts_are_adjacent         (const uint vid0, const uint vid1) const;
                bool             vert_is_local_min          (const uint vid, const int tex_coord = U_param) const;
                bool             vert_is_local_max          (const uint vid, const int tex_coord = U_param) const;
                uint             vert_valence               (const uint vid) const;
                uint             vert_shared                (const uint eid0, const uint eid1) const;
                double           vert_min_uvw_value         (const int tex_coord = U_param) const;
                double           vert_max_uvw_value         (const int tex_coord = U_param) const;
                void             vert_unmark_all            ();
                void             vert_local_unmark_near_vert(const uint vid);
                void             vert_local_unmark_near_edge(const uint eid);
                void             vert_local_unmark_near_poly(const uint pid);
                void             vert_apply_labels          (const std::vector<int> & labels);
                void             vert_apply_label           (const int label);
        virtual double           vert_mass                  (const uint vid) const = 0;
        virtual void             vert_set_color             (const Color & c);
        virtual void             vert_set_alpha             (const float alpha);
        virtual uint             vert_opposite_to           (const uint eid, const uint vid) const;
        virtual void             vert_weights               (const uint vid, const int type, std::vector<std::pair<uint,double>> & wgts) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                vec3d                  edge_vert                  (const uint eid, const uint offset) const;
                uint                   edge_vert_id               (const uint eid, const uint offset) const;
                std::vector<uint>      edge_vert_ids              (const uint eid) const;
                std::vector<vec3d>     edge_verts                 (const uint eid) const;
                int                    edge_id                    (const uint vid0, const uint vid1) const;
                int                    edge_id                    (const ipair & vids) const;
                int                    edge_id                    (const std::vector<uint> & vids) const;
                vec3d                  edge_sample_at             (const uint eid, double lambda) const; // arc-length param
                uint                   edge_valence               (const uint eid) const;
                bool                   edge_contains_vert         (const uint eid, const uint vid) const;
                bool                   edges_are_adjacent         (const uint eid0, const uint eid1) const;
                double                 edge_length                (const uint eid) const;
                vec3d                  edge_vec                   (const uint eid, const bool normalized = false) const;
                double                 edge_avg_length            () const;
                double                 edge_max_length            () const;
                double                 edge_min_length            () const;
                void                   edge_unmark_all            ();
                void                   edge_local_unmark_near_vert(const uint vid);
                void                   edge_local_unmark_near_edge(const uint eid);
                void                   edge_local_unmark_near_poly(const uint pid);
                void                   edge_apply_labels          (const std::vector<int> & labels);
                void                   edge_apply_label           (const int label);
                void                   edge_mark_sharp_creases    (const float thresh_rad = 1.0472); // 60 degrees
        virtual double                 edge_dihedral_angle        (const uint eid) const = 0;
        virtual void                   edge_set_color             (const Color & c);
        virtual void                   edge_set_alpha             (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

                vec3d              poly_vert                  (const uint pid, const uint offset) const;
                std::vector<vec3d> poly_verts                 (const uint pid) const;
                std::vector<uint>  poly_verts_id              (const uint pid, const bool sort_by_vid = false) const;
                std::vector<uint>  poly_v2v                   (const uint pid, const uint vid) const;
                std::vector<uint>  poly_v2e                   (const uint pid, const uint vid) const;
                uint               poly_vert_valence          (const uint pid, const uint vid) const;
                uint               poly_vert_id               (const uint pid, const uint offset) const;
                uint               poly_vert_offset           (const uint pid, const uint vid) const;
                vec3d              poly_centroid              (const uint pid) const;
                vec3d              poly_sample_at             (const uint pid, const std::vector<double> & bc) const;
                double             poly_sample_param_at       (const uint pid, const std::vector<double> & bc, const int tex_coord = U_param) const;
                uint               poly_edge_id               (const uint pid, const uint vid0, const uint vid1) const;
                bool               poly_contains_vert         (const uint pid, const uint vid) const;
                bool               poly_contains_edge         (const uint pid, const uint eid) const;
                bool               poly_contains_edge         (const uint pid, const uint vid0, const uint vid1) const;
                void               poly_show_all              ();
                void               poly_color_wrt_label       (const bool sorted=false, const float s=.5f, float v=.85f); // s => saturation, v => value in HSV color space
                void               poly_label_wrt_color       ();
                void               poly_unmark_all            ();
                void               poly_local_unmark_near_vert(const uint vid);
                void               poly_local_unmark_near_edge(const uint eid);
                void               poly_local_unmark_near_poly(const uint pid);
                uint               polys_n_unique_colors      () const;
                uint               polys_n_unique_labels      () const;
                bool               polys_are_colored          () const;
                bool               polys_are_labeled          () const;
                void               poly_apply_labels          (const std::vector<int> & labels);
                void               poly_apply_label           (const int label);
                AABB               poly_aabb                  (const uint pid) const;
        virtual double             poly_mass                  (const uint pid) const = 0;
        virtual void               poly_set_color             (const Color & c);
        virtual void               poly_set_alpha             (const float alpha);
        virtual void               poly_export_element        (const uint pid, std::vector<vec3d> & verts, std::vector<std::vector<uint>> & faces) const = 0;
};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_mesh.cpp"
#endif

#endif //CINO_ABSTRACT_MESH_H
