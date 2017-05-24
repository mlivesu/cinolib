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
#ifndef CINO_HEXMESH_H
#define CINO_HEXMESH_H

#include <sys/types.h>
#include <vector>

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/quadmesh/quadmesh.h>
#include <cinolib/meshes/hexmesh/hexmesh_split_schemas.h>
#include <cinolib/meshes/mesh_std_data.h>

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

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data,
         class C = Cell_std_data>
class Hexmesh
{
    public:

        Hexmesh(){}

        Hexmesh(const char * filename);

        Hexmesh(const std::vector<double> & coords,
                const std::vector<uint>   & cells);

        Hexmesh(const std::vector<vec3d> & verts,
                const std::vector<uint>  & cells);

    protected:

        Bbox bb;

        std::vector<vec3d> verts;
        std::vector<uint>  edges;
        std::vector<uint>  faces;     // boundary only!
        std::vector<uint>  cells;
        std::vector<bool>  v_on_srf;  // true if a vertex is on the surface, false otherwise
        std::vector<bool>  e_on_srf;  // true if a vertex is on the surface, false otherwise

        // attributes
        //
        M              m_data;
        std::vector<V> v_data;
        std::vector<E> e_data;
        std::vector<F> f_data;
        std::vector<C> c_data;

        // adjacencies -- Yes, I have lots of memory ;)
        //
        std::vector<std::vector<uint>> v2v; // vert to vert adjacency
        std::vector<std::vector<uint>> v2e; // vert to edge adjacency
        std::vector<std::vector<uint>> v2f; // vert to face adjacency
        std::vector<std::vector<uint>> v2c; // vert to cell adjacency
        std::vector<std::vector<uint>> e2f; // edge to face adjacency
        std::vector<std::vector<uint>> e2c; // edge to cell adjacency
        std::vector<std::vector<uint>> f2e; // face to edge adjacency
        std::vector<std::vector<uint>> f2f; // face to face adjacency
        std::vector<uint>              f2c; // face to cell adjacency
        std::vector<std::vector<uint>> c2e; // cell to edge adjacency
        std::vector<std::vector<uint>> c2f; // cell to face adjacency
        std::vector<std::vector<uint>> c2c; // cell to cell adjacency

    public:

