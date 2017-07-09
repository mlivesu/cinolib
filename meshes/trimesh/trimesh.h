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
class Trimesh : public AbstractSurfaceMesh<M,V,E,F>
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

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void operator+=(const Trimesh & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_f_normal(const uint fid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint              vert_opposite_to        (const uint fid, const uint vid0, const uint vid1) const;
                          using AbstractSurfaceMesh<M,V,E,F>::vert_opposite_to; // avoid to hide the most general method
        std::vector<uint> vert_boundary_edges     (const uint vid) const;
        void              vert_switch_id          (const uint vid0, const uint vid1);
        void              vert_remove_unreferenced(const uint vid);
        uint              vert_add                (const vec3d & pos);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int    edge_opposite_to        (const uint fid, const uint vid) const;
        void   edge_switch_id          (const uint eid0, const uint eid1);
        bool   edge_collapse           (const uint eid);
        uint   edge_add                (const uint vid0, const uint vid1);
        void   edge_remove_unreferenced(const uint eid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double face_area               (const uint fid) const;
        uint   face_edge_id            (const uint fid, const uint offset) const;
        bool   face_is_cap             (const uint fid, const double angle_thresh_deg = 177.0) const;
        bool   face_is_needle          (const uint fid, const double angle_thresh_deg = 3.0) const;
        bool   face_bary_coords        (const uint fid, const vec3d & P, std::vector<double> & wgts) const;
        bool   face_bary_is_vert       (const uint fid, const std::vector<double> & wgts, uint & vid, const double tol = 1e-5) const;
        bool   face_bary_is_edge       (const uint fid, const std::vector<double> & wgts, uint & eid, const double tol = 1e-5) const;
        void   face_switch_id          (const uint fid0, const uint fid1);
        uint   face_add                (const uint vid0, const uint vid1, const uint vid2);
        void   face_set                (const uint fid, const uint vid0, const uint vid1, const uint vid2);
        void   face_remove_unreferenced(const uint fid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool   elem_bary_coords (const uint fid, const vec3d & P, std::vector<double> & wgts) const;
        bool   elem_bary_is_vert(const uint fid, const std::vector<double> & wgts, uint & vid, const double tol = 1e-5) const;
        bool   elem_bary_is_edge(const uint fid, const std::vector<double> & wgts, uint & eid, const double tol = 1e-5) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "trimesh.cpp"
#endif

#endif // CINO_TRIMESH_H
