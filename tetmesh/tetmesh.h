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

#include "../cino_inline.h"
#include "../common.h"
#include "../bbox.h"
#include "../vec3.h"
#include "../trimesh/trimesh.h"
#include "quality.h"

namespace cinolib
{


static int TET_FACES[4][3] =
{
    { 0, 2, 1 } ,
    { 0, 1, 3 } ,
    { 0, 3, 2 } ,
    { 1, 2, 3 }
};

static int TET_EDGES[6][2] =
{
    { 0, 2 }, // 0
    { 2, 1 }, // 1
    { 1, 0 }, // 2
    { 1, 3 }, // 3
    { 3, 0 }, // 4
    { 3, 2 }  // 5
};

class Tetmesh
{
    public:

        Tetmesh(){}
        Tetmesh(const char * filename);
        Tetmesh(const std::vector<double> & coords,
                const std::vector<u_int>  & tets);

        std::string filename;

        // bounding box
        //
        Bbox bb;

    protected:

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

        const std::vector<double> & vector_coords()    const { return coords; }
        const std::vector<uint>   & vector_tris()    const { return tris; }

        const std::vector<float> & vector_v_float_scalar() const { return u_text; }
        const std::vector<int>   & vector_t_int_scalar() const { return t_label; }


        std::string loaded_file() const { return filename; }

        Trimesh export_surface(std::map<int,int> & tet2tri_map, std::map<int,int> & tri2tet_map) const;

        virtual void operator+=(const Tetmesh & m);

        void init();
        void clear();
        void load(const char * filename);
        void save(const char * filename) const;

        void normalize_volume();

        bool empty() const { return coords.size() == 0; }

        void update_bbox();
        void center_bbox();
        void update_interior_adjacency();
        void update_surface_adjacency();

        void update_t_normals();

        int num_vertices()      const { return coords.size()/3; }
        int num_tetrahedra()    const { return tets.size()  /4; }
        int num_elements()      const { return tets.size()  /4; }
        int num_edges()         const { return edges.size() /2; }
        int num_srf_triangles() const { return tris.size()  /3; }

        const std::vector<int> & adj_vtx2vtx(const int vid) const { return vtx2vtx.at(vid); }
        const std::vector<int> & adj_vtx2edg(const int vid) const { return vtx2edg.at(vid); }
        const std::vector<int> & adj_vtx2tri(const int vid) const { return vtx2tri.at(vid); }
        const std::vector<int> & adj_vtx2tet(const int vid) const { return vtx2tet.at(vid); }
        const std::vector<int> & adj_edg2tet(const int eid) const { return edg2tet.at(eid); }
        const std::vector<int> & adj_edg2tri(const int eid) const { return edg2tri.at(eid); }
        const std::vector<int> & adj_tet2edg(const int tid) const { return tet2edg.at(tid); }
        const std::vector<int> & adj_tet2tet(const int tid) const { return tet2tet.at(tid); }
        const std::vector<int> & adj_tet2tri(const int tid) const { return tet2tri.at(tid); }
        const std::vector<int> & adj_tri2tri(const int tid) const { return tri2tri.at(tid); }
        const std::vector<int> & adj_tri2edg(const int tid) const { return tri2edg.at(tid); }
        const int              & adj_tri2tet(const int tid) const { return tri2tet.at(tid); }

        vec3d vertex(const int vid) const
        {
            int vid_ptr = vid * 3;
            return vec3d(coords[vid_ptr+0], coords[vid_ptr+1], coords[vid_ptr+2]);
        }

        void set_vertex_u_text(const int vid, const float val)
        {
            u_text.at(vid) = val;
        }

        void set_tet_label(const int tid, const int label)
        {
            t_label.at(tid) = label;
        }

        int tet_label(const int tid) const
        {
            return t_label.at(tid);
        }

        float vertex_u_text(const int vid) const
        {
            return u_text.at(vid);
        }

        float min_u_text() const
        {
            return *std::min_element(u_text.begin(), u_text.end());
        }

        float max_u_text() const
        {
            return *std::max_element(u_text.begin(), u_text.end());
        }

