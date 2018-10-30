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

#ifdef CINOLIB_USES_OPENGL

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include <cinolib/meshes/drawable_skel.h>
#include <cinolib/cino_inline.h>
#include <cinolib/color.h>
#include <cinolib/gl/draw_cylinder.h>
#include <cinolib/gl/draw_sphere.h>

#include <cstdlib>

namespace cinolib
{

CINO_INLINE
DrawableSkel::DrawableSkel() : Skel()
{
    init();
}

CINO_INLINE
DrawableSkel::DrawableSkel(const Skel & s) : Skel(s.vector_coords(), s.vector_segments())
{
    init();
}

CINO_INLINE
DrawableSkel::DrawableSkel(const char *filename) : Skel(filename)
{
    init();
}

CINO_INLINE
DrawableSkel::DrawableSkel(const std::vector<double> & coords, const std::vector<int> &segs) : Skel(coords, segs)
{
    init();
}

CINO_INLINE
void DrawableSkel::init()
{
    draw_mode = DRAW_SKEL | DRAW_BONES | DRAW_STD_COLOR | DRAW_FEATURE_POINTS;

    bone_thickness_modifier = 1.0;
    sphere_radius_modifier  = 1.0;

    set_std_bone_color (1.0, 1.0, 1.0);
    set_std_leaf_color (0.0, 1.0, 0.0);
    set_std_joint_color(1.0, 0.0, 0.0);

    update_bone_colors();
}

CINO_INLINE
void DrawableSkel::draw(const float scene_size) const
{
    float bone_thickness = scene_size * BONE_SCALAR() * bone_thickness_modifier;
    float sphere_radius  = scene_size * BALL_SCALAR() * sphere_radius_modifier;

    if (draw_mode & DRAW_SKEL)
    {
        if (draw_mode & DRAW_MAX_SPHERES)
        {
            for(int vid=0; vid<num_vertices(); ++vid)
            {
                vec3d v = vertex(vid);
                glDisable(GL_DEPTH_TEST);
                sphere<vec3d>(v, max_sphere_radius(vid), std_bone_color);
                glEnable(GL_DEPTH_TEST);
            }
        }

        if (draw_mode & DRAW_BONES)
        {
            for(int bid=0; bid<num_bones(); ++bid)
            {
                std::vector<int> v_bone = vertex_bone(bid);

                for(int i=1; i<(int)v_bone.size()-1; ++i)
                {
                    int vid = v_bone[i];

                    vec3d v = vertex(vid);
                    glDisable(GL_DEPTH_TEST);
                    if (draw_mode & DRAW_STD_COLOR)
                    {
                        sphere<vec3d>(v, bone_thickness, std_bone_color);
                    }
                    else if (draw_mode & DRAW_BONE_COLOR)
                    {
                        sphere<vec3d>(v, bone_thickness, vertex_color(vid));
                    }
                    glEnable(GL_DEPTH_TEST);
                }

                std::vector<int> s_bone = segment_bone(bid);

                for(int i=0; i<(int)s_bone.size(); ++i)
                {
                    int sid = s_bone[i];
                    vec3d v0 = segment_vertex(sid, 0);
                    vec3d v1 = segment_vertex(sid, 1);

                    glDisable(GL_DEPTH_TEST);
                    if (draw_mode & DRAW_STD_COLOR)
                    {
                        cylinder<vec3d>(v0, v1, bone_thickness, bone_thickness, std_bone_color);
                    }
                    else if (draw_mode & DRAW_BONE_COLOR)
                    {
                        cylinder<vec3d>(v0, v1, bone_thickness, bone_thickness, segment_color(sid));
                    }
                    glEnable(GL_DEPTH_TEST);
                }
            }
        }

        if (draw_mode & DRAW_POINTS)
        {
            for(int vid=0; vid<num_vertices(); ++vid)
            {
                vec3d v = vertex(vid);
                glDisable(GL_DEPTH_TEST);
                if (draw_mode & DRAW_STD_COLOR)
                {
                    sphere<vec3d>(v, sphere_radius, std_bone_color);
                }
                else if (draw_mode & DRAW_BONE_COLOR)
                {
                    sphere<vec3d>(v, sphere_radius, vertex_color(vid));
                }
                glEnable(GL_DEPTH_TEST);
            }
        }

        if (draw_mode & DRAW_FEATURE_POINTS)
        {
            for(int vid=0; vid<num_vertices(); ++vid)
            {
                if (vertex_is_bone(vid)) continue;
                vec3d v = vertex(vid);
                glDisable(GL_DEPTH_TEST);
                sphere<vec3d>(v, sphere_radius, (vertex_is_leaf(vid) ? std_leaf_color : std_joint_color));
                glEnable(GL_DEPTH_TEST);
            }
        }
    }
}


CINO_INLINE
void DrawableSkel::set_draw_skel(bool b)
{
    if (b) draw_mode |=  DRAW_SKEL;
    else   draw_mode &= ~DRAW_SKEL;
}

CINO_INLINE
void DrawableSkel::set_draw_bones(bool b)
{
    if (b) draw_mode |=  DRAW_BONES;
    else   draw_mode &= ~DRAW_BONES;
}

CINO_INLINE
void DrawableSkel::set_draw_bone_points(bool b)
{
    if (b) draw_mode |=  DRAW_POINTS;
    else   draw_mode &= ~DRAW_POINTS;
}

CINO_INLINE
void DrawableSkel::set_draw_feature_points(bool b)
{
    if (b) draw_mode |=  DRAW_FEATURE_POINTS;
    else   draw_mode &= ~DRAW_FEATURE_POINTS;
}

CINO_INLINE
void DrawableSkel::set_draw_max_spheres(bool b)
{
    if (b) draw_mode |=  DRAW_MAX_SPHERES;
    else   draw_mode &= ~DRAW_MAX_SPHERES;
}

CINO_INLINE
void DrawableSkel::set_enable_std_color()
{
    draw_mode |=  DRAW_STD_COLOR;
    draw_mode &= ~DRAW_BONE_COLOR;
}

CINO_INLINE
void DrawableSkel::set_enable_bone_color()
{
    draw_mode |=  DRAW_BONE_COLOR;
    draw_mode &= ~DRAW_STD_COLOR;
}

CINO_INLINE
void DrawableSkel::set_enable_selection_color()
{
    draw_mode |= DRAW_SELECTION_COLOR;
}

CINO_INLINE
void DrawableSkel::set_std_leaf_color(float r, float g, float b)
{
    std_leaf_color[0] = r;
    std_leaf_color[1] = g;
    std_leaf_color[2] = b;
}

CINO_INLINE
void DrawableSkel::set_std_bone_color(float r, float g, float b)
{
    std_bone_color[0] = r;
    std_bone_color[1] = g;
    std_bone_color[2] = b;
}

CINO_INLINE
void DrawableSkel::set_std_joint_color(float r, float g, float b)
{
    std_joint_color[0] = r;
    std_joint_color[1] = g;
    std_joint_color[2] = b;
}

CINO_INLINE
void DrawableSkel::update_bone_colors()
{
    s_colors.resize(num_segments()*3);
    for(int sid=0; sid<num_segments(); ++sid)
    {
        Color c = Color::scatter(num_bones(), segment_bone_id(sid));
        set_segment_color(sid, c.rgba);
    }

    v_colors.resize(num_vertices()*3);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        if (vertex_is_feature(vid)) continue;

        Color c = Color::scatter(num_bones(), segment_bone_id(vid));
        set_vertex_color(vid, c.rgba);
    }
}

}

#endif // #ifdef CINOLIB_USES_OPENGL