        void clear();
        void init();
        void print_quality(const bool list_folded_elements = false);
        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_bbox();
        void update_interior_adjacency();
        void update_surface_adjacency();
        void update_face_normals();
        void update_cell_quality(const uint cid);
        void update_cell_quality();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_face() const { return  4; }
        uint verts_per_cell() const { return  8; }
        uint edges_per_cell() const { return 12; }
        uint faces_per_cell() const { return  6; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_verts() const { return verts.size();                    }
        uint num_edges() const { return edges.size() / 2;                }
        uint num_faces() const { return faces.size() / verts_per_face(); }
        uint num_cells() const { return cells.size() / verts_per_cell(); }
        uint num_elems() const { return cells.size() / verts_per_cell(); } // elem == cell!!

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const Bbox                & bbox()          const { return bb;    }
              std::vector<double>   vector_coords() const;
        const std::vector<vec3d>  & vector_verts()  const { return verts; }
        const std::vector<uint>   & vector_edges()  const { return edges; }
        const std::vector<uint>   & vector_faces()  const { return faces; }
        const std::vector<uint>   & vector_cells()  const { return cells; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<float> export_uvw_param(const int mode) const;
        void               set_uvw_from_xyz(const int mode);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Quadmesh export_surface() const;
        Quadmesh export_surface(std::map<uint,uint> & c2f_map,
                                std::map<uint,uint> & f2c_map) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<uint> & adj_v2v(const uint vid) const { return v2v.at(vid); }
        const std::vector<uint> & adj_v2e(const uint vid) const { return v2e.at(vid); }
        const std::vector<uint> & adj_v2f(const uint vid) const { return v2f.at(vid); }
        const std::vector<uint> & adj_v2c(const uint vid) const { return v2c.at(vid); }
        const std::vector<uint> & adj_e2f(const uint eid) const { return e2f.at(eid); }
        const std::vector<uint> & adj_e2c(const uint eid) const { return e2c.at(eid); }
        const std::vector<uint> & adj_f2e(const uint fid) const { return f2e.at(fid); }
        const std::vector<uint> & adj_f2f(const uint fid) const { return f2f.at(fid); }
              uint                adj_f2c(const uint fid) const { return f2c.at(fid); }
        const std::vector<uint> & adj_c2e(const uint cid) const { return c2e.at(cid); }
        const std::vector<uint> & adj_c2f(const uint cid) const { return c2f.at(cid); }
        const std::vector<uint> & adj_c2c(const uint cid) const { return c2c.at(cid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const M & mesh_data()               const { return m_data;         }
              M & mesh_data()                     { return m_data;         }
        const V & vert_data(const uint vid) const { return v_data.at(vid); }
              V & vert_data(const uint vid)       { return v_data.at(vid); }
        const E & edge_data(const uint eid) const { return e_data.at(eid); }
              E & edge_data(const uint eid)       { return e_data.at(eid); }
        const F & face_data(const uint fid) const { return f_data.at(fid); }
              F & face_data(const uint fid)       { return f_data.at(fid); }
        const C & cell_data(const uint cid) const { return c_data.at(cid); }
              C & cell_data(const uint cid)       { return c_data.at(cid); }
        const C & elem_data(const uint cid) const { return c_data.at(cid); } // elem == cell!!
              C & elem_data(const uint cid)       { return c_data.at(cid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const vec3d & vert          (const uint vid) const { return verts.at(vid); }
              vec3d & vert          (const uint vid)       { return verts.at(vid); }
virtual       void    vert_set_color(const Color & c);
virtual       void    vert_set_alpha(const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d edge_vert     (const uint eid, const uint offset) const;
        uint  edge_vert_id  (const uint eid, const uint offset) const;
        bool  edge_is_on_srf(const uint eid) const;
virtual void  edge_set_color(const Color & c);
virtual void  edge_set_alpha(const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d face_vert         (const uint fid, const uint offset) const;
        uint  face_vert_id      (const uint fid, const uint offset) const;
        uint  face_edge_id      (const uint fid, const uint vid0, const uint vid1) const;
        vec3d face_centroid     (const uint fid) const;
        bool  face_contains_vert(const uint fid, const uint vid) const;
virtual void  face_set_color    (const Color & c);
virtual void  face_set_alpha    (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  cell_vert          (const uint cid, const uint off)   const;
        uint   cell_vert_id       (const uint cid, const uint off)   const;
        uint   cell_edge_id       (const uint cid, const uint vid0, const uint vid1) const;
        vec3d  cell_centroid      (const uint cid) const;
        int    cell_shared_face   (const uint cid0, const uint cid1) const;
        bool   cell_contains_vert (const uint cid, const uint vid)   const;
virtual void   cell_set_color     (const Color & c);
virtual void   cell_set_alpha     (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // These are all wraps for the "cell_ methods". They are useful for generic
        // programming, because "elem_" will wrap face_ for surface meshes and wrap
        // "cell_" for volumetric meshes, allowing the use of templated algorithms
        // that work with both types of meshes without requiring specialzed code

        vec3d elem_centroid(const uint cid) const;
        void  elem_show_all();
};

}

#ifndef  CINO_STATIC_LIB
#include "hexmesh.cpp"
#endif

#endif // CINO_HEXMESH_H


/*  TO BE INCLUDED IN THE DATA STRUCTURE TO HANDLE ELEMENT SPLITTING
// finds average point (adding it if needed)
// the second parameters contains the already added vertices
// as they should never be added twice
int _find_avg(vector<int> &points, map<vector<int>, int> &new_vertices)
{
    if (points.size() == 1) return points[0];

    sort(points.begin(), points.end());

    if (new_vertices.count(points)) return new_vertices[points];

    // add new point as the average of existing points
    vec3<real> new_point(0, 0, 0);
    for (int p: points)
    {
        new_point += vertex(p);
    }
    new_point /= points.size();
    add_vertex(new_point);
    int vertex_index = m_coords.size()/3 - 1;
    new_vertices[points] = vertex_index;
    return vertex_index;
}

void _apply_split_scheme(const vector<vector<vector<int>>> & split_scheme,
                         const vector<int> & old_hexes, // linearized input hexa (8 entries per element)
                         const int base_addr,           // 8 * hexa ID
                         map<vector<int>,int> & new_vertices)
{
    for (auto &hex: split_scheme)
    for (auto &hex_point: hex)
    {
        std::vector<int> points;  // points to be averaged
        for (int j: hex_point)
        {
            points.push_back(old_hexes[base_addr+j]);
        }
        m_hexes.push_back(_find_avg(points, new_vertices));
    }
}

*/
