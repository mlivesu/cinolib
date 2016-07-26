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
#include "drawable_quadmesh.h"
#include "../../textures/quality_ramp_texture.h"
#include "../../textures/quality_ramp_texture_plus_isolines.h"
#include "../../textures/isolines_texture.h"
#include "../../colors.h"

namespace cinolib
{

CINO_INLINE
DrawableQuadmesh::DrawableQuadmesh() : Quadmesh()
{
    init();
}

CINO_INLINE
DrawableQuadmesh::DrawableQuadmesh(const char *filename) : Quadmesh(filename)
{
    init();
}

CINO_INLINE
DrawableQuadmesh::DrawableQuadmesh(const std::vector<double> &coords,
                                   const std::vector<u_int>  &quads) : Quadmesh(coords, quads)
{
    init();
}

CINO_INLINE
void DrawableQuadmesh::init()
{
    type               = QUADMESH;
    draw_mode          = DRAW_MESH | DRAW_SMOOTH | DRAW_FACECOLOR;

    texture_id         = 0;

    wireframe_width    = 1;
    wireframe_color[0] = 0.1;
    wireframe_color[1] = 0.1;
    wireframe_color[2] = 0.1;
    wireframe_color[3] = 1.0;

    border_width       = 4;
    border_color[0]    = 0.1;
    border_color[1]    = 0.1;
    border_color[2]    = 0.1;
    border_color[3]    = 1.0;

    set_v_color(0.1, 0.8, 0.1);
    set_q_color(0.1, 0.8, 0.1);
    //color_wrt_quad_scalar();
}

CINO_INLINE
void DrawableQuadmesh::clear()
{
    Quadmesh::clear();
    v_colors.clear();
    q_colors.clear();
}

CINO_INLINE
vec3d DrawableQuadmesh::scene_center() const
{
    return bb.center();
}

CINO_INLINE
float DrawableQuadmesh::scene_radius() const
{
    return bb.diag() * 0.5;
}

CINO_INLINE
void DrawableQuadmesh::draw() const
{
    RenderFaceData data;
    data.face_type       = GL_QUADS;
    data.draw_mode       = draw_mode;
    data.coords          = &coords;
    data.faces           = &quads;
    data.v_norms         = &v_norm;
    data.v_colors        = &v_colors;
    data.f_colors        = &q_colors;
    data.text1D          = &u_text;
    data.border_coords   = &border_coords;
    data.border_segs     = &border_segs;
    data.wireframe_color = wireframe_color;
    data.wireframe_width = wireframe_width;
    data.border_color    = border_color;
    data.border_width    = border_width;

    render_faces(data);
}

CINO_INLINE
void DrawableQuadmesh::set_draw_mesh(bool b)
{
    if (b) draw_mode |=  DRAW_MESH;
    else   draw_mode &= ~DRAW_MESH;
}

CINO_INLINE
void DrawableQuadmesh::set_wireframe(bool b)
{
    if (b) draw_mode |=  DRAW_WIREFRAME;
    else   draw_mode &= ~DRAW_WIREFRAME;
}

CINO_INLINE
void DrawableQuadmesh::set_border(bool b)
{
    if (b) draw_mode |=  DRAW_BORDER;
    else   draw_mode &= ~DRAW_BORDER;
}

CINO_INLINE
void DrawableQuadmesh::set_flat_shading()
{
    draw_mode |=  DRAW_FLAT;
    draw_mode &= ~DRAW_SMOOTH;
    draw_mode &= ~DRAW_POINTS;
}

CINO_INLINE
void DrawableQuadmesh::set_smooth_shading()
{
    draw_mode |=  DRAW_SMOOTH;
    draw_mode &= ~DRAW_FLAT;
    draw_mode &= ~DRAW_POINTS;
}

CINO_INLINE
void DrawableQuadmesh::set_points_shading()
{
    draw_mode |=  DRAW_POINTS;
    draw_mode &= ~DRAW_FLAT;
    draw_mode &= ~DRAW_SMOOTH;    
}

CINO_INLINE
void DrawableQuadmesh::set_enable_vertex_color()
{
    draw_mode |=  DRAW_VERTEXCOLOR;
    draw_mode &= ~DRAW_FACECOLOR;
    draw_mode &= ~DRAW_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableQuadmesh::set_enable_quad_color()
{
    draw_mode |=  DRAW_FACECOLOR;
    draw_mode &= ~DRAW_VERTEXCOLOR;
    draw_mode &= ~DRAW_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableQuadmesh::set_enable_texture1D(int texture)
{
    draw_mode |=  DRAW_TEXTURE1D;
    draw_mode &= ~DRAW_VERTEXCOLOR;
    draw_mode &= ~DRAW_FACECOLOR;

    if (texture_id > 0)
    {
        glDeleteTextures(1, &texture_id);
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_1D, texture_id);

    switch (texture)
    {
        case TEXTURE_ISOLINES:
        {
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, isolines_texture1D);
            break;
        }

        case TEXTURE_QUALITY_RAMP:
        {
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, quality_ramp_texture1D);
            break;
        }

        case TEXTURE_QUALITY_RAMP_W_ISOLINES:
        {
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, quality_ramp_texture1D_with_isolines);
            break;
        }

        default : assert("Unknown 1D Texture" && false);
    }

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R,     GL_REPEAT);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableQuadmesh::set_wireframe_color(float r, float g, float b)
{
    wireframe_color[0] = r;
    wireframe_color[1] = g;
    wireframe_color[2] = b;
}

