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
#ifndef CINO_ABSTRACT_POLYGON_MESH_H
#define CINO_ABSTRACT_POLYGON_MESH_H

#include <cinolib/meshes/abstract_mesh.h>

namespace cinolib
{

template<class M,
         class V,
         class E,
         class P>
class AbstractPolygonMesh : public AbstractMesh<M,V,E,P>
{
    public:

        explicit AbstractPolygonMesh() : AbstractMesh<M,V,E,P>() {}
        virtual ~AbstractPolygonMesh() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void load(const char * filename);
        virtual void save(const char * filename) const;
        virtual void init();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void update_adjacency();
        virtual void update_p_normal(const uint fid) = 0;
        virtual void update_v_normal(const uint vid);
        virtual void update_p_normals();
        virtual void update_v_normals();
        virtual void update_normals();

         //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint verts_per_poly(const uint pid) const { return this->polys.at(pid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual const std::vector<uint> & adj_p2v(const uint pid) const { return this->polys.at(pid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void operator+=(const AbstractPolygonMesh<M,V,E,P> & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual std::vector<int>   export_per_poly_labels() const;
        virtual std::vector<Color> export_per_poly_colors() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual std::vector<uint>  get_boundary_vertices() const;
        virtual std::vector<ipair> get_boundary_edges() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual double mesh_volume() const;
        virtual double mesh_area()   const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void normalize_area();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual bool              vert_is_saddle          (const uint vid, const int tex_coord = U_param) const;
        virtual bool              vert_is_critical_p      (const uint vid, const int tex_coord = U_param) const;
        virtual double            vert_area               (const uint vid) const;
        virtual double            vert_mass               (const uint vid) const;
        virtual bool              vert_is_boundary        (const uint vid) const;
        virtual void              vert_switch_id          (const uint vid0, const uint vid1);
        virtual void              vert_remove             (const uint vid);
        virtual void              vert_remove_unreferenced(const uint vid);
        virtual uint              vert_add                (const vec3d & pos);
        virtual std::vector<uint> vert_boundary_edges     (const uint vid) const;
        virtual std::vector<uint> vert_ordered_vert_ring  (const uint vid) const;
        virtual std::vector<uint> vert_ordered_poly_ring  (const uint vid) const;
        virtual std::vector<uint> vert_ordered_edge_ring  (const uint vid) const;
        virtual std::vector<uint> vert_ordered_edge_link  (const uint vid) const;
        virtual void              vert_ordered_one_ring   (const uint          vid,
                                                           std::vector<uint> & v_ring,        // sorted list of adjacent vertices
                                                           std::vector<uint> & f_ring,        // sorted list of adjacent triangles
                                                           std::vector<uint> & e_ring,        // sorted list of edges incident to vid
                                                           std::vector<uint> & e_link) const; // sorted list of edges opposite to vid

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual bool  edge_is_manifold             (const uint eid) const;
        virtual bool  edge_is_boundary             (const uint eid) const;
        virtual bool  edges_share_poly             (const uint eid1, const uint eid2) const;
        virtual uint  edge_shared                  (const uint pid0, const uint pid1) const;
        virtual void  edge_switch_id               (const uint eid0, const uint eid1);
        virtual uint  edge_add                     (const uint vid0, const uint vid1);
        virtual void  edge_remove                  (const uint eid);
        virtual void  edge_remove_unreferenced     (const uint eid);
        virtual void  edge_mark_labeling_boundaries();
        virtual void  edge_mark_boundaries();
        virtual void  edge_unmark_all();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual       uint                 poly_vert_offset        (const uint pid, const uint vid) const;
        virtual       double               poly_angle_at_vert      (const uint pid, const uint vid, const int unit = RAD) const;
        virtual       double               poly_area               (const uint pid) const;
        virtual       double               poly_mass               (const uint pid) const;
        virtual       int                  poly_shared             (const uint eid0, const uint eid1) const;
        virtual       bool                 polys_are_adjacent      (const uint pid0, const uint pid1) const;
        virtual       std::vector<uint>    polys_adjacent_along    (const uint pid, const uint vid0, const uint vid1) const;
        virtual       std::vector<uint>    polys_adjacent_along    (const uint pid, const uint eid) const;
        virtual       void                 poly_flip_winding_order (const uint pid);
        virtual       void                 poly_switch_id          (const uint pid0, const uint pid1);
        virtual       bool                 poly_is_boundary        (const uint pid) const;
        virtual       uint                 poly_add                (const std::vector<uint> & p);
        virtual       void                 poly_remove_unreferenced(const uint pid);
        virtual       void                 poly_remove             (const uint pid);
        virtual       void                 polys_remove            (const std::vector<uint> & pids);
        virtual       int                  poly_opposite_to        (const uint eid, const uint pid) const;
        virtual       bool                 poly_verts_are_CCW      (const uint pid, const uint curr, const uint prev) const;
        virtual       std::vector<vec3d>   poly_vlist              (const uint pid) const;
        virtual const std::vector<uint>  & poly_tessellation       (const uint pid) const = 0;
};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_polygonmesh.cpp"
#endif

#endif //CINO_ABSTRACT_POLYGON_MESH_H
