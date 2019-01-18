/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_ABSTRACT_DRAWABLE_POLYGON_MESH_H
#define CINO_ABSTRACT_DRAWABLE_POLYGON_MESH_H

#ifdef CINOLIB_USES_OPENGL

#include <cinolib/drawable_object.h>
#include <cinolib/meshes/mesh_slicer.h>
#include <cinolib/gl/draw_lines_tris.h>

namespace cinolib
{

template<class Mesh>
class AbstractDrawablePolygonMesh : public virtual Mesh, public DrawableObject
{
    protected:

        MeshSlicer<Mesh> slicer;
        RenderData       drawlist;
        RenderData       drawlist_marked; // rendering info about marked edges (can be extended to handle marked verts/faces too)
        Color            marked_edge_color;
        float            AO_alpha = 1.0;

    public:

        explicit AbstractDrawablePolygonMesh() : Mesh() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void       draw(const float scene_size=1) const;
        vec3d      scene_center() const { return this->bb.center();     }
        float      scene_radius() const { return this->bb.diag() * 0.5; }
        ObjectType object_type()  const = 0;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void vert_set_color(const Color & c) { Mesh::vert_set_color(c); updateGL(); }
        void edge_set_color(const Color & c) { Mesh::edge_set_color(c); updateGL(); }
        void poly_set_color(const Color & c) { Mesh::poly_set_color(c); updateGL(); }
        void vert_set_alpha(const float   a) { Mesh::vert_set_alpha(a); updateGL(); }
        void edge_set_alpha(const float   a) { Mesh::edge_set_alpha(a); updateGL(); }
        void poly_set_alpha(const float   a) { Mesh::poly_set_alpha(a); updateGL(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init_drawable_stuff();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void updateGL();        // regenerates rendering data for both mesh and marked elements
        void updateGL_mesh();   // regenerates rendering data for mesh elements
        void updateGL_marked(); // regenerates rendering data for marked mesh elements

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void slice(const SlicerState & s);
        void slicer_reset();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void show_mesh(const bool b);
        void show_AO_alpha(const float alpha);
        void show_mesh_flat();
        void show_mesh_smooth();
        void show_mesh_points();
        void show_vert_color();
        void show_poly_color();
        void show_texture1D(const int tex_type);
        void show_texture2D(const int tex_type, const double tex_unit_scalar, const char *bitmap = NULL);
        void show_wireframe(const bool b);
        void show_wireframe_color(const Color & c);
        void show_wireframe_width(const float width);
        void show_wireframe_transparency(const float alpha);
        void show_marked_edge(const bool b);
        void show_marked_edge_color(const Color & c);
        void show_marked_edge_width(const float width);
        void show_marked_edge_transparency(const float alpha);
};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_drawable_polygonmesh.cpp"
#endif

#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_ABSTRACT_DRAWABLE_POLYGON_MESH_H
