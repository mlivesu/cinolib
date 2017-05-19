/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
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
#ifndef CINO_POLYGONMESH_H
#define CINO_POLYGONMESH_H

#include <vector>
#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/mesh_std_data.h>


namespace cinolib
{

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data>
class Polygonmesh
{
    public:

        Polygonmesh(){}

        Polygonmesh(const char * filename);

        Polygonmesh(const std::vector<vec3d>             & verts,
                    const std::vector<std::vector<uint>> & faces);

        Polygonmesh(const std::vector<double>            & coords,
                    const std::vector<std::vector<uint>> & faces);

    protected:

        Bbox bb;

        std::vector<vec3d>             verts;
        std::vector<uint>              edges;
        std::vector<std::vector<uint>> faces;

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

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_face(const uint fid) const { return faces.at(fid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_verts() const { return verts.size();   }
        uint num_edges() const { return edges.size()/2; }
        uint num_faces() const { return faces.size();   }
        uint num_elems() const { return faces.size();   } // elem == face!!

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const Bbox               & bbox()         const { return bb;    }
        const std::vector<vec3d> & vector_verts() const { return verts; }
        const std::vector<uint>  & vector_edges() const { return edges; }

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

        const vec3d vert(const uint vid) const { return verts.at(vid); }
              vec3d vert(const uint vid)       { return verts.at(vid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d edge_vert   (const uint eid, const uint offset) const;
        uint  edge_vert_id(const uint eid, const uint offset) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d face_vert    (const uint fid, const uint offset) const;
        uint  face_vert_id (const uint fid, const uint offset) const;
        vec3d face_centroid(const uint fid)                    const;

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
#include "polygonmesh.cpp"
#endif

#endif // CINO_POLYGONMESH_H
