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
#ifndef CINO_QUADMESH_H
#define CINO_QUADMESH_H

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/mesh_std_data.h>

#include <assert.h>
#include <float.h>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>
#include <cinolib/meshes/quadmesh/quadmesh.h>

namespace cinolib
{

static const uint QUAD_EDGES[4][2] =
{
    { 0, 1 }, // 0
    { 1, 2 }, // 1
    { 2, 3 }, // 2
    { 3, 0 }, // 3
};

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data>
class Quadmesh
{
    public:

        Quadmesh(){}

        Quadmesh(const char * filename);

        Quadmesh(const std::vector<vec3d> & verts,
                 const std::vector<uint>  & faces);

        Quadmesh(const std::vector<double> & coords,
                 const std::vector<uint>   & faces);

    protected:

        Bbox bb;

        std::vector<vec3d>             verts;
        std::vector<uint>              edges;
        std::vector<uint>              faces;
        std::vector<std::vector<uint>> tessellated_faces; // triangles covering each quad. Useful for
                                                          // robust normal estimation and rendering
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

        void update_adjacency();
        void update_bbox();
        void update_f_normals();
        void update_v_normals();
        void update_normals();
        void update_face_tessellation();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_face() const { return 4; }

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

  const vec3d           & vert                 (const uint vid) const { return verts.at(vid); }
        vec3d           & vert                 (const uint vid)       { return verts.at(vid); }
        uint              vert_opposite_to     (const uint eid, const uint vid) const;
        uint              vert_valence         (const uint vid) const;
        bool              verts_are_adjacent   (const uint vid0, const uint vid1) const;
        bool              verts_are_ordered_CCW(const uint fid, const uint curr, const uint prev) const;
        bool              vert_is_boundary     (const uint vid) const;
        bool              vert_is_singular     (const uint vid) const;
        bool              vert_is_regular      (const uint vid) const;        
        void              vert_set_color       (const Color & c);
        void              vert_set_alpha       (const float alpha);
        std::vector<uint> vert_boundary_edges  (const uint vid) const;
        std::vector<uint> vert_loop            (const uint start, const uint next) const;
        void              vert_ordered_one_ring(const uint vid,
                                                std::vector<uint> & v_ring,        // sorted list of adjacent vertices
                                                std::vector<uint> & f_ring,        // sorted list of adjacent quads
                                                std::vector<uint> & e_ring,        // sorted list of edges incident to vid
                                                std::vector<uint> & e_link) const; // sorted list of edges opposite to vid
        std::vector<uint> vert_ordered_vert_ring(const uint vid) const;
        std::vector<uint> vert_ordered_face_ring(const uint vid) const;
        std::vector<uint> vert_ordered_edge_ring(const uint vid) const;
        std::vector<uint> vert_ordered_edge_link(const uint vid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d edge_vert         (const uint eid, const uint offset) const;
        uint  edge_vert_id      (const uint eid, const uint offset) const;
        bool  edge_contains_vert(const uint eid, const uint vid) const;
        bool  edge_is_manifold  (const uint eid) const;
        bool  edge_is_boundary  (const uint eid) const;
        void  edge_set_color    (const Color & c);
        void  edge_set_alpha    (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint  face_vert_offset  (const uint fid, const uint vid) const;
        uint  face_vert_id      (const uint fid, const uint offset) const;
        vec3d face_vert         (const uint fid, const uint offset) const;
        vec3d face_centroid     (const uint fid) const;
        uint  face_edge_id      (const uint fid, const uint vid0, const uint vid1) const;
        bool  face_contains_vert(const uint fid, const uint vid) const;
        void  face_set_color    (const Color & c);
        void  face_set_alpha    (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // These are all wraps for the "face_ methods". They are useful for generic
        // programming, because "elem_" will wrap face_ for surface meshes and wrap
        // "cell_" for volumetric meshes, allowing the use of templated algorithms
        // that work with both types of meshes without requiring specialzed code

        vec3d elem_centroid(const uint fid) const;
        void  elem_show_all();
};

}

#ifndef  CINO_STATIC_LIB
#include "quadmesh.cpp"
#endif

#endif // CINO_QUADMESH_H
