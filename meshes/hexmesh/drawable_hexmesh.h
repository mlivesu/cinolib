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
#ifndef CINO_DRAWABLE_HEXMESH_H
#define CINO_DRAWABLE_HEXMESH_H

#ifdef CINOLIB_USES_OPENGL

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/gl/draw_lines_tris.h>
#include <cinolib/meshes/hexmesh/hexmesh.h>
#include <cinolib/meshes/mesh_slicer.h>

namespace cinolib
{

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data,
         class C = Cell_std_data>
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

        void  draw(const float scene_size=1) const;
        vec3d scene_center() const { return this->bb.center(); }
        float scene_radius() const { return this->bb.diag();   }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init_drawable_stuff();
        void update_drawlist();
        void update_drawlist_in();
        void update_drawlist_out();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void slice(const float thresh, const int item, const int sign, const int mode);
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_hexmesh.cpp"
#endif

#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_DRAWABLE_HEXMESH_H
