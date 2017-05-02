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
#include <cinolib/gl/draw_tris_quads.h>
#include <cinolib/meshes/trimesh/trimesh.h>


namespace cinolib
{

class DrawableTrimesh : public Trimesh , public DrawableObject
{
    public:

        enum
        {
            TEXTURE_ISOLINES                = 0,
            TEXTURE_QUALITY_RAMP            = 1,
            TEXTURE_QUALITY_RAMP_W_ISOLINES = 2
        };

        DrawableTrimesh();
        DrawableTrimesh(const Trimesh & m);
        DrawableTrimesh(const char *filename);
        DrawableTrimesh(const std::vector<double> & coords,
                        const std::vector<u_int>  & tris);

        void init();
        void clear();

        // Implementation of the
        // DrawableObject interface
        //
        void  draw(const float scene_size=1) const;
        vec3d scene_center() const;
        float scene_radius() const;

        // rendering options
        //
        void set_draw_mesh(bool b);
        void set_wireframe(bool b);
        void set_border(bool b);
        void set_flat_shading();
        void set_smooth_shading();
        void set_points_shading();
        void set_wireframe_color(float r, float g, float b);
        void set_wireframe_width(float width);
        void set_wireframe_transparency(float alpha);
        void set_border_color(float r, float g, float b);
        void set_border_width(float width);
        void set_border_transparency(float alpha);
        void set_enable_vertex_color();
        void set_enable_triangle_color();
        void set_enable_texture1D(int texture);
        void set_v_color(float r, float g, float b);
        void set_t_color(float r, float g, float b);

        const float * vertex_color  (const int vid) const;
        const float * triangle_color(const int tid) const;

        void set_vertex_color(const int vid, const float * color);
        void set_triangle_color(const int tid, const float * color);

        void color_wrt_triangle_scalar();
        void unglue_face_colors();
        void unglue_boundary_edges();

        std::vector<int> get_border_vertices() const;

        int add_vertex(const vec3d & v, const float scalar = 0.0);
        int add_triangle(const int vid0, const int vid1, const int vid2, const int scalar = 0);

        void operator+=(const DrawableTrimesh & m);

    protected:

        void render_pass() const;
        void update_border();

        int    draw_mode;
        int    wireframe_width;
        int    border_width;
        float  wireframe_color[4];
        float  border_color[4];
        GLuint texture_id;

        std::vector<int>    border_vertices;
        std::vector<double> border_coords;
        std::vector<u_int>  border_segs;
        std::vector<float>  v_colors;
        std::vector<float>  t_colors;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_trimesh.cpp"
#endif

#endif // CINO_DRAWABLE_TRIMESH_H
