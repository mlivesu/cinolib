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
#ifndef DRAWABLE_HEXMESH_H
#define DRAWABLE_HEXMESH_H

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include "../cinolib.h"
#include "../drawable_object.h"
#include "../gl/draw_tris_quads.h"
#include "hexmesh.h"

namespace cinolib
{

class DrawableHexmesh : public Hexmesh, public DrawableObject
{

    public:

        enum
        {
            TEXTURE_ISOLINES                = 0,
            TEXTURE_QUALITY_RAMP            = 1,
            TEXTURE_QUALITY_RAMP_W_ISOLINES = 2
        };

        // X : slice w.r.t. tet centroid X coord
        // Y : slice w.r.t. tet centroid Y coord
        // Z : slice w.r.t. tet centroid Z coord
        // Q : slice w.r.t. tet quality (MSJ)
        // L : slice w.r.t. tet label
        //
        enum { X, Y, Z, Q, L };

        DrawableHexmesh();
        DrawableHexmesh(const char *filename);
        DrawableHexmesh(const std::vector<double> & coords,
                        const std::vector<u_int>  & hexa);

        void init();

        void operator+=(const DrawableHexmesh & m);

        // Implementation of the
        // DrawableObject interface
        //
        void  draw()         const;
        vec3d scene_center() const;
        float scene_radius() const;

        void  draw_out() const;
        void  draw_in()  const;

        void set_draw_mesh(bool b);
        void set_flat_shading();
        void set_smooth_shading();
        void set_points_shading();
        void set_h_out_color(const float r, const float g, const float b);
        void set_h_in_color(const float r, const float g, const float b);
        void set_enable_out_face_color();
        void set_enable_out_quality_color();
        void set_enable_out_texture1D(int texture);
        void set_out_wireframe(bool b);
        void set_out_wireframe_color(float r, float g, float b);
        void set_out_wireframe_width(float width);
        void set_out_wireframe_transparency(float alpha);
        void set_in_wireframe(bool b);
        void set_in_wireframe_color(float r, float g, float b);
        void set_in_wireframe_width(float width);
        void set_in_wireframe_transparency(float alpha);
        void set_draw_slice(bool b);
        void set_slice_parameters(const float thresh, const int item, const bool dir, const bool mode);
        void update_slice(const bool mode = true);
        void set_enable_in_quality_color();
        void set_enable_in_face_color();
        void set_enable_in_texture1D(int texture);
        void color_wrt_hex_scalar();


    protected:

        void render_pass_out() const;
        void render_pass_in()  const;

        int    draw_mode_in;
        int    draw_mode_out;
        int    wireframe_out_width;
        float  wireframe_out_color[4];
        GLuint texture_out_id;

        int    wireframe_in_width;
        float  wireframe_in_color[4];
        GLuint texture_in_id;

        void color_tets_wrt_quality(const bool in, const bool out);
        void update_outer_visible_mesh();
        void update_inner_slice();

        std::vector<bool> slice_mask;
        float slice_thresh[5];
        bool  slice_dir[5];

        std::vector<float> h_out_colors;
        std::vector<float> h_in_colors;

        // sub-portion of the EXTERIOR of the hexmesh to be rendered
        // (it depends on the slicing - if slicing is disabled the whole outer surface will be rendered)
        //
        std::vector<double> outer_visible_coords;
        std::vector<u_int>  outer_visible_quads;
        std::vector<double> outer_visible_v_norms;
        std::vector<float>  outer_visible_f_values;
        std::vector<float>  outer_visible_h_colors;

        // sub-portion of the INTERIOR of the hexmesh to be rendered
        //
        std::vector<double> inner_slice_coords;
        std::vector<u_int>  inner_slice_quads;
        std::vector<double> inner_slice_v_norms;
        std::vector<float>  inner_slice_f_values;
        std::vector<float>  inner_slice_h_colors;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_hexmesh.cpp"
#endif

#endif // DRAWABLE_HEXMESH_H
