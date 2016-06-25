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
#ifndef HEXMESH_H
#define HEXMESH_H

#include <sys/types.h>
#include <vector>

#include "../cinolib.h"
#include "../bbox.h"
#include "../vec3.h"
#include "../quadmesh/quadmesh.h"
//#include "quality.h"

namespace cinolib
{


static int HEXA_FACES[6][4] =
{
    { 0 , 3 , 2 , 1 } ,
    { 1 , 2 , 6 , 5 } ,
    { 4 , 5 , 6 , 7 } ,
    { 3 , 0 , 4 , 7 } ,
    { 0 , 1 , 5 , 4 } ,
    { 2 , 3 , 7 , 6 }
};

static int HEXA_EDGES[12][2] =
{
    { 0, 1 }, // 0
    { 1, 2 }, // 1
    { 2, 3 }, // 2
    { 3, 0 }, // 3
    { 4, 5 }, // 4
    { 5, 6 }, // 5
    { 6, 7 }, // 6
    { 7, 4 }, // 7
    { 0, 4 }, // 8
    { 1, 5 }, // 9
    { 2, 6 }, // 10
    { 3, 7 }  // 11
};

class Hexmesh
{
    public:

        Hexmesh(){}
        Hexmesh(const char * filename);
        Hexmesh(const std::vector<double> & coords,
                const std::vector<u_int>  & hexa);

        std::string filename;

        // bounding box
        //
        Bbox bb;

    protected:

        // serialized xyz coordinates, hexa and edges
        //
        std::vector<double> coords;
        std::vector<u_int>  hexa;
        std::vector<u_int>  edges;
        std::vector<u_int>  quads;    // exterior surface
        std::vector<bool>   v_on_srf;  // true if a vertex is on the surface, false otherwise
        std::vector<bool>   e_on_srf;  // true if a vertex is on the surface, false otherwise

        // per vertex/quad surface normals
        //
        std::vector<double> q_norm;

        // general purpose float and int scalars
        //
        std::vector<float> u_text;    // 1d texture per vertex
        std::vector<int>   t_label;   // per tet

        // adjacencies
        //
        std::vector< std::vector<int> > vtx2vtx;
        std::vector< std::vector<int> > vtx2edg;
        std::vector< std::vector<int> > vtx2hexa;
        std::vector< std::vector<int> > vtx2quad;
        std::vector< std::vector<int> > edg2hexa;
        std::vector< std::vector<int> > edg2quad;
        std::vector< std::vector<int> > hexa2edg;
        std::vector< std::vector<int> > hexa2hexa;
        std::vector< std::vector<int> > hexa2quad;
        std::vector< std::vector<int> > quad2quad;
        std::vector< std::vector<int> > quad2edg;
        std::vector< int >              quad2hexa;


    public:

        const std::vector<double> & vector_coords()         const;
        const std::vector<uint>   & vector_quads()          const;
        const std::vector<float>  & vector_v_float_scalar() const;
        const std::vector<int>    & vector_h_int_scalar()   const;


        std::string loaded_file() const;

        Trimesh export_surface() const;
        Trimesh export_surface(std::map<int,int> & hexa2quad_map, std::map<int,int> & quad2hex_map) const;

        virtual void operator+=(const Hexmesh & m);

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
        void update_q_normals();

        int num_vertices()      const;
        int num_hexahedra()    const;
        int num_elements()      const;
        int num_edges()         const;
        int num_srf_quads() const;

        const std::vector<int> & adj_vtx2vtx(const int vid) const;
        const std::vector<int> & adj_vtx2edg(const int vid) const;
        const std::vector<int> & adj_vtx2quad(const int vid) const;
        const std::vector<int> & adj_vtx2hexa(const int vid) const;
        const std::vector<int> & adj_edg2hexa(const int eid) const;
        const std::vector<int> & adj_edg2quad(const int eid) const;
        const std::vector<int> & adj_hexa2edg(const int hid) const;
        const std::vector<int> & adj_hexa2hexa(const int hid) const;
        const std::vector<int> & adj_hexa2quad(const int hid) const;
        const std::vector<int> & adj_quad2quad(const int hid) const;
        const std::vector<int> & adj_quad2edg(const int hid) const;
        const int              & adj_quad2hexa(const int hid) const;

        vec3d vertex(const int vid) const;

        void set_vertex_u_text(const int vid, const float val);

        void set_hex_label(const int tid, const int label);

        int hex_label(const int hid) const;

        float vertex_u_text(const int vid) const;

        float min_u_text() const;

        float max_u_text() const;

        int max_t_label() const;

        void set_vertex(const int vid, const vec3d & pos);

        bool is_surface_vertex(const int vid) const;

        bool is_surface_edge(const int eid) const;

        vec3d quad_normal(const int hid) const;

        vec3d hex_centroid(const int hid) const;

        int hex_vertex_id(const int tid, const int offset) const;

        vec3d hex_vertex(const int tid, const int offset) const;

        int quad_vertex_id(const int tid, const int offset) const;

        vec3d quad_vertex(const int tid, const int offset) const;

        bool hex_contains_vertex(const int tid, const std::set<int> & vids) const;

        bool hex_contains_vertex(const int tid, const int vid) const;

        bool hex_contains_edge(const int tid, const int eid) const;

        vec3d element_barycenter(const int hid) const;

        double avg_edge_length() const;
        double edge_length(const int eid) const;

        vec3d edge_vertex(const int eid, const int offset) const;

        double hex_quality(const int hid) const;

        bool hex_is_adjacent_to(const int tid, const int nbr) const;

        double vertex_quality(const int vid) const;

        int vertex_inverted_elements(const int vid) const;

        double hex_volume(const int hid) const;

        int edge_vertex_id(const int eid, const int offset) const;

        int adjacent_hex_through_facet(const int tid, const int facet);

        int shared_facet(const int hid0, const int hid1) const;

        double vertex_mass(const int vid) const;

        vec3d hex_face_normal(const int tid, const int fid) const;

        int hex_face_opposite_to(const int tid, const int vid) const;

        int hex_vertex_opposite_to(const int tid, const int facet) const;

        int hex_edge_opposite_to(const int tid, const int vid0, const int vid1) const;

        double hex_edge_length(const int tid, const int eid) const;

        double minimum_SJ() const;
        double average_SJ() const;
        void print_quality_statistics(bool list_folded_elements = false) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "hexmesh.cpp"
#endif

#endif // HEXMESH_H