CINO_INLINE
void DrawableQuadmesh::set_wireframe_width(float width)
{
    wireframe_width = width;
}

CINO_INLINE
void DrawableQuadmesh::set_wireframe_transparency(float alpha)
{
    wireframe_color[3] = alpha;
}

CINO_INLINE
void DrawableQuadmesh::set_border_color(float r, float g, float b)
{
    border_color[0] = r;
    border_color[1] = g;
    border_color[2] = b;
}

CINO_INLINE
void DrawableQuadmesh::set_border_width(float width)
{
    border_width = width;
}

CINO_INLINE
void DrawableQuadmesh::set_border_transparency(float alpha)
{
    border_color[3] = alpha;
}

CINO_INLINE
void DrawableQuadmesh::set_v_color(float r, float g, float b)
{
    v_colors.resize(num_vertices()*3);
    for(int i=0; i<(int)v_colors.size(); i+=3)
    {
        v_colors[i + 0] = r;
        v_colors[i + 1] = g;
        v_colors[i + 2] = b;
    }
}

CINO_INLINE
void DrawableQuadmesh::set_q_color(float r, float g, float b)
{
    q_colors.resize(num_quads()*3);
    for(int i=0; i<(int)q_colors.size(); i+=3)
    {
        q_colors[i + 0] = r;
        q_colors[i + 1] = g;
        q_colors[i + 2] = b;
    }
}

CINO_INLINE
const float *DrawableQuadmesh::vertex_color(const int vid) const
{
    return &(v_colors[vid*3]);
}

CINO_INLINE
const float *DrawableQuadmesh::quad_color(const int qid) const
{
    return &(q_colors[qid*3]);
}

CINO_INLINE
void DrawableQuadmesh::set_vertex_color(const int vid, const float *color)
{
    int vid_ptr = vid * 3;
    v_colors.at(vid_ptr + 0) = color[0];
    v_colors.at(vid_ptr + 1) = color[1];
    v_colors.at(vid_ptr + 2) = color[2];
}

CINO_INLINE
void DrawableQuadmesh::set_quad_color(const int qid, const float *color)
{
    int qid_ptr = qid * 3;
    q_colors.at(qid_ptr + 0) = color[0];
    q_colors.at(qid_ptr + 1) = color[1];
    q_colors.at(qid_ptr + 2) = color[2];
}

}
