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
#ifndef DRAWABLE_TETMESH_H
#define DRAWABLE_TETMESH_H

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include "../cino_inline.h"
#include "../drawable_object.h"
#include "tetmesh.h"

namespace cinolib
{

class DrawableTetmesh : public Tetmesh, public DrawableObject
{

    public:

        enum
        {
            DRAW_MESH            = 0x00000001,
            DRAW_OUT_POINTS      = 0x00000002,
            DRAW_OUT_FLAT        = 0x00000004,
            DRAW_OUT_SMOOTH      = 0x00000008,
            DRAW_OUT_WIREFRAME   = 0x00000010,
            DRAW_OUT_FACECOLOR   = 0x00000020,
            DRAW_OUT_VERTEXCOLOR = 0x00000040,
            DRAW_OUT_TEXTURE1D   = 0x00000080,
            DRAW_OUT_EL_QUALITY  = 0x00000100,
            DRAW_IN_SLICE        = 0x00000200,
            DRAW_IN_POINTS       = 0x00000400,
            DRAW_IN_FLAT         = 0x00000800,
            DRAW_IN_SMOOTH       = 0x00001000,
            DRAW_IN_WIREFRAME    = 0x00002000,
            DRAW_IN_FACECOLOR    = 0x00004000,
            DRAW_IN_VERTEXCOLOR  = 0x00008000,
            DRAW_IN_TEXTURE1D    = 0x00010000,
            DRAW_IN_EL_QUALITY   = 0x00020000,
        };

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

        DrawableTetmesh();
        DrawableTetmesh(const char *filename);
        DrawableTetmesh(const std::vector<double> & coords,
                        const std::vector<u_int>  & tets);

        void init();

        void operator+=(const DrawableTetmesh & m);

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
        void set_t_out_color(const float r, const float g, const float b);
        void set_t_in_color(const float r, const float g, const float b);
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
        void color_wrt_tet_scalar();


    protected:

        void render_pass_out() const;
        void render_pass_in()  const;

        int    draw_mode;
        int    wireframe_out_width;
        float  wireframe_out_color[4];
        GLuint texture_out_id;

        int    wireframe_in_width;
        float  wireframe_in_color[4];
        GLuint texture_in_id;

        void update_outer_visible_mesh();
        void update_inner_slice();

        std::vector<bool> slice_mask;
        float slice_thresh[5];
        bool  slice_dir[5];

        std::vector<float> t_out_colors;
        std::vector<float> t_in_colors;

        // sub-portion of the EXTERIOR of the tetmesh to be rendered
        // (it depends on the slicing - if slicing is disabled the whole outer surface will be rendered)
        //
        std::vector<double> outer_visible_coords;
        std::vector<u_int>  outer_visible_tris;
        std::vector<double> outer_visible_v_norms;
        std::vector<float>  outer_visible_f_values;
        std::vector<float>  outer_visible_t_colors;

        // sub-portion of the INTERIOR of the tetmesh to be rendered
        //
        std::vector<double> inner_slice_coords;
        std::vector<u_int>  inner_slice_tris;
        std::vector<double> inner_slice_v_norms;
        std::vector<float>  inner_slice_f_values;
        std::vector<float>  inner_slice_t_colors;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_tetmesh.cpp"
#endif

#endif // DRAWABLE_TETMESH_H
