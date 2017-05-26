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
#ifndef CINO_TRI_H
#define CINO_TRI_H

#include <vector>
#include <cinolib/cinolib.h>
#include <cinolib/common.h>
#include <cinolib/symbols.h>
#include <cinolib/color.h>
#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/mesh_std_data.h>
#include <cinolib/geometry/triangle.h>

namespace cinolib
{

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data>
class Tri
{
    public:

        Tri(){}

        Tri(const char * filename);

        Tri(const std::vector<vec3d> & verts,
            const std::vector<uint>  & faces);

        Tri(const std::vector<double> & coords,
            const std::vector<uint>   & faces);

    protected:

        Bbox bb;

        std::vector<vec3d> verts;
        std::vector<uint>  edges;
        std::vector<uint>  faces;

        // attributes
        //
        M              m_data;
        std::vector<V> v_data;
        std::vector<E> e_data;
        std::vector<F> f_data;

        // adjacencies -- Yes, I have lots of memory ;)
        //
        std::vector<std::vector<uint>> v2v; // vert to vert adjacency
        std::vector<std::vector<uint>> v2e; // vert to edge adjacency
        std::vector<std::vector<uint>> v2f; // vert to face adjacency
        std::vector<std::vector<uint>> e2f; // edge to face adjacency
        std::vector<std::vector<uint>> f2e; // face to edge adjacency
        std::vector<std::vector<uint>> f2f; // face to face adjacency

    public:

