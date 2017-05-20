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
#ifndef CINO_TRIMESH_H
#define CINO_TRIMESH_H

#include <cinolib/cinolib.h>
#include <cinolib/common.h>
#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/triangle.h>

#include <assert.h>
#include <float.h>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>


namespace cinolib
{

class Trimesh
{
    public:

        Trimesh(){}

        Trimesh(const char * filename);

        Trimesh(const std::vector<double> & coords,
                const std::vector<u_int>  & tris);

        static const int verts_per_element = 3;
        static const int edges_per_element = 3;

    protected:

        std::string filename;

        // bounding box
        //
        Bbox bb;

        // serialized xyz coordinates, triangles and edges
        //
        std::vector<double> coords;
        std::vector<u_int>  tris;
        std::vector<u_int>  edges;

        // per vertex/triangle normals
        //
        std::vector<double> v_norm;
        std::vector<double> t_norm;

        // general purpose float and int scalars
        //
        std::vector<float> u_text;  // per vertex: 1D texturing
        std::vector<int>   t_label; // per triangle

        // adjacencies
        //
        std::vector< std::vector<int> > vtx2vtx;
        std::vector< std::vector<int> > vtx2edg;
        std::vector< std::vector<int> > vtx2tri;
        std::vector< std::vector<int> > edg2tri;
        std::vector< std::vector<int> > tri2edg;
        std::vector< std::vector<int> > tri2tri;

        void load(const char * filename);
        void init();

    public:

