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
#ifndef CINO_TRIMESH_H
#define CINO_TRIMESH_H

#include <cinolib/cinolib.h>
#include <cinolib/symbols.h>
#include <cinolib/meshes/mesh_std_data.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/meshes/abstract_surface_mesh.h>

namespace cinolib
{

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data>
class Trimesh : public virtual AbstractSurfaceMesh<M,V,E,F>
{
    public:

        Trimesh(){}

        Trimesh(const char * filename);

        Trimesh(const std::vector<vec3d>             & coords,
                const std::vector<std::vector<uint>> & faces);

        Trimesh(const std::vector<double>            & coords,
                const std::vector<std::vector<uint>> & faces);

        Trimesh(const std::vector<vec3d> & coords,
                const std::vector<uint>  & faces);

        Trimesh(const std::vector<double> & coords,
                const std::vector<uint>   & faces);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const { return TRIMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void clear();
        void init();
        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void operator+=(const Trimesh & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_adjacency();
        void update_f_normal(const uint fid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_face(const uint) const { return 3; }
        uint verts_per_elem(const uint) const { return 3; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_elems() const { return this->faces.size(); } // elem == face!!

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<uint> & adj_elem2elem(const uint fid) const { return this->f2f.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const F & elem_data(const uint fid) const { return this->f_data.at(fid); } // elem == face!!
              F & elem_data(const uint fid)       { return this->f_data.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void               center_bbox();
        void               normalize_area();
        void               translate(const vec3d & delta);
        void               rotate(const vec3d & axis, const double angle);
        vec3d              centroid() const;
        uint               connected_components() const;
        uint               connected_components(std::vector<std::set<uint>> & ccs) const;
        std::vector<uint>  get_boundary_vertices() const;
        std::vector<ipair> get_boundary_edges() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void              vert_ordered_one_ring   (const uint vid,
                                                   std::vector<uint> & v_ring,        // sorted list of adjacent vertices
                                                   std::vector<uint> & f_ring,        // sorted list of adjacent triangles
                                                   std::vector<uint> & e_ring,        // sorted list of edges incident to vid
                                                   std::vector<uint> & e_link) const; // sorted list of edges opposite to vid
        std::vector<uint> vert_ordered_vert_ring  (const uint vid) const;
        std::vector<uint> vert_ordered_face_ring  (const uint vid) const;
        std::vector<uint> vert_ordered_edge_ring  (const uint vid) const;
        std::vector<uint> vert_ordered_edge_link  (const uint vid) const;
        double            vert_area               (const uint vid) const;
        double            vert_mass               (const uint vid) const;
        bool              vert_is_boundary        (const uint vid) const;
        bool              vert_is_saddle          (const uint vid, const int tex_coord = U_param) const;
        bool              vert_is_critical_p      (const uint vid, const int tex_coord = U_param) const;
        uint              vert_opposite_to        (const uint fid, const uint vid0, const uint vid1) const;
        uint              vert_opposite_to        (const uint eid, const uint vid) const;
        std::vector<uint> vert_boundary_edges     (const uint vid) const;
        void              vert_switch_id          (const uint vid0, const uint vid1);
        void              vert_remove_unreferenced(const uint vid);
        uint              vert_add                (const vec3d & pos);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int    edge_opposite_to        (const uint fid, const uint vid) const;
        bool   edge_is_manifold        (const uint eid) const;
        bool   edge_is_boundary        (const uint eid) const;
        bool   edges_share_face        (const uint eid1, const uint eid2) const;
        ipair  edge_shared             (const uint fid0, const uint fid1) const;
        void   edge_switch_id          (const uint eid0, const uint eid1);
        bool   edge_collapse           (const uint eid);
        uint   edge_add                (const uint vid0, const uint vid1);
        void   edge_remove_unreferenced(const uint eid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        //using  AbstractSurfaceMesh<M,V,E,F>::face_edge_id; // tell the compiler we want both the get from Abstand ours
        uint   face_edge_id            (const uint fid, const uint offset) const;
        int    face_shared             (const uint eid0, const uint eid1) const;
        double face_mass               (const uint fid) const;
        double face_area               (const uint fid) const;
        bool   face_is_cap             (const uint fid, const double angle_thresh_deg = 177.0) const;
        bool   face_is_needle          (const uint fid, const double angle_thresh_deg = 3.0) const;
        int    face_adjacent_along     (const uint fid, const uint vid0, const uint vid1) const;
        bool   face_is_boundary        (const uint fid) const;
        bool   face_bary_coords        (const uint fid, const vec3d & P, std::vector<double> & wgts) const;
        bool   face_bary_is_vert       (const uint fid, const std::vector<double> & wgts, uint & vid, const double tol = 1e-5) const;
        bool   face_bary_is_edge       (const uint fid, const std::vector<double> & wgts, uint & eid, const double tol = 1e-5) const;
        void   face_switch_id          (const uint fid0, const uint fid1);
        uint   face_add                (const uint vid0, const uint vid1, const uint vid2);
        void   face_set                (const uint fid, const uint vid0, const uint vid1, const uint vid2);
        void   face_remove_unreferenced(const uint fid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // These are all wraps for the "face_ methods". They are useful for generic
        // programming, because "elem_" will wrap face_ for surface meshes and wrap
        // "cell_" for volumetric meshes, allowing the use of templated algorithms
        // that work with both types of meshes without requiring specialzed code

        vec3d  elem_vert        (const uint fid, const uint offset) const;
        uint   elem_vert_id     (const uint fid, const uint offset) const;
        uint   elem_vert_offset (const uint fid, const uint vid) const;
        vec3d  elem_centroid    (const uint fid) const;
        void   elem_show_all    ();
        double elem_mass        (const uint fid) const;
        bool   elem_bary_coords (const uint fid, const vec3d & P, std::vector<double> & wgts) const;
        bool   elem_bary_is_vert(const uint fid, const std::vector<double> & wgts, uint & vid, const double tol = 1e-5) const;
        bool   elem_bary_is_edge(const uint fid, const std::vector<double> & wgts, uint & eid, const double tol = 1e-5) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "trimesh.cpp"
#endif

#endif // CINO_TRIMESH_H
