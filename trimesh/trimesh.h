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

static int TRI_EDGES[3][2] =
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

    public:

        void init();
        void clear();
        void update_adjacency();
        void update_t_normals();
        void update_v_normals();

        std::string loaded_file() const { return filename; }

        int num_vertices()  const { return coords.size()/3; }
        int num_triangles() const { return tris.size()  /3; }
        int num_elements()  const { return tris.size()  /3; }
        int num_edges()     const { return edges.size() /2; }

        const std::vector<int> & adj_vtx2vtx(const int vid) const { return vtx2vtx.at(vid); }
        const std::vector<int> & adj_vtx2edg(const int vid) const { return vtx2edg.at(vid); }
        const std::vector<int> & adj_vtx2tri(const int vid) const { return vtx2tri.at(vid); }
        const std::vector<int> & adj_edg2tri(const int eid) const { return edg2tri.at(eid); }
        const std::vector<int> & adj_tri2edg(const int tid) const { return tri2edg.at(tid); }
        const std::vector<int> & adj_tri2tri(const int tid) const { return tri2tri.at(tid); }

        std::vector<int> adj_vtx2vtx_ordered(const int vid) const;

        const std::vector<double> & vector_coords()    const { return coords; }
        const std::vector<u_int>  & vector_triangles() const { return tris;   }
        const std::vector<u_int>  & vector_edges()     const { return edges;  }
        const Bbox                & bbox()             const { return bb;     }

        const std::vector<float> & vector_v_float_scalar() const { return u_text; }
        const std::vector<int>   & vector_t_int_scalar() const { return t_label; }

        void save(const char * filename) const;

        virtual int add_vertex(const vec3d & v, const float scalar = 0.0);
        virtual int add_triangle(const int vid0, const int vid1, const int vid2, const int scalar);

        void set_triangle(const int tid, const int vid0, const int vid1, const int vid2);

        std::set<int> vertex_n_ring(const int vid, const int n) const;

        double element_mass(const int tid) const;
        double vertex_mass (const int vid) const;

        std::vector<int>  get_boundary_vertices() const;
        std::vector<ipair> get_boundary_edges()    const;

        int vertex_opposite_to(const int tid, const int vid0, const int vid1) const;
        int vertex_opposite_to(const int eid, const int vid) const;
        int edge_opposite_to(const int tid, const int vid) const;

        double angle_at_vertex(const int tid, const int vid) const;

        double edge_length(const int eid) const;
        double avg_edge_length() const;
        double max_edge_length() const;
        double min_edge_length() const;

        vec3d triangle_vertex(const int tid, const int offset) const
        {
            int tid_ptr = tid * 3;
            int vid     = tris[tid_ptr + offset];
            int vid_ptr = vid * 3;
            return vec3d(coords[vid_ptr + 0], coords[vid_ptr + 1], coords[vid_ptr + 2]);
        }

        bool edge_contains_vertex(const int eid, const int vid) const
        {
            if (edge_vertex_id(eid,0) == vid) return true;
            if (edge_vertex_id(eid,1) == vid) return true;
            return false;
        }

        vec3d edge_vertex(const int eid, const int offset) const
        {
            int eid_ptr = eid * 2;
            int vid     = edges[eid_ptr + offset];
            int vid_ptr = vid * 3;
            return vec3d(coords[vid_ptr + 0], coords[vid_ptr + 1], coords[vid_ptr + 2]);
        }

        bool vertex_is_border(const int vid) const; // has adjacent triangles with different per triangle iscalars

        bool vertex_is_boundary(const int vid) const; // is along an open boundary

        bool edge_is_manifold(const int eid) const
        {
            return (adj_edg2tri(eid).size() == 2);
        }

        bool edge_is_boundary(const int eid) const
        {
            return (adj_edg2tri(eid).size() < 2);
        }

        bool edge_is_border(const int eid) const
        {
            assert(edge_is_manifold(eid));

            int tid_0 = adj_edg2tri(eid)[0];
            int tid_1 = adj_edg2tri(eid)[1];

            return (triangle_label(tid_0) != triangle_label(tid_1));
        }

        bool triangle_is_boundary(const int tid) const
        {
            return (adj_tri2tri(tid).size() < 3);
        }

        int edge_vertex_id(const int eid, const int offset) const
        {
            int eid_ptr = eid * 2;
            return edges[eid_ptr + offset];
        }

        int triangle_vertex_id(const int tid, const int offset) const
        {
            int tid_ptr = tid * 3;
            return tris[tid_ptr + offset];
        }

        vec3d triangle_normal(const int tid) const
        {
            int tid_ptr = tid * 3;
            return vec3d(t_norm[tid_ptr + 0], t_norm[tid_ptr + 1], t_norm[tid_ptr + 2]);
        }

        vec3d vertex_normal(const int vid) const
        {
            int vid_ptr = vid * 3;
            return vec3d(v_norm[vid_ptr+0], v_norm[vid_ptr+1], v_norm[vid_ptr+2]);
        }

        vec3d vertex(const int vid) const
        {
            int vid_ptr = vid * 3;
            return vec3d(coords[vid_ptr+0], coords[vid_ptr+1], coords[vid_ptr+2]);
        }

        void set_vertex(const int vid, const vec3d & pos)
        {
            int vid_ptr = vid * 3;
            coords[vid_ptr + 0] = pos.x();
            coords[vid_ptr + 1] = pos.y();
            coords[vid_ptr + 2] = pos.z();
        }

        int vertex_valence(const int vid) const
        {
            return adj_vtx2vtx(vid).size();
        }

        float triangle_min_u_text(const int tid) const
        {
            float vals[3] =
            {
                vertex_u_text(triangle_vertex_id(tid,0)),
                vertex_u_text(triangle_vertex_id(tid,1)),
                vertex_u_text(triangle_vertex_id(tid,2)),
            };
            return *std::min_element(vals, vals+3);
        }

        float vertex_u_text(const int vid) const
        {
            CHECK_BOUNDS(u_text, vid);
            return u_text[vid];
        }

        void set_vertex_u_text(const int vid, const float s)
        {
            CHECK_BOUNDS(u_text, vid);
            u_text[vid] = s;
        }

        int vertex_with_min_u_text() const
        {
            return (std::min_element(u_text.begin(), u_text.end()) - u_text.begin());
        }

        int triangle_label(const int tid) const
        {
            CHECK_BOUNDS(t_label, tid);
            return t_label[tid];
        }

        void set_triangle_label(const int tid, const int i)
        {
            CHECK_BOUNDS(t_label, tid);
            t_label[tid] = i;
        }

        void update_normals()
        {
            update_t_normals();
            update_v_normals();
        }

        bool triangle_contains_vertex(const int tid, const int vid) const
        {
            if (triangle_vertex_id(tid, 0) == vid) return true;
            if (triangle_vertex_id(tid, 1) == vid) return true;
            if (triangle_vertex_id(tid, 2) == vid) return true;
            return false;
        }

        ipair shared_edge(const int tid0, const int tid1) const;

        int shared_vertex(const int eid0, const int eid1) const;

        int shared_triangle(const int eid0, const int eid1) const;

        bool edges_share_same_triangle(const int eid1, const int eid2) const;

        int triangle_adjacent_along(const int tid, const int vid0, const int vid1) const;

        vec3d element_barycenter(const int tid) const;

        void normalize_area();
        void center_bbox();
        void update_bbox();        

        void translate(const vec3d & delta);
        void rotate(const vec3d & axis, const double angle);

        virtual void operator+=(const Trimesh & m);

        void remove_duplicated_vertices(const double eps = 1e-7);
        void remove_duplicated_triangles();

        int connected_components(std::vector< std::set<int> > & ccs) const;
        int connected_components() const;
};

}

#ifndef  CINO_STATIC_LIB
#include "trimesh.cpp"
#endif

#endif // TRIMESH_H