        void                        clear();
        void                        update_adjacency();
        void                        update_normals();
        void                        update_t_normals();
        void                        update_v_normals();
        void                        update_t_normal(const int tid);
        void                        update_v_normal(const int vid);
        void                        update_bbox();
        void                        normalize_area();
        void                        center_bbox();
        void                        check_topology() const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        virtual void                operator+=(const Trimesh & m);
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        std::string                 loaded_file() const;
        void                        save(const char * filename) const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        int                         num_vertices()  const;
        int                         num_triangles() const;
        int                         num_elements()  const;
        int                         num_edges()     const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        const std::vector<int> &    adj_vtx2vtx(const int vid) const;
        const std::vector<int> &    adj_vtx2edg(const int vid) const;
        const std::vector<int> &    adj_vtx2tri(const int vid) const;
        const std::vector<int> &    adj_vtx2ele(const int vid) const; // equal to adj_vtx2tri - just for template compatibility
        const std::vector<int> &    adj_edg2tri(const int eid) const;
        const std::vector<int> &    adj_tri2edg(const int tid) const;
        const std::vector<int> &    adj_ele2edg(const int tid) const; // equal to adj_tri2edg - just for template compatibility
        const std::vector<int> &    adj_tri2tri(const int tid) const;
        std::vector<int>            adj_vtx2vtx_ordered(const int vid) const;
        std::vector<uint>           adj_vtx2tri_ordered(const int vid) const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        const Bbox                & bbox()                  const;
        const std::vector<double> & vector_coords()         const;
        const std::vector<u_int>  & vector_triangles()      const;
        const std::vector<u_int>  & vector_edges()          const;
        const std::vector<float>  & vector_v_float_scalar() const;
        const std::vector<int>    & vector_t_int_scalar()   const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        vec3d                       barycenter() const;
        void                        translate(const vec3d & delta);
        void                        rotate   (const vec3d & axis, const double angle);
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        int                         connected_components(std::vector< std::set<int> > & ccs) const;
        int                         connected_components() const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        std::vector<ipair>          get_boundary_edges()    const;
        int                         edge_vertex_id(const int eid, const int offset) const;
        int                         edge_opposite_to(const int tid, const int vid) const;
        bool                        edge_contains_vertex(const int eid, const int vid) const;
        bool                        edge_is_manifold(const int eid) const;
        bool                        edge_is_boundary(const int eid) const;
        bool                        edge_is_border(const int eid) const;
        bool                        edges_share_same_triangle(const int eid1, const int eid2) const;
        double                      edge_length(const int eid) const;
        double                      avg_edge_length() const;
        double                      max_edge_length() const;
        double                      min_edge_length() const;
        vec3d                       edge_vertex(const int eid, const int offset) const;
        ipair                       shared_edge(const int tid0, const int tid1) const;
        virtual bool                edge_collapse(const int eid);
        virtual void                edge_switch_id(const int eid0, const int eid1);
        virtual void                remove_unreferenced_edge(const int eid);
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        bool                        barycentric_coordinates(const int tid, const vec3d & P, std::vector<double> & wgts) const;
        bool                        elem_bary_is_vertex(const int tid, const std::vector<double> & wgts, int & vid, const double tol = 1e-5) const;
        bool                        elem_bary_is_edge(const int tid, const std::vector<double> & wgts, int & eid, const double tol = 1e-5) const;
        vec3d                       triangle_vertex(const int tid, const int offset) const;
        vec3d                       elem_vertex(const int eid, const int offset) const;
        bool                        triangle_is_boundary(const int tid) const;
        int                         triangle_vertex_id(const int tid, const int offset) const;
        int                         elem_vertex_id(const int tid, const int offset) const;
        int                         triangle_vertex_offset(const int tid, const int vid) const;
        int                         elem_vertex_offset(const int tid, const int vid) const;
        vec3d                       triangle_normal(const int tid) const;
        float                       triangle_min_u_text(const int tid) const;
        int                         triangle_edge_local_to_global(const int tid, const int off) const;
        int                         triangle_vertex_global_to_local(const int tid, const int vid) const;
        int                         triangle_label(const int tid) const;
        int                         elem_label(const int tid) const;
        void                        triangle_set_label(const int tid, const int i);
        int                         shared_triangle(const int eid0, const int eid1) const;
        bool                        triangle_contains_vertex(const int tid, const int vid) const;
        int                         triangle_adjacent_along(const int tid, const int vid0, const int vid1) const;
        double                      element_mass(const int tid) const;
        double                      triangle_area(const int tid) const;
        vec3d                       element_barycenter(const int tid) const;
        void                        set_triangle(const int tid, const int vid0, const int vid1, const int vid2);
        double                      triangle_angle_at_vertex(const int tid, const int vid, const bool rad = true) const;
        bool                        triangle_is_cap(const int tid, const double angle_thresh_deg = 177.0) const;
        bool                        triangle_is_needle(const int tid, const double angle_thresh_deg = 3.0) const;
        virtual int                 add_triangle(const int vid0, const int vid1, const int vid2, const int scalar = 0);
        virtual void                triangle_switch_id(const int tid0, const int tid1);
        virtual void                remove_unreferenced_triangle(const int eid);
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        std::set<int>               vertex_n_ring(const int vid, const int n) const;
        double                      vertex_mass (const int vid) const;
        int                         vertex_opposite_to(const int tid, const int vid0, const int vid1) const;
        int                         vertex_opposite_to(const int eid, const int vid) const;
        bool                        vertex_is_border(const int vid) const;
        bool                        vertex_is_boundary(const int vid) const;
        bool                        vertices_are_adjacent(const int vid0, const int vid1) const;
        vec3d                       vertex_normal(const int vid) const;
        vec3d                       vertex(const int vid) const;
        void                        set_vertex(const int vid, const vec3d & pos);
        int                         vertex_valence(const int vid) const;
        float                       vertex_u_text(const int vid) const;
        void                        set_vertex_u_text(const int vid, const float s);
        int                         vertex_with_min_u_text() const;
        int                         shared_vertex(const int eid0, const int eid1) const;
        bool                        vertex_is_local_minima(const int vid) const;
        bool                        vertex_is_local_maxima(const int vid) const;
        bool                        vertex_is_critical_point(const int vid) const;
        std::vector<uint>           vertex_incident_boundary_edges(const int vid) const;
        std::vector<int>            get_boundary_vertices() const;
        virtual int                 add_vertex(const vec3d & v, const float scalar = 0.0);
        virtual void                vertex_switch_id(const int vid0, const int vid1);
        virtual void                remove_unreferenced_vertex(const int vid);
};

}

#ifndef  CINO_STATIC_LIB
#include "trimesh.cpp"
#endif

#endif // CINO_TRIMESH_H
