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
#ifndef CINO_ABSTRACT_SURFACE_MESH_H
#define CINO_ABSTRACT_SURFACE_MESH_H

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

                 AbstractPolygonMesh() : AbstractMesh<M,V,E,P>() {}
        virtual ~AbstractPolygonMesh() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void load(const char * filename);
        virtual void save(const char * filename) const;
        virtual void init();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void update_adjacency();
        virtual void update_f_normal(const uint fid) = 0;
        virtual void update_v_normal(const uint vid);
        virtual void update_f_normals();
        virtual void update_v_normals();
        virtual void update_normals();

         //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

         void operator+=(const AbstractPolygonMesh<M,V,E,P> & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual uint verts_per_elem(const uint fid) const { return this->polys.at(fid).size(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual std::vector<int>   export_per_poly_labels() const;
        virtual std::vector<Color> export_per_poly_colors() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void               normalize_area();
        virtual std::vector<uint>  get_boundary_vertices() const;
        virtual std::vector<ipair> get_boundary_edges() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual bool              vert_is_saddle        (const uint vid, const int tex_coord = U_param) const;
        virtual bool              vert_is_critical_p    (const uint vid, const int tex_coord = U_param) const;
        virtual uint              vert_opposite_to      (const uint eid, const uint vid) const;
        virtual double            vert_area             (const uint vid) const;
        virtual double            vert_mass             (const uint vid) const;
        virtual bool              verts_are_ordered_CCW (const uint fid, const uint curr, const uint prev) const;
        virtual bool              vert_is_boundary      (const uint vid) const;
        virtual std::vector<uint> vert_boundary_edges   (const uint vid) const;
        virtual std::vector<uint> vert_ordered_vert_ring(const uint vid) const;
        virtual std::vector<uint> vert_ordered_poly_ring(const uint vid) const;
        virtual std::vector<uint> vert_ordered_edge_ring(const uint vid) const;
        virtual std::vector<uint> vert_ordered_edge_link(const uint vid) const;
        virtual void              vert_ordered_one_ring (const uint          vid,
                                                         std::vector<uint> & v_ring,        // sorted list of adjacent vertices
                                                         std::vector<uint> & f_ring,        // sorted list of adjacent triangles
                                                         std::vector<uint> & e_ring,        // sorted list of edges incident to vid
                                                         std::vector<uint> & e_link) const; // sorted list of edges opposite to vid

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual bool  edge_is_manifold             (const uint eid) const;
        virtual bool  edge_is_boundary             (const uint eid) const;
        virtual bool  edges_share_face             (const uint eid1, const uint eid2) const;
        virtual ipair edge_shared                  (const uint fid0, const uint fid1) const;
        virtual void  edge_mark_labeling_boundaries();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void   poly_show_all          ();
        virtual double poly_angle_at_vert     (const uint fid, const uint vid, const int unit = RAD) const;
        virtual double poly_area              (const uint fid) const = 0;
        virtual double poly_mass              (const uint fid) const;
        virtual int    poly_shared            (const uint eid0, const uint eid1) const;
        virtual int    poly_adjacent_along    (const uint fid, const uint vid0, const uint vid1) const;
        virtual void   poly_flip_winding_order(const uint fid);
        virtual bool   poly_is_boundary       (const uint fid) const;
        virtual int    poly_opposite_to       (const uint eid, const uint fid) const;
        virtual void   poly_set_color         (const Color & c);
        virtual void   poly_set_alpha         (const float alpha);
        virtual void   poly_color_wrt_label   ();
};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_surface_mesh.cpp"
#endif

#endif //CINO_ABSTRACT_SURFACE_MESH_H