        void clear();
        void init();
        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

virtual void operator+=(const Tri & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_adjacency();
        void update_bbox();
        void update_f_normal(const uint fid);
        void update_v_normal(const uint vid);
        void update_f_normals();
        void update_v_normals();
        void update_normals();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_face() const { return 3; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_verts() const { return verts.size();                    }
        uint num_edges() const { return edges.size() / 2;                }
        uint num_faces() const { return faces.size() / verts_per_face(); }
        uint num_elems() const { return faces.size() / verts_per_face(); } // elem == face!!

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const Bbox                & bbox()          const { return bb;    }
        const std::vector<uint>   & vector_edges()  const { return edges; }
        const std::vector<vec3d>  & vector_verts()  const { return verts; }
              std::vector<double>   vector_coords() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<float> export_uvw_param(const int mode) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<uint> & adj_v2v(const uint vid) const { return v2v.at(vid); }
        const std::vector<uint> & adj_v2e(const uint vid) const { return v2e.at(vid); }
        const std::vector<uint> & adj_v2f(const uint vid) const { return v2f.at(vid); }
        const std::vector<uint> & adj_e2f(const uint eid) const { return e2f.at(eid); }
        const std::vector<uint> & adj_f2e(const uint fid) const { return f2e.at(fid); }
        const std::vector<uint> & adj_f2f(const uint fid) const { return f2f.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const M & mesh_data()               const { return m_data;         }
              M & mesh_data()                     { return m_data;         }
        const V & vert_data(const uint vid) const { return v_data.at(vid); }
              V & vert_data(const uint vid)       { return v_data.at(vid); }
        const E & edge_data(const uint eid) const { return e_data.at(eid); }
              E & edge_data(const uint eid)       { return e_data.at(eid); }
        const F & face_data(const uint fid) const { return f_data.at(fid); }
              F & face_data(const uint fid)       { return f_data.at(fid); }
        const F & elem_data(const uint fid) const { return f_data.at(fid); } // elem == face!!
              F & elem_data(const uint fid)       { return f_data.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void               center_bbox();
        void               normalize_area();
        void               translate(const vec3d & delta);
        void               rotate(const vec3d & axis, const double angle);
        vec3d              centroid() const;
        uint               connected_components() const;
        uint               connected_components(std::vector<std::set<uint>> & ccs) const;
        std::vector<uint>  get_boundary_vertices() const;
        std::vector<ipair> get_boundary_edges() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  const vec3d           & vert                    (const uint vid) const { return verts.at(vid); }
        vec3d           & vert                    (const uint vid)       { return verts.at(vid); }
        void              vert_ordered_one_ring   (const uint vid,
                                                   std::vector<uint> & v_ring,        // sorted list of adjacent vertices
                                                   std::vector<uint> & f_ring,        // sorted list of adjacent triangles
                                                   std::vector<uint> & e_ring,        // sorted list of edges incident to vid
                                                   std::vector<uint> & e_link) const; // sorted list of edges opposite to vid
        std::vector<uint> vert_ordered_vert_ring  (const uint vid) const;
        std::vector<uint> vert_ordered_face_ring  (const uint vid) const;
        std::vector<uint> vert_ordered_edge_ring  (const uint vid) const;
        std::vector<uint> vert_ordered_edge_link  (const uint vid) const;
        std::set<uint>    vert_n_ring             (const uint vid, const uint n) const;
        double            vert_area               (const uint vid) const;
        double            vert_mass               (const uint vid) const;
        bool              vert_is_boundary        (const uint vid) const;
        bool              verts_are_adjacent      (const uint vid0, const uint vid1) const;
        bool              vert_is_local_min       (const uint vid, const int tex_coord = U_param) const;
        bool              vert_is_local_max       (const uint vid, const int tex_coord = U_param) const;
        bool              vert_is_critical_p      (const uint vid) const;
        uint              vert_opposite_to        (const uint fid, const uint vid0, const uint vid1) const;
        uint              vert_opposite_to        (const uint eid, const uint vid) const;
        uint              vert_valence            (const uint vid) const;
        uint              vert_shared             (const uint eid0, const uint eid1) const;
        std::vector<uint> vert_boundary_edges     (const uint vid) const;
        void              vert_switch_id          (const uint vid0, const uint vid1);
virtual void              vert_remove_unreferenced(const uint vid);
virtual uint              vert_add                (const vec3d & pos, const V & data);
virtual void              vert_set_color          (const Color & c);
virtual void              vert_set_alpha          (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  edge_vert               (const uint eid, const uint offset) const;
        uint   edge_vert_id            (const uint eid, const uint offset) const;
        int    edge_opposite_to        (const uint fid, const uint vid) const;
        bool   edge_contains_vert      (const uint eid, const uint vid) const;
        bool   edge_is_manifold        (const uint eid) const;
        bool   edge_is_boundary        (const uint eid) const;
        bool   edges_share_face        (const uint eid1, const uint eid2) const;
        ipair  edge_shared             (const uint fid0, const uint fid1) const;
        double edge_length             (const uint eid) const;
        double edge_avg_length         () const;
        double edge_max_length         () const;
        double edge_min_length         () const;
        void   edge_switch_id          (const uint eid0, const uint eid1);
virtual bool   edge_collapse           (const uint eid);
virtual void   edge_remove_unreferenced(const uint eid);
virtual void   edge_set_color          (const Color & c);
virtual void   edge_set_alpha          (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  face_vert               (const uint fid, const uint offset) const;
        uint   face_vert_id            (const uint fid, const uint offset) const;
        uint   face_edge_id            (const uint fid, const uint offset) const;
        uint   face_vert_offset        (const uint fid, const uint vid) const;
        uint   face_shared             (const uint eid0, const uint eid1) const;
        vec3d  face_centroid           (const uint fid) const;
        double face_mass               (const uint fid) const;
        double face_area               (const uint fid) const;
        double face_angle_at_vert      (const uint fid, const uint vid, const int unit = RAD) const;
        bool   face_contains_vert      (const uint fid, const uint vid) const;
        bool   face_is_cap             (const uint fid, const double angle_thresh_deg = 177.0) const;
        bool   face_is_needle          (const uint fid, const double angle_thresh_deg = 3.0) const;
        int    face_adjacent_along     (const uint fid, const uint vid0, const uint vid1) const;
        bool   face_is_boundary        (const uint fid) const;
        bool   face_bary_coords        (const uint fid, const vec3d & P, std::vector<double> & wgts) const;
        bool   face_bary_is_vert       (const uint fid, const std::vector<double> & wgts, uint & vid, const double tol = 1e-5) const;
        bool   face_bary_is_edge       (const uint fid, const std::vector<double> & wgts, uint & eid, const double tol = 1e-5) const;
        void   face_switch_id          (const uint fid0, const uint fid1);
virtual uint   face_add                (const uint vid0, const uint vid1, const uint vid2, const F & data);
virtual void   face_remove_unreferenced(const uint fid);
virtual void   face_set_color          (const Color & c);
virtual void   face_set_alpha          (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // These are all wraps for the "face_ methods". They are useful for generic
        // programming, because "elem_" will wrap face_ for surface meshes and wrap
        // "cell_" for volumetric meshes, allowing the use of templated algorithms
        // that work with both types of meshes without requiring specialzed code

        vec3d  elem_centroid    (const uint fid) const;
        void   elem_show_all    ();
        double elem_mass        (const uint fid) const;
        bool   elem_bary_is_vert(const uint fid, const std::vector<double> & wgts, uint & vid, const double tol = 1e-5) const;
        bool   elem_bary_is_edge(const uint fid, const std::vector<double> & wgts, uint & eid, const double tol = 1e-5) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "tri.cpp"
#endif

#endif // CINO_TRI_H
