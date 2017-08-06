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
#ifndef CINO_DRAWABLE_HEXMESH_H
#define CINO_DRAWABLE_HEXMESH_H

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/gl/draw_lines_tris.h>
#include <cinolib/meshes/hexmesh.h>
#include <cinolib/meshes/mesh_slicer.h>

namespace cinolib
{

template<class M = Mesh_min_attributes, // default template arguments
         class V = Vert_min_attributes,
         class E = Edge_min_attributes,
         class F = Polygon_min_attributes,
         class C = Polyhedron_min_attributes>
class DrawableHexmesh : public Hexmesh<M,V,E,F,C>, public DrawableObject
{

    public:

        DrawableHexmesh();

        DrawableHexmesh(const char * filename);

        DrawableHexmesh(const std::vector<vec3d> & verts,
                        const std::vector<uint>  & cells);

        DrawableHexmesh(const std::vector<double> & coords,
                        const std::vector<uint>   & cells);

    protected:

        RenderData drawlist_in;
        RenderData drawlist_out;        
        MeshSlicer<DrawableHexmesh<M,V,E,F,C>> slicer;

    public:

        void       draw(const float scene_size=1) const;
        vec3d      scene_center() const { return this->bb.center(); }
        float      scene_radius() const { return this->bb.diag();   }
        ObjectType object_type()  const { return DRAWABLE_HEXMESH;  }

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
#include "drawable_hexmesh.cpp"
#endif

#endif // CINO_DRAWABLE_HEXMESH_H
