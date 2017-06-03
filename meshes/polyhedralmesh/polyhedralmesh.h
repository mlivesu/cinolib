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
#ifndef CINO_POLYHEDRALMESH_H
#define CINO_POLYHEDRALMESH_H

#include <sys/types.h>
#include <vector>

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/meshes/polygonmesh/polygonmesh.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/mesh_std_data.h>

namespace cinolib
{

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data,
         class C = Cell_std_data>
class Polyhedralmesh
{
    public:

        Polyhedralmesh(){}

        Polyhedralmesh(const std::vector<vec3d>             & verts,
                       const std::vector<std::vector<uint>> & faces,
                       const std::vector<std::vector<int>>  & cells);

    protected:

        Bbox bb;

        std::vector<vec3d>             verts;
        std::vector<uint>              edges;
        std::vector<std::vector<uint>> faces;             // list of vertices (assumed CCW)
        std::vector<std::vector<uint>> tessellated_faces; // triangles covering each face (e.g., for rendering)
        std::vector<std::vector<int>>  cells;             // unordered list of faces (<fid> => CCW, -<fid> => CW)
        std::vector<bool>              v_on_srf;          // true if a vertex is on the surface
        std::vector<bool>              e_on_srf;          // true if an edge is on the surface
        std::vector<bool>              f_on_srf;          // true if a face is on the surface


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
        std::vector<std::vector<uint>> f2f; // face to face adjacency (through edges)
        std::vector<std::vector<uint>> f2c; // face to cell adjacency
        std::vector<std::vector<uint>> c2v; // cell to edge adjacency
        std::vector<std::vector<uint>> c2e; // cell to edge adjacency
        std::vector<std::vector<uint>> c2c; // cell to cell adjacency

    public:

        void clear();
        void init();
        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_bbox();
        void update_adjacency();
        void update_face_tessellation();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_face(const uint fid) const { return faces.at(fid).size(); }
        uint edges_per_cell(const uint cid) const { return c2e.at(cid).size();   }
        uint faces_per_cell(const uint cid) const { return cells.at(cid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_verts() const { return verts.size();     }
        uint num_edges() const { return edges.size() / 2; }
        uint num_faces() const { return faces.size();     }
        uint num_cells() const { return cells.size();     }
        uint num_elems() const { return cells.size();     } // elem == cell!!

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const Bbox                           & bbox()          const { return bb;    }
              std::vector<double>              vector_coords() const;
        const std::vector<vec3d>             & vector_verts()  const { return verts; }
        const std::vector<uint>              & vector_edges()  const { return edges; }
        const std::vector<std::vector<uint>> & vector_faces()  const { return faces; }
        const std::vector<std::vector<uint>> & vector_cells()  const { return cells; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<uint> & adj_v2v(const uint vid) const { return v2v.at(vid); }
        const std::vector<uint> & adj_v2e(const uint vid) const { return v2e.at(vid); }
        const std::vector<uint> & adj_v2f(const uint vid) const { return v2f.at(vid); }
        const std::vector<uint> & adj_v2c(const uint vid) const { return v2c.at(vid); }
        const std::vector<uint> & adj_e2f(const uint eid) const { return e2f.at(eid); }
        const std::vector<uint> & adj_e2c(const uint eid) const { return e2c.at(eid); }
        const std::vector<uint> & adj_f2e(const uint fid) const { return f2e.at(fid); }
        const std::vector<uint> & adj_f2f(const uint fid) const { return f2f.at(fid); }
        const std::vector<uint> & adj_f2c(const uint fid) const { return f2c.at(fid); }
        const std::vector<uint> & adj_c2v(const uint cid) const { return c2v.at(cid); }
        const std::vector<uint> & adj_c2e(const uint cid) const { return c2e.at(cid); }
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

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint face_vert_id(const uint fid, const uint off) const { return faces.at(fid).at(off); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
};

}

#ifndef  CINO_STATIC_LIB
#include "polyhedralmesh.cpp"
#endif

#endif // CINO_POLYHEDRALMESH_H
