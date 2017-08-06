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
#ifndef CINO_DRAWABLE_POLYHEDRALMESH_H
#define CINO_DRAWABLE_POLYHEDRALMESH_H

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/gl/draw_lines_tris.h>
#include <cinolib/meshes/polyhedralmesh.h>
#include <cinolib/meshes/mesh_slicer.h>

namespace cinolib
{

template<class M = Mesh_min_attributes, // default template arguments
         class V = Vert_min_attributes,
         class E = Edge_min_attributes,
         class F = Polygon_min_attributes,
         class C = Polyhedron_min_attributes>
class DrawablePolyhedralmesh : public Polyhedralmesh<M,V,E,F,C>, public DrawableObject
{

    public:

        DrawablePolyhedralmesh();

        DrawablePolyhedralmesh(const char * filename);

        DrawablePolyhedralmesh(const std::vector<vec3d>             & verts,
                               const std::vector<std::vector<uint>> & faces,
                               const std::vector<std::vector<uint>> & polys,
                               const std::vector<std::vector<bool>> & polys_face_winding);

    protected:

        RenderData drawlist_in;
        RenderData drawlist_out;        
        MeshSlicer<DrawablePolyhedralmesh<M,V,E,F,C>> slicer;

    public:

        void       draw(const float scene_size=1) const;
        vec3d      scene_center() const { return this->bb.center();       }
        float      scene_radius() const { return this->bb.diag();         }
        ObjectType object_type()  const { return DRAWABLE_POLYHEDRALMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void vert_set_color(const Color & c) { Polyhedralmesh<M,V,E,F,C>::vert_set_color(c); updateGL(); }
        void edge_set_color(const Color & c) { Polyhedralmesh<M,V,E,F,C>::edge_set_color(c); updateGL(); }
        void face_set_color(const Color & c) { Polyhedralmesh<M,V,E,F,C>::face_set_color(c); updateGL(); }
        void cell_set_color(const Color & c) { Polyhedralmesh<M,V,E,F,C>::cell_set_color(c); updateGL(); }
        void vert_set_alpha(const float   a) { Polyhedralmesh<M,V,E,F,C>::vert_set_alpha(a); updateGL(); }
        void edge_set_alpha(const float   a) { Polyhedralmesh<M,V,E,F,C>::edge_set_alpha(a); updateGL(); }
        void face_set_alpha(const float   a) { Polyhedralmesh<M,V,E,F,C>::face_set_alpha(a); updateGL(); }
        void cell_set_alpha(const float   a) { Polyhedralmesh<M,V,E,F,C>::cell_set_alpha(a); updateGL(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init_drawable_stuff();
        void updateGL();
        void updateGL_in();
        void updateGL_out();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void slice(const float thresh, const int item, const int sign, const int mode);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void show_mesh(const bool b);
        void show_mesh_flat();
        void show_mesh_smooth();
        void show_mesh_points();
        void show_face_color();
        void show_face_quality();
        void show_face_texture1D(const GLint texture);
        void show_face_wireframe(const bool b);
        void show_face_wireframe_color(const Color & c);
        void show_face_wireframe_width(const float width);
        void show_face_wireframe_transparency(const float alpha);
        void show_cell_color();
        void show_cell_quality();
        void show_cell_texture1D(const GLint texture);
        void show_cell_wireframe(const bool b);
        void show_cell_wireframe_color(const Color & c);
        void show_cell_wireframe_width(const float width);
        void show_cell_wireframe_transparency(const float alpha);
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_polyhedralmesh.cpp"
#endif

#endif // CINO_DRAWABLE_POLYHEDRALMESH_H
