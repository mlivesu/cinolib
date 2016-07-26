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
#ifndef DRAWABLE_QUADMESH_H
#define DRAWABLE_QUADMESH_H

#include <vector>
#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include "../../cinolib.h"
#include "../../drawable_object.h"
#include "../../gl/draw_tris_quads.h"
#include "quadmesh.h"

namespace cinolib
{

class DrawableQuadmesh : public Quadmesh , public DrawableObject
{
    public:

        enum
        {
            TEXTURE_ISOLINES                = 0,
            TEXTURE_QUALITY_RAMP            = 1,
            TEXTURE_QUALITY_RAMP_W_ISOLINES = 2
        };

        DrawableQuadmesh();
        DrawableQuadmesh(const char *filename);
        DrawableQuadmesh(const std::vector<double> & coords,
                         const std::vector<u_int>  & quads);

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
        void set_enable_quad_color();
        void set_enable_texture1D(int texture);
        void set_v_color(float r, float g, float b);
        void set_q_color(float r, float g, float b);

        const float * vertex_color  (const int vid) const;
        const float * quad_color(const int qid) const;

        void set_vertex_color(const int vid, const float * color);
        void set_quad_color(const int qid, const float * color);

    protected:

        void render_pass() const;

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
        std::vector<float>  q_colors;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_quadmesh.cpp"
#endif

#endif // DRAWABLE_QUADMESH_H
