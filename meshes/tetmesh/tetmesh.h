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
#ifndef TETMESH_H
#define TETMESH_H

#include <sys/types.h>
#include <vector>

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/tetrahedron.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/quality.h>

namespace cinolib
{

class Tetmesh
{
    public:

        Tetmesh(){}
        Tetmesh(const char * filename);
        Tetmesh(const std::vector<double> & coords,
                const std::vector<u_int>  & tets);

        std::string filename;

        const Bbox & bbox() const;

    protected:

        // bounding box
        //
        Bbox bb;

        // serialized xyz coordinates, tets and edges
        //
        std::vector<double> coords;
        std::vector<u_int>  tets;
        std::vector<u_int>  edges;
        std::vector<u_int>  tris;    // exterior surface
        std::vector<bool>   v_on_srf;  // true if a vertex is on the surface, false otherwise
        std::vector<bool>   e_on_srf;  // true if a vertex is on the surface, false otherwise

        // per vertex/triangle surface normals
        //
        std::vector<double> t_norm;

        // general purpose float and int scalars
        //
        std::vector<float> u_text;    // 1d texture per vertex
        std::vector<int>   t_label;   // per tet

        // adjacencies
        //
        std::vector< std::vector<int> > vtx2vtx;
        std::vector< std::vector<int> > vtx2edg;
        std::vector< std::vector<int> > vtx2tet;
        std::vector< std::vector<int> > vtx2tri;
        std::vector< std::vector<int> > edg2tet;
        std::vector< std::vector<int> > edg2tri;
        std::vector< std::vector<int> > tet2edg;
        std::vector< std::vector<int> > tet2tet;
        std::vector< std::vector<int> > tet2tri;
        std::vector< std::vector<int> > tri2tri;
        std::vector< std::vector<int> > tri2edg;
        std::vector< int >              tri2tet;


    public:

        const std::vector<double> & vector_coords()         const;
        const std::vector<uint>   & vector_tris()           const;
        const std::vector<float>  & vector_v_float_scalar() const;
        const std::vector<int>    & vector_t_int_scalar()   const;


        std::string loaded_file() const;

        Trimesh export_surface() const;
        Trimesh export_surface(std::map<int,int> & tet2tri_map, std::map<int,int> & tri2tet_map) const;

        virtual void operator+=(const Tetmesh & m);

        void init();
        void clear();
        void load(const char * filename);
        void save(const char * filename) const;

        void normalize_volume();

        bool empty() const;

        void update_bbox();
        void center_bbox();
        void update_interior_adjacency();
        void update_surface_adjacency();
        void update_t_normals();

        int num_vertices()      const;
        int num_tetrahedra()    const;
        int num_elements()      const;
        int num_edges()         const;
        int num_srf_triangles() const;

        const std::vector<int> & adj_vtx2vtx(const int vid) const;
        const std::vector<int> & adj_vtx2edg(const int vid) const;
        const std::vector<int> & adj_vtx2tri(const int vid) const;
        const std::vector<int> & adj_vtx2tet(const int vid) const;
        const std::vector<int> & adj_edg2tet(const int eid) const;
        const std::vector<int> & adj_edg2tri(const int eid) const;
        const std::vector<int> & adj_tet2edg(const int tid) const;
        const std::vector<int> & adj_tet2tet(const int tid) const;
        const std::vector<int> & adj_tet2tri(const int tid) const;
        const std::vector<int> & adj_tri2tri(const int tid) const;
        const std::vector<int> & adj_tri2edg(const int tid) const;
        const int              & adj_tri2tet(const int tid) const;

        vec3d vertex(const int vid) const;

        void set_vertex_u_text(const int vid, const float val);

        void set_tet_label(const int tid, const int label);

        int tet_label(const int tid) const;
        int elem_label(const int tid) const;

        float vertex_u_text(const int vid) const;

        float min_u_text() const;

        float max_u_text() const;

        int max_t_label() const;

        void set_vertex(const int vid, const vec3d & pos);

        bool is_surface_vertex(const int vid) const;

        bool is_surface_edge(const int eid) const;

        vec3d triangle_normal(const int tid) const;

        vec3d tet_centroid(const int tid) const;

        int tet_vertex_id(const int tid, const int offset) const;

        vec3d tet_vertex(const int tid, const int offset) const;

        int tri_vertex_id(const int tid, const int offset) const;

        vec3d tri_vertex(const int tid, const int offset) const;

        bool tet_contains_vertex(const int tid, const std::set<int> & vids) const;

        bool tet_contains_vertex(const int tid, const int vid) const;

        bool tet_contains_edge(const int tid, const int eid) const;

        vec3d element_barycenter(const int tid) const;

        double avg_edge_length() const;
        double edge_length(const int eid) const;

        vec3d edge_vertex(const int eid, const int offset) const;

        double tet_quality(const int tid) const;

        bool tet_is_adjacent_to(const int tid, const int nbr) const;

        double vertex_quality(const int vid) const;

        int vertex_inverted_elements(const int vid) const;

        double tet_volume(const int tid) const;

        int edge_vertex_id(const int eid, const int offset) const;

        void print_quality_statistics(bool list_folded_elements = false) const;

        int adjacent_tet_through_facet(const int tid, const int facet) const;

        int shared_facet(const int tid0, const int tid1) const;

        double vertex_mass(const int vid) const;

        int vertex_valence(const int vid) const;

        double tet_dihedral_angle(const int tid, const int fid0, const int fid1) const;

        vec3d tet_face_normal(const int tid, const int fid) const;

        int tet_face_opposite_to(const int tid, const int vid) const;

        int tet_vertex_opposite_to(const int tid, const int facet) const;

        int tet_edge_opposite_to(const int tid, const int vid0, const int vid1) const;

        double tet_edge_length(const int tid, const int eid) const;

        std::vector<int> tet_one_ring(const int tid) const;

        std::vector<int> get_flipped_tets() const;

        void export_submesh_with_label(const int             label,
                                       std::vector<double> & sub_coords,
                                       std::vector<u_int>  & sub_tets,
                                       std::map<int,int>   & vid2sub_vid,
                                       std::map<int,int>   & sub_vid2sub) const;


        std::vector<int> edge_ordered_tet_ring(const int eid) const;

        double barycentric_coordinates(const int tid, const vec3d & P, double wgt[4]) const;

        void translate(const vec3d & delta);

        double tet_face_area(const int tid, const int fid) const;

        void scale(const double x_scale, const double y_scale, const double z_scale);

        void normalize_u_text_field();

        bool vertex_is_local_minima(const int vid) const;
        bool vertex_is_local_maxima(const int vid) const;
        bool vertex_is_critical_point(const int vid) const;
        float tet_min_u_text(const int tid) const;
        int tet_edge_id(const int tid, const int vid0, const int vid1) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "tetmesh.cpp"
#endif

#endif // TETMESH_H