        int max_t_label() const
        {
            return *std::max_element(t_label.begin(), t_label.end());
        }

        void set_vertex(const int vid, const vec3d & pos)
        {
            int vid_ptr = vid * 3;
            coords[vid_ptr + 0] = pos.x();
            coords[vid_ptr + 1] = pos.y();
            coords[vid_ptr + 2] = pos.z();
        }

        bool is_surface_vertex(const int vid) const
        {
            return v_on_srf[vid];
        }

        bool is_surface_edge(const int eid) const
        {
            return e_on_srf[eid];
        }

        vec3d triangle_normal(const int tid) const
        {
            int tid_ptr = tid * 3;
            return vec3d(t_norm[tid_ptr + 0], t_norm[tid_ptr + 1], t_norm[tid_ptr + 2]);
        }

        vec3d tet_centroid(const int tid) const
        {
            return (tet_vertex(tid,0) +
                    tet_vertex(tid,1) +
                    tet_vertex(tid,2) +
                    tet_vertex(tid,3))* 0.25;
        }

        int tet_vertex_id(const int tid, const int offset) const
        {
            int tid_ptr = tid * 4;
            return tets[tid_ptr + offset];
        }

        vec3d tet_vertex(const int tid, const int offset) const
        {
            int tid_ptr = tid * 4;
            return vertex(tets[tid_ptr + offset]);
        }

        int tri_vertex_id(const int tid, const int offset) const
        {
            int tid_ptr = tid * 3;
            return tris[tid_ptr + offset];
        }

        vec3d tri_vertex(const int tid, const int offset) const
        {
            int tid_ptr = tid * 3;
            return vertex(tris[tid_ptr + offset]);
        }

        bool tet_contains_vertex(const int tid, const std::set<int> & vids) const
        {
            for(int vid : vids)
            {
                if (tet_contains_vertex(tid, vid)) return true;
            }
            return false;
        }

        bool tet_contains_vertex(const int tid, const int vid) const
        {
            if (tet_vertex_id(tid, 0) == vid) return true;
            if (tet_vertex_id(tid, 1) == vid) return true;
            if (tet_vertex_id(tid, 2) == vid) return true;
            if (tet_vertex_id(tid, 3) == vid) return true;
            return false;
        }

        bool tet_contains_edge(const int tid, const int eid) const
        {
            for(int nbr : adj_tet2edg(tid))
            {
                if (nbr == eid) return true;
            }
            return false;
        }

        vec3d element_barycenter(const int tid) const;

        double avg_edge_length() const;
        double edge_length(const int eid) const;

        vec3d edge_vertex(const int eid, const int offset) const
        {
            int eid_ptr = eid * 2;
            int vid     = edges[eid_ptr + offset];
            int vid_ptr = vid * 3;
            return vec3d(coords[vid_ptr + 0], coords[vid_ptr + 1], coords[vid_ptr + 2]);
        }

        double tet_quality(const int tid) const
        {
            return tet_scaled_jacobian(tet_vertex(tid,0),
                                       tet_vertex(tid,1),
                                       tet_vertex(tid,2),
                                       tet_vertex(tid,3));
        }

        bool tet_is_adjacent_to(const int tid, const int nbr) const;

        double vertex_quality(const int vid) const;

        int vertex_inverted_elements(const int vid) const;

        double tet_volume(const int tid) const
        {
            return tet_unsigned_volume(tet_vertex(tid,0),
                                       tet_vertex(tid,1),
                                       tet_vertex(tid,2),
                                       tet_vertex(tid,3));
        }

        int edge_vertex_id(const int eid, const int offset) const
        {
            int eid_ptr = eid * 2;
            return edges[eid_ptr + offset];
        }

        void print_quality_statistics(bool list_folded_elements = false) const;

        int adjacent_tet_through_facet(const int tid, const int facet);

        int shared_facet(const int tid0, const int tid1) const;

        double vertex_mass(const int vid) const;

        int vertex_valence(const int vid) const
        {
            return adj_vtx2vtx(vid).size();
        }

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
};

}

#ifndef  CINO_STATIC_LIB
#include "tetmesh.cpp"
#endif

#endif // TETMESH_H
