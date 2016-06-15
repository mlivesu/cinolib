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
#ifndef DRAWABLE_TRIMESH_H
#define DRAWABLE_TRIMESH_H

#include <vector>
#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include "../cino_inline.h"
#include "../drawable_object.h"
#include "trimesh.h"

namespace cinolib
{

class DrawableTrimesh : public Trimesh , public DrawableObject
{
    public:

        enum
        {
            DRAW_MESH        = 0x00000001,
            DRAW_POINTS      = 0x00000002,
            DRAW_FLAT        = 0x00000004,
            DRAW_SMOOTH      = 0x00000008,
            DRAW_WIREFRAME   = 0x00000010,
            DRAW_FACECOLOR   = 0x00000020,
            DRAW_VERTEXCOLOR = 0x00000040,
            DRAW_BORDERS     = 0x00000080,
            DRAW_TEXTURE1D   = 0x00000100
        };

        enum
        {
            TEXTURE_ISOLINES                = 0,
            TEXTURE_QUALITY_RAMP            = 1,
            TEXTURE_QUALITY_RAMP_W_ISOLINES = 2
        };

        DrawableTrimesh();
        DrawableTrimesh(const char *filename);
        DrawableTrimesh(const std::vector<double> & coords,
                        const std::vector<u_int>  & tris);

        void init();
        void clear();

        // Implementation of the
        // DrawableObject interface
        //
        void  draw()         const;
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

        inline const float * vertex_color  (const int vid) const { return &(v_colors[vid*3]); }
        inline const float * triangle_color(const int tid) const { return &(t_colors[tid*3]); }

        inline void set_vertex_color(const int vid, const float * color)
        {
            int vid_ptr = vid * 3;
            v_colors[vid_ptr + 0] = color[0];
            v_colors[vid_ptr + 1] = color[1];
            v_colors[vid_ptr + 2] = color[2];
        }

        inline void set_triangle_color(const int tid, const float * color)
        {
            int tid_ptr = tid * 3;
            t_colors[tid_ptr + 0] = color[0];
            t_colors[tid_ptr + 1] = color[1];
            t_colors[tid_ptr + 2] = color[2];
        }

        void color_wrt_triangle_scalar();
        void unglue_face_colors();
        void unglue_boundary_edges();

        std::vector<int> get_border_vertices() const;

        // re-implementation of topology editing
        // methods declared virtual in Trimesh
        //
        int add_vertex(const vec3d & v, const float scalar = 0.0);
        int add_triangle(const int vid0, const int vid1, const int vid2, const int scalar = 0);

        void operator+=(const DrawableTrimesh & m);

    protected:

        void render_pass() const;

        int    draw_mode;
        int    wireframe_width;
        int    border_width;
        float  wireframe_color[4];
        float  border_color[4];
        GLuint texture_id;

        std::vector<int>   borders;
        std::vector<float> v_colors;
        std::vector<float> t_colors;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_trimesh.cpp"
#endif

#endif // DRAWABLE_TRIMESH_H
