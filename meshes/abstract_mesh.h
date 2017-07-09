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

template<class M,
         class V,
         class E,
         class F>
class AbstractMesh
{
    protected:

        Bbox bb;

        std::vector<vec3d>             verts;
        std::vector<uint>              edges;
        std::vector<std::vector<uint>> faces;

        M              m_data;
        std::vector<V> v_data;
        std::vector<E> e_data;
        std::vector<F> f_data;

        std::vector<std::vector<uint>> v2v;
        std::vector<std::vector<uint>> v2e;
        std::vector<std::vector<uint>> v2f;
        std::vector<std::vector<uint>> e2f;
        std::vector<std::vector<uint>> f2e;
        std::vector<std::vector<uint>> f2f;

    public:

        virtual MeshType mesh_type() const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void clear();
        virtual void init();
        virtual void load(const char * filename) = 0;
        virtual void save(const char * filename) const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void center_bbox();
        virtual void update_bbox();
        virtual void update_adjacency() = 0;
        virtual void update_f_normal(const uint fid) = 0;
        virtual void update_v_normal(const uint vid);
        virtual void update_f_normals();
        virtual void update_v_normals();
        virtual void update_normals();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void  translate(const vec3d & delta);
        virtual void  rotate(const vec3d & axis, const double angle);
        virtual vec3d centroid() const;
        virtual uint  connected_components() const;
        virtual uint  connected_components(std::vector<std::set<uint>> & ccs) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint verts_per_face(const uint fid) const = 0;
        virtual uint verts_per_elem(const uint eid) const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint num_verts() const { return verts.size();     }
        virtual uint num_edges() const { return edges.size() / 2; }
        virtual uint num_faces() const { return faces.size();     }
        virtual uint num_elems() const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const Bbox                           & bbox()          const { return bb;    }
        virtual const std::vector<vec3d>             & vector_verts()  const { return verts; }
        virtual const std::vector<uint>              & vector_edges()  const { return edges; }
        virtual const std::vector<std::vector<uint>> & vector_faces()  const { return faces; }
        virtual       std::vector<double>              vector_coords() const ;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual std::vector<float> export_uvw_param(const int mode) const;
        virtual std::vector<int>   export_per_face_labels() const;
        virtual std::vector<Color> export_per_face_colors() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const std::vector<uint> & adj_v2v(const uint vid) const { return v2v.at(vid); }
        virtual const std::vector<uint> & adj_v2e(const uint vid) const { return v2e.at(vid); }
        virtual const std::vector<uint> & adj_v2f(const uint vid) const { return v2f.at(vid); }
        virtual const std::vector<uint> & adj_e2f(const uint eid) const { return e2f.at(eid); }
        virtual const std::vector<uint> & adj_f2e(const uint fid) const { return f2e.at(fid); }
        virtual const std::vector<uint> & adj_f2f(const uint fid) const { return f2f.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const std::vector<uint> & adj_vert2elem(const uint vid) const { return v2f.at(vid); }
        virtual const std::vector<uint> & adj_edge2elem(const uint eid) const { return e2f.at(eid); }
        virtual const std::vector<uint> & adj_elem2edge(const uint fid) const { return f2e.at(fid); }
        virtual const std::vector<uint> & adj_elem2elem(const uint eid) const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const M & mesh_data()               const { return m_data;         }
        virtual       M & mesh_data()                     { return m_data;         }
        virtual const V & vert_data(const uint vid) const { return v_data.at(vid); }
        virtual       V & vert_data(const uint vid)       { return v_data.at(vid); }
        virtual const E & edge_data(const uint eid) const { return e_data.at(eid); }
        virtual       E & edge_data(const uint eid)       { return e_data.at(eid); }
        virtual const F & face_data(const uint fid) const { return f_data.at(fid); }
        virtual       F & face_data(const uint fid)       { return f_data.at(fid); }
        virtual const F & elem_data(const uint eid) const = 0;
        virtual       F & elem_data(const uint eid)       = 0;

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

        virtual       vec3d  face_vert              (const uint fid, const uint offset) const;
        virtual       uint   face_vert_id           (const uint fid, const uint offset) const;
        virtual       uint   face_edge_id           (const uint fid, const uint vid0, const uint vid1) const;
        virtual       uint   face_vert_offset       (const uint fid, const uint vid) const;
        virtual       vec3d  face_centroid          (const uint fid) const;
        virtual       double face_angle_at_vert     (const uint fid, const uint vid, const int unit = RAD) const;
        virtual       bool   face_contains_vert     (const uint fid, const uint vid) const;
        virtual       bool   face_contains_edge     (const uint fid, const uint eid) const;
        virtual       void   face_flip_winding_order(const uint fid);
        virtual       void   face_set_color         (const Color & c);
        virtual       void   face_set_alpha         (const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // These are useful wraps for generic programming, because "elem_" will wrap
        // "face_" for surface meshes and will wrap "cell_" for volumetric meshes, thus
        // enabling the use of templated algorithms that work with both types of meshes
        // without requiring specialzed code

        virtual       void   elem_show_all   () = 0;
        virtual       vec3d  elem_vert       (const uint eid, const uint offset) const = 0;
        virtual       uint   elem_vert_id    (const uint eid, const uint offset) const = 0;
        virtual       uint   elem_vert_offset(const uint eid, const uint vid) const = 0;
        virtual       vec3d  elem_centroid   (const uint eid) const = 0;
        virtual       double elem_mass       (const uint eid) const = 0;
};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_mesh.cpp"
#endif

#endif //CINO_ABSTRACT_MESH_H
