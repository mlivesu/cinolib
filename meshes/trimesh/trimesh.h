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
#include <cinolib/meshes/mesh_attributes.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/meshes/abstract_surface_mesh.h>

namespace cinolib
{

template<class M = Mesh_min_attributes, // default template arguments
         class V = Vert_min_attributes,
         class E = Edge_min_attributes,
         class P = Polygon_min_attributes>
class Trimesh : public AbstractPolygonMesh<M,V,E,P>
{
    public:

        Trimesh(){}

        Trimesh(const char * filename);

        Trimesh(const std::vector<vec3d>             & coords,
                const std::vector<std::vector<uint>> & polys);

        Trimesh(const std::vector<double>            & coords,
                const std::vector<std::vector<uint>> & polys);

        Trimesh(const std::vector<vec3d> & coords,
                const std::vector<uint>  & polys);

        Trimesh(const std::vector<double> & coords,
                const std::vector<uint>   & polys);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const { return TRIMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_p_normal(const uint pid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<uint> get_ordered_boundary_vertices() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void   vert_switch_id          (const uint vid0, const uint vid1);
        void   vert_remove_unreferenced(const uint vid);
        uint   vert_add                (const vec3d & pos);
        uint   vert_opposite_to        (const uint pid, const uint vid0, const uint vid1) const;

        using  AbstractPolygonMesh<M,V,E,P>::vert_opposite_to; // avoid hiding vert_opposite_to(eid,vid)

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int    edge_opposite_to        (const uint pid, const uint vid) const;
        void   edge_switch_id          (const uint eid0, const uint eid1);
        bool   edge_collapse           (const uint eid);
        uint   edge_add                (const uint vid0, const uint vid1);
        void   edge_remove_unreferenced(const uint eid);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double poly_area               (const uint pid) const;
        bool   poly_is_cap             (const uint pid, const double angle_thresh_deg = 177.0) const;
        bool   poly_is_needle          (const uint pid, const double angle_thresh_deg = 3.0) const;
        bool   poly_bary_coords        (const uint pid, const vec3d & p, std::vector<double> & wgts) const;
        bool   poly_bary_is_vert       (const uint pid, const std::vector<double> & wgts, uint & vid, const double tol = 1e-5) const;
        bool   poly_bary_is_edge       (const uint pid, const std::vector<double> & wgts, uint & eid, const double tol = 1e-5) const;
        void   poly_switch_id          (const uint pid0, const uint pid1);
        uint   poly_add                (const uint vid0, const uint vid1, const uint vid2);
        void   poly_set                (const uint pid, const uint vid0, const uint vid1, const uint vid2);
        void   poly_remove_unreferenced(const uint pid);
        uint   poly_edge_id            (const uint pid, const uint offset) const;

        using  AbstractMesh<M,V,E,P>::poly_edge_id; // avoid hiding poly_ege_id(pid,vid0,vid1)
};

}

#ifndef  CINO_STATIC_LIB
#include "trimesh.cpp"
#endif

#endif // CINO_TRIMESH_H
