/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#ifndef CINO_DRAWABLE_TRIMESH_H
#define CINO_DRAWABLE_TRIMESH_H

#include <vector>
#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/gl/draw_lines_tris.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/meshes/mesh_slicer.h>

namespace cinolib
{

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data>
class DrawableTrimesh : public Trimesh<M,V,E,F>, public DrawableObject
{
    public:

        DrawableTrimesh();

        DrawableTrimesh(const char * filename);

        DrawableTrimesh(const std::vector<vec3d> & verts,
                        const std::vector<uint>  & faces);

        DrawableTrimesh(const std::vector<double> & coords,
                        const std::vector<uint>   & faces);

    protected:

        RenderData drawlist;
        MeshSlicer<Trimesh<M,V,E,F>> slicer;

    public:

        void  draw(const float scene_size=1) const;
        vec3d scene_center() const { return this->bb.center(); }
        float scene_radius() const { return this->bb.diag();   }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init_drawable_stuff();
        void updateGL();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void slice(const float thresh, const int item, const int sign, const int mode);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void show_mesh(const bool b);
        void show_mesh_flat();
        void show_mesh_smooth();
        void show_mesh_points();
        void show_vert_color();
        void show_face_color();
        void show_face_texture1D(const GLint texture);
        void show_face_wireframe(const bool b);
        void show_face_wireframe_color(const Color & c);
        void show_face_wireframe_width(const float width);
        void show_face_wireframe_transparency(const float alpha);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        /* (Re)implementation of virtual methods (mainly to call updateGL())*/
        void operator+=              (const DrawableTrimesh & m);
        void vert_set_color          (const Color & c);
        void vert_set_alpha          (const float a);
        void vert_remove_unreferenced(const uint vid);
        uint vert_add                (const vec3d & pos, const V & data);
        void edge_set_color          (const Color & c);
        void edge_set_alpha          (const float a);
        bool edge_collapse           (const uint eid);
        void edge_remove_unreferenced(const uint eid);
        void face_set_color          (const Color & c);
        void face_set_alpha          (const float a);
        uint face_add                (const uint vid0, const uint vid1, const uint vid2, const F & data);
        void face_remove_unreferenced(const uint fid);
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_trimesh.cpp"
#endif

#endif // CINO_DRAWABLE_TRIMESH_H
