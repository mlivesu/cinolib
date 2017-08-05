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
#ifndef CINO_ABSTRACT_MESH_H
#define CINO_ABSTRACT_MESH_H

#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/color.h>
#include <cinolib/common.h>

#include <set>
#include <vector>

typedef enum
{
    TRIMESH       ,
    QUADMESH      ,
    POLYGONMESH   ,
    TETMESH       ,
    HEXMESH       ,
    POLYHEDRALMESH,
}
MeshType;


namespace cinolib
{

template<class M, // mesh attributes
         class V, // vert attributes
         class E, // edge attributes
         class P> // polygon attributes for surface meshes, polyhedra attributes for volume meshes
class AbstractMesh
{
    protected:

        Bbox bb;

        std::vector<vec3d>             verts;
        std::vector<uint>              edges;
        std::vector<std::vector<uint>> polys;

        M              m_data;
        std::vector<V> v_data;
        std::vector<E> e_data;
        std::vector<P> p_data;

        std::vector<std::vector<uint>> v2v; // vert to vert adjacency
        std::vector<std::vector<uint>> v2e; // vert to edge adjacency
        std::vector<std::vector<uint>> v2p; // vert to poly adjacency
        std::vector<std::vector<uint>> e2p; // edge to poly adjacency
        std::vector<std::vector<uint>> p2e; // poly to edge adjacency
        std::vector<std::vector<uint>> p2p; // poly to poly adjacency

    public:

                 AbstractMesh() {}
        virtual ~AbstractMesh() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual MeshType mesh_type() const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void clear();
        virtual void init() = 0;
        virtual void load(const char * filename) = 0;
        virtual void save(const char * filename) const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void center_bbox();
        virtual void update_bbox();
        virtual void update_adjacency() = 0;
        virtual void update_normals() = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void  translate(const vec3d & delta);
        virtual void  rotate(const vec3d & axis, const double angle);
        virtual void  scale(const double scale_factor);
        virtual vec3d centroid() const;
        virtual uint  connected_components() const;
        virtual uint  connected_components(std::vector<std::set<uint>> & ccs) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint verts_per_poly(const uint pid) const { return polys.at(pid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint num_verts() const { return verts.size();     }
        virtual uint num_edges() const { return edges.size() / 2; }
        virtual uint num_polys() const { return polys.size();     }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const Bbox                           & bbox()          const { return bb;    }
        virtual const std::vector<vec3d>             & vector_verts()  const { return verts; }
        virtual const std::vector<uint>              & vector_edges()  const { return edges; }
        virtual const std::vector<std::vector<uint>> & vector_polys()  const { return polys; }
        virtual       std::vector<double>              vector_coords() const ;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual std::vector<float> export_uvw_param(const int mode) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const std::vector<uint> & adj_v2v(const uint vid) const { return v2v.at(vid); }
        virtual const std::vector<uint> & adj_v2e(const uint vid) const { return v2e.at(vid); }
        virtual const std::vector<uint> & adj_v2p(const uint vid) const { return v2p.at(vid); }
        virtual const std::vector<uint> & adj_e2p(const uint eid) const { return e2p.at(eid); }
        virtual const std::vector<uint> & adj_p2e(const uint pid) const { return p2e.at(pid); }
        virtual const std::vector<uint> & adj_p2p(const uint pid) const { return p2p.at(pid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const M & mesh_data()               const { return m_data;         }
        virtual       M & mesh_data()                     { return m_data;         }
        virtual const V & vert_data(const uint vid) const { return v_data.at(vid); }
        virtual       V & vert_data(const uint vid)       { return v_data.at(vid); }
        virtual const E & edge_data(const uint eid) const { return e_data.at(eid); }
        virtual       E & edge_data(const uint eid)       { return e_data.at(eid); }
        virtual const P & poly_data(const uint pid) const { return p_data.at(pid); }
        virtual       P & poly_data(const uint pid)       { return p_data.at(pid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const vec3d          & vert                 (const uint vid) const { return verts.at(vid); }
        virtual       vec3d          & vert                 (const uint vid)       { return verts.at(vid); }
        virtual       std::set<uint>   vert_n_ring          (const uint vid, const uint n) const;
        virtual       bool             verts_are_adjacent   (const uint vid0, const uint vid1) const;
        virtual       bool             vert_is_local_min    (const uint vid, const int tex_coord = U_param) const;
        virtual       bool             vert_is_local_max    (const uint vid, const int tex_coord = U_param) const;
        virtual       uint             vert_valence         (const uint vid) const;
        virtual       uint             vert_shared          (const uint eid0, const uint eid1) const;
        virtual       void             vert_set_color       (const Color & c);
        virtual       void             vert_set_alpha       (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual       vec3d  edge_vert         (const uint eid, const uint offset) const;
        virtual       uint   edge_vert_id      (const uint eid, const uint offset) const;
        virtual       bool   edge_contains_vert(const uint eid, const uint vid) const;
        virtual       double edge_length       (const uint eid) const;
        virtual       double edge_avg_length   () const;
        virtual       double edge_max_length   () const;
        virtual       double edge_min_length   () const;
        virtual       void   edge_set_color    (const Color & c);
        virtual       void   edge_set_alpha    (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual       vec3d  poly_vert         (const uint fid, const uint offset) const;
        virtual       uint   poly_vert_id      (const uint fid, const uint offset) const;
        virtual       uint   poly_vert_offset  (const uint fid, const uint vid) const;
        virtual       vec3d  poly_centroid     (const uint fid) const;
        virtual       uint   poly_edge_id      (const uint fid, const uint vid0, const uint vid1) const;
        virtual       bool   poly_contains_vert(const uint fid, const uint vid) const;
        virtual       bool   poly_contains_edge(const uint fid, const uint eid) const;
        virtual       double poly_mass         (const uint fid) const = 0;
        virtual       void   poly_show_all     ();
        virtual       void   poly_set_color    (const Color & c) = 0;
        virtual       void   poly_set_alpha    (const float alpha) = 0;
};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_mesh.cpp"
#endif

#endif //CINO_ABSTRACT_MESH_H
