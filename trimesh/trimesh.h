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
#ifndef TRIMESH_H
#define TRIMESH_H

#include "../cinolib.h"
#include "../bbox.h"
#include "../vec3.h"

#include <assert.h>
#include <float.h>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>


namespace cinolib
{

static const int TRI_EDGES[3][2] =
{
    { 0, 1 }, // 0
    { 1, 2 }, // 1
    { 2, 0 }, // 2
};

class Trimesh
{
    public:

        Trimesh(){}

        Trimesh(const char * filename);

        Trimesh(const std::vector<double> & coords,
                const std::vector<u_int>  & tris);

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
        void                        update_bbox();
        void                        normalize_area();
        void                        center_bbox();
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
        const std::vector<int> &    adj_edg2tri(const int eid) const;
        const std::vector<int> &    adj_tri2edg(const int tid) const;
        const std::vector<int> &    adj_tri2tri(const int tid) const;
        std::vector<int>            adj_vtx2vtx_ordered(const int vid) const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        const Bbox                & bbox()                  const;
        const std::vector<double> & vector_coords()         const;
        const std::vector<u_int>  & vector_triangles()      const;
        const std::vector<u_int>  & vector_edges()          const;
        const std::vector<float>  & vector_v_float_scalar() const;
        const std::vector<int>    & vector_t_int_scalar()   const;
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
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
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        void                        remove_duplicated_triangles();
        vec3d                       triangle_vertex(const int tid, const int offset) const;
        bool                        triangle_is_boundary(const int tid) const;
        int                         triangle_vertex_id(const int tid, const int offset) const;
        vec3d                       triangle_normal(const int tid) const;
        float                       triangle_min_u_text(const int tid) const;
        int                         triangle_label(const int tid) const;
        void                        triangle_set_label(const int tid, const int i);
        int                         shared_triangle(const int eid0, const int eid1) const;
        bool                        triangle_contains_vertex(const int tid, const int vid) const;
        int                         triangle_adjacent_along(const int tid, const int vid0, const int vid1) const;
        double                      element_mass(const int tid) const;
        vec3d                       element_barycenter(const int tid) const;
        void                        set_triangle(const int tid, const int vid0, const int vid1, const int vid2);
        virtual int                 add_triangle(const int vid0, const int vid1, const int vid2, const int scalar);
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        std::set<int>               vertex_n_ring(const int vid, const int n) const;
        double                      vertex_mass (const int vid) const;
        int                         vertex_opposite_to(const int tid, const int vid0, const int vid1) const;
        int                         vertex_opposite_to(const int eid, const int vid) const;
        double                      angle_at_vertex(const int tid, const int vid) const;
        bool                        vertex_is_border(const int vid) const;
        bool                        vertex_is_boundary(const int vid) const;
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
        void                        remove_duplicated_vertices(const double eps = 1e-7);
        std::vector<int>            get_boundary_vertices() const;
        virtual int                 add_vertex(const vec3d & v, const float scalar = 0.0);
};

}

#ifndef  CINO_STATIC_LIB
#include "trimesh.cpp"
#endif

#endif // TRIMESH_H
