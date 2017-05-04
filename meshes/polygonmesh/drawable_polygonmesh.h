/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
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
#ifndef CINO_DRAWABLE_POLYGONMESH_H
#define CINO_DRAWABLE_POLYGONMESH_H

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/gl/draw_tris_quads.h>
#include <cinolib/meshes/polygonmesh/polygonmesh.h>
#include <cinolib/meshes/mesh_slicer.h>

namespace cinolib
{

template<class V_data = Vert_std_data, // default template arguments
         class E_data = Edge_std_data,
         class F_data = Face_std_data>
class DrawablePolygonmesh : public Polygonmesh<V_data,E_data,F_data>, public DrawableObject
{
    public:

        DrawablePolygonmesh();

        DrawablePolygonmesh(const std::vector<double>            & coords,
                            const std::vector<std::vector<uint>> & faces);

    protected:

        MeshSlicer<Polygonmesh<V_data,E_data,F_data>> slicer;

        int    draw_mode;
        int    wireframe_width;
        float  wireframe_color[4];
        int    border_width;
        float  border_color[4];
        GLuint texture_id;

        // to be substituted with a new version of RenderFaceData
        // (which will no longer take pointers to vectors, but rather concrete vectors)
        // this will involce changes in the other data structures too.
        // it has to be done at the very end of the refactoring
        std::vector<double> drawlist_coords;
        std::vector<uint>   drawlist_faces;
        std::vector<double> drawlist_v_norms;
        std::vector<float>  drawlist_v_colors;
        std::vector<float>  drawlist_f_colors;
        std::vector<double> drawlist_border_coords;
        std::vector<uint>   drawlist_border_segs;

    public:

        void  draw(const float scene_size=1) const;
        vec3d scene_center() const { return this->bb.center(); }
        float scene_radius() const { return this->bb.diag();   }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init_drawable_stuff();
        void update_drawlist();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void slice(const float thresh, const int item, const int sign, const int mode);

};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_polygonmesh.cpp"
#endif

#endif // CINO_DRAWABLE_POLYGONMESH_H
