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
#ifndef CINO_DRAWABLE_SKEL_H
#define CINO_DRAWABLE_SKEL_H

#include <vector>
#include <cinolib/drawable_object.h>
#include <cinolib/meshes/skel.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/stl_container_utilities.h>

namespace cinolib
{

class DrawableSkel : public Skel, public DrawableObject
{
    public:

        enum
        {
            DRAW_SKEL            = 0x00000001,
            DRAW_POINTS          = 0x00000010,
            DRAW_FEATURE_POINTS  = 0x00000100,
            DRAW_BONES           = 0x00001000,
            DRAW_MAX_SPHERES     = 0x00010000,
            DRAW_STD_COLOR       = 0x00100000,
            DRAW_BONE_COLOR      = 0x01000000,
            DRAW_SELECTION_COLOR = 0x10000000
        };

        ~DrawableSkel(){}

        explicit DrawableSkel();
        explicit DrawableSkel(const Skel & s);
        explicit DrawableSkel(const char * filename);
        explicit DrawableSkel(const std::vector<double> & coords, const std::vector<int> & segs);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void       draw(const float scene_size=1) const;
        vec3d      scene_center() const { return bb.center();       }
        float      scene_radius() const { return bb.diag() * 0.5;   }
        ObjectType object_type()  const { return DRAWABLE_SKELETON; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_draw_skel(bool b);
        void set_draw_bones(bool b);
        void set_draw_bone_points(bool b);
        void set_draw_feature_points(bool b);
        void set_draw_max_spheres(bool b);
        void set_enable_std_color();
        void set_enable_bone_color();
        void set_enable_selection_color();
        void set_std_leaf_color(float r, float g, float b);
        void set_std_bone_color(float r, float g, float b);
        void set_std_joint_color(float r, float g, float b);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        inline void set_std_leaf_color(float *rgb) { set_std_leaf_color(rgb[0], rgb[1], rgb[2]); }
        inline void set_std_bone_color(float *rgb) { set_std_bone_color(rgb[0], rgb[1], rgb[2]); }
        inline void set_std_joint_color(float *rgb) { set_std_joint_color(rgb[0], rgb[1], rgb[2]); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_bone_colors();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        inline const float * vertex_color (int vid) const { return &(v_colors[vid*3]); }
        inline const float * segment_color(int sid) const { return &(s_colors[sid*3]); }
        inline void bone_color(int bid, float * rgb) const
        {
            int first_segment = segment_bone(bid).front();
            const float * c = segment_color(first_segment);
            rgb[0] = c[0];
            rgb[1] = c[1];
            rgb[2] = c[2];
        }

        inline void set_vertex_color(int vid, float * color)
        {
            int vid_ptr = vid * 3;
            CHECK_BOUNDS(v_colors, vid_ptr+2);
            v_colors[vid_ptr + 0] = color[0];
            v_colors[vid_ptr + 1] = color[1];
            v_colors[vid_ptr + 2] = color[2];
        }

        inline void set_segment_color(int sid, float * color)
        {
            int sid_ptr = sid * 3;
            CHECK_BOUNDS(s_colors, sid_ptr+2);
            s_colors[sid_ptr + 0] = color[0];
            s_colors[sid_ptr + 1] = color[1];
            s_colors[sid_ptr + 2] = color[2];
        }

        inline void set_bone_thickness(float scalar)
        {
            bone_thickness_modifier = scalar;
        }

        inline void set_sphere_radius(float scalar)
        {
            sphere_radius_modifier = scalar;
        }

    private:

        inline float BONE_SCALAR() const { return 0.004; }
        inline float BALL_SCALAR() const { return 0.008; }

        int   draw_mode;
        float bone_thickness_modifier; // useful for rendering
        float sphere_radius_modifier;  // useful for rendering

        float std_bone_color[3];
        float std_leaf_color[3];
        float std_joint_color[3];

        std::vector<float> s_colors;
        std::vector<float> v_colors;        
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_skel.cpp"
#endif

#endif // CINO_DRAWABLE_SKEL_H
