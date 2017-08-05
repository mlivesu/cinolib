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

template<class M = Mesh_min_attributes, // default template arguments
         class V = Vert_min_attributes,
         class E = Edge_min_attributes,
         class F = Polygon_min_attributes,
         class P = Polyhedron_min_attributes>
class Polyhedralmesh
{
    public:

        Polyhedralmesh(){}

        Polyhedralmesh(const char * filename);

        Polyhedralmesh(const std::vector<vec3d>             & verts,
                       const std::vector<std::vector<uint>> & faces,
                       const std::vector<std::vector<uint>> & polys,
                       const std::vector<std::vector<bool>> & polys_face_winding);

    protected:

        Bbox bb;

        std::vector<vec3d>             verts;
        std::vector<uint>              edges;
        std::vector<std::vector<uint>> faces;               // list of vertices (assumed CCW)
        std::vector<std::vector<uint>> triangulated_faces;  // triangles covering each face (e.g., for rendering)
        std::vector<std::vector<uint>> polys;               // unordered list of faces (<fid> => CCW, -<fid> => CW)
        std::vector<bool>              v_on_srf;            // true if a vertex is on the surface
        std::vector<bool>              e_on_srf;            // true if an edge is on the surface
        std::vector<bool>              f_on_srf;            // true if a face is on the surface

        std::vector<std::vector<bool>> polys_face_winding; // true if the face is CCW, false if it is CW
        // TODO!!

        // attributes
        //
        M              m_data;
        std::vector<V> v_data;
        std::vector<E> e_data;
        std::vector<F> f_data;
        std::vector<P> p_data;

        // adjacencies -- Yes, I have lots of memory ;)
        //
        std::vector<std::vector<uint>> v2v; // vert to vert adjacency
        std::vector<std::vector<uint>> v2e; // vert to edge adjacency
        std::vector<std::vector<uint>> v2f; // vert to face adjacency
        std::vector<std::vector<uint>> v2p; // vert to poly adjacency
        std::vector<std::vector<uint>> e2f; // edge to face adjacency
        std::vector<std::vector<uint>> e2p; // edge to poly adjacency
        std::vector<std::vector<uint>> f2e; // face to edge adjacency
        std::vector<std::vector<uint>> f2f; // face to face adjacency (through edges)
        std::vector<std::vector<uint>> f2p; // face to poly adjacency
        std::vector<std::vector<uint>> p2v; // poly to edge adjacency
        std::vector<std::vector<uint>> p2e; // poly to edge adjacency
        std::vector<std::vector<uint>> p2p; // poly to poly adjacency

    public:

        void clear();
        void init();
        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_bbox();
        void update_adjacency();
        void update_face_tessellation();
        void update_f_normals();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_face(const uint fid) const { return faces.at(fid).size(); }
        uint verts_per_poly(const uint pid) const { return p2v.at(pid).size();   }
        uint edges_per_poly(const uint pid) const { return p2e.at(pid).size();   }
        uint faces_per_poly(const uint pid) const { return polys.at(pid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_verts() const { return verts.size();     }
        uint num_edges() const { return edges.size() / 2; }
        uint num_faces() const { return faces.size();     }
        uint num_polys() const { return polys.size();     }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const Bbox                           & bbox()          const { return bb;    }
              std::vector<double>              vector_coords() const;
        const std::vector<vec3d>             & vector_verts()  const { return verts; }
        const std::vector<uint>              & vector_edges()  const { return edges; }
        const std::vector<std::vector<uint>> & vector_faces()  const { return faces; }
        const std::vector<std::vector<uint>> & vector_polys()  const { return polys; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<uint> & adj_v2v(const uint vid) const { return v2v.at(vid); }
        const std::vector<uint> & adj_v2e(const uint vid) const { return v2e.at(vid); }
        const std::vector<uint> & adj_v2f(const uint vid) const { return v2f.at(vid); }
        const std::vector<uint> & adj_v2p(const uint vid) const { return v2p.at(vid); }
        const std::vector<uint> & adj_e2f(const uint eid) const { return e2f.at(eid); }
        const std::vector<uint> & adj_e2p(const uint eid) const { return e2p.at(eid); }
        const std::vector<uint> & adj_f2e(const uint fid) const { return f2e.at(fid); }
        const std::vector<uint> & adj_f2f(const uint fid) const { return f2f.at(fid); }
        const std::vector<uint> & adj_f2p(const uint fid) const { return f2p.at(fid); }
        const std::vector<uint> & adj_p2v(const uint pid) const { return p2v.at(pid); }
        const std::vector<uint> & adj_p2e(const uint pid) const { return p2e.at(pid); }
        const std::vector<uint> & adj_p2p(const uint pid) const { return p2p.at(pid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const M & mesh_data()               const { return m_data;         }
              M & mesh_data()                     { return m_data;         }
        const V & vert_data(const uint vid) const { return v_data.at(vid); }
              V & vert_data(const uint vid)       { return v_data.at(vid); }
        const E & edge_data(const uint eid) const { return e_data.at(eid); }
              E & edge_data(const uint eid)       { return e_data.at(eid); }
        const F & face_data(const uint fid) const { return f_data.at(fid); }
              F & face_data(const uint fid)       { return f_data.at(fid); }
        const P & poly_data(const uint pid) const { return p_data.at(pid); }
              P & poly_data(const uint pid)       { return p_data.at(pid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  const vec3d & vert          (const uint vid) const { return verts.at(vid); }
        vec3d & vert          (const uint vid)       { return verts.at(vid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d edge_vert     (const uint eid, const uint off) const;
        uint  edge_vert_id  (const uint eid, const uint off) const;
        bool  edge_is_on_srf(const uint eid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d face_vert     (const uint fid, const uint off) const;
        uint  face_vert_id  (const uint fid, const uint off) const;
        bool  face_is_on_srf(const uint fid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool  poly_is_on_surf (const uint pid) const;
        uint  poly_face_id    (const uint pid, const uint off) const;
        bool  poly_face_is_CCW(const uint pid, const uint off) const;
        bool  poly_face_is_CW (const uint pid, const uint off) const;
        uint  poly_face_offset(const uint pid, const uint fid) const;
        vec3d poly_centroid   (const uint pid) const;
        void  poly_show_all();
};

}

#ifndef  CINO_STATIC_LIB
#include "polyhedralmesh.cpp"
#endif

#endif // CINO_POLYHEDRALMESH_H
