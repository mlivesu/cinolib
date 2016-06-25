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
#include "drawable_trimesh.h"
#include "../textures/quality_ramp_texture.h"
#include "../textures/quality_ramp_texture_plus_isolines.h"
#include "../textures/isolines_texture.h"
#include "../colors.h"

namespace cinolib
{

CINO_INLINE
DrawableTrimesh::DrawableTrimesh() : Trimesh()
{
    init();
}

CINO_INLINE
DrawableTrimesh::DrawableTrimesh(const char *filename) : Trimesh(filename)
{
    init();
}

CINO_INLINE
DrawableTrimesh::DrawableTrimesh(const std::vector<double> &coords,
                                 const std::vector<u_int>  &tris) : Trimesh(coords, tris)
{
    init();
}

CINO_INLINE
void DrawableTrimesh::init()
{
    type               = TRIMESH;
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
    set_t_color(0.1, 0.8, 0.1);
    //color_wrt_triangle_scalar();
}

CINO_INLINE
void DrawableTrimesh::clear()
{
    Trimesh::clear();
    v_colors.clear();
    t_colors.clear();
}

CINO_INLINE
vec3d DrawableTrimesh::scene_center() const
{
    return bb.center();
}

CINO_INLINE
float DrawableTrimesh::scene_radius() const
{
    return bb.diag() * 0.5;
}

CINO_INLINE
void DrawableTrimesh::draw() const
{
    RenderFaceData data;
    data.face_type       = GL_TRIANGLES;
    data.draw_mode       = draw_mode;
    data.coords          = &coords;
    data.faces           = &tris;
    data.v_norms         = &v_norm;
    data.v_colors        = &v_colors;
    data.f_colors        = &t_colors;
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
void DrawableTrimesh::set_draw_mesh(bool b)
{
    if (b) draw_mode |=  DRAW_MESH;
    else   draw_mode &= ~DRAW_MESH;
}

CINO_INLINE
void DrawableTrimesh::set_wireframe(bool b)
{
    if (b) draw_mode |=  DRAW_WIREFRAME;
    else   draw_mode &= ~DRAW_WIREFRAME;
}

CINO_INLINE
void DrawableTrimesh::set_border(bool b)
{
    if (b) draw_mode |=  DRAW_BORDER;
    else   draw_mode &= ~DRAW_BORDER;
}

CINO_INLINE
void DrawableTrimesh::set_flat_shading()
{
    draw_mode |=  DRAW_FLAT;
    draw_mode &= ~DRAW_SMOOTH;
    draw_mode &= ~DRAW_POINTS;
}

CINO_INLINE
void DrawableTrimesh::set_smooth_shading()
{
    draw_mode |=  DRAW_SMOOTH;
    draw_mode &= ~DRAW_FLAT;
    draw_mode &= ~DRAW_POINTS;
}

CINO_INLINE
void DrawableTrimesh::set_points_shading()
{
    draw_mode |=  DRAW_POINTS;
    draw_mode &= ~DRAW_FLAT;
    draw_mode &= ~DRAW_SMOOTH;    
}

CINO_INLINE
void DrawableTrimesh::set_enable_vertex_color()
{
    draw_mode |=  DRAW_VERTEXCOLOR;
    draw_mode &= ~DRAW_FACECOLOR;
    draw_mode &= ~DRAW_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableTrimesh::set_enable_triangle_color()
{
    draw_mode |=  DRAW_FACECOLOR;
    draw_mode &= ~DRAW_VERTEXCOLOR;
    draw_mode &= ~DRAW_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableTrimesh::set_enable_texture1D(int texture)
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
void DrawableTrimesh::set_wireframe_color(float r, float g, float b)
{
    wireframe_color[0] = r;
    wireframe_color[1] = g;
    wireframe_color[2] = b;
}

CINO_INLINE
void DrawableTrimesh::set_wireframe_width(float width)
{
    wireframe_width = width;
}

CINO_INLINE
void DrawableTrimesh::set_wireframe_transparency(float alpha)
{
    wireframe_color[3] = alpha;
}

CINO_INLINE
void DrawableTrimesh::set_border_color(float r, float g, float b)
{
    border_color[0] = r;
    border_color[1] = g;
    border_color[2] = b;
}

CINO_INLINE
void DrawableTrimesh::set_border_width(float width)
{
    border_width = width;
}

CINO_INLINE
void DrawableTrimesh::set_border_transparency(float alpha)
{
    border_color[3] = alpha;
}

CINO_INLINE
void DrawableTrimesh::set_v_color(float r, float g, float b)
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
void DrawableTrimesh::set_t_color(float r, float g, float b)
{
    t_colors.resize(num_triangles()*3);
    for(int i=0; i<(int)t_colors.size(); i+=3)
    {
        t_colors[i + 0] = r;
        t_colors[i + 1] = g;
        t_colors[i + 2] = b;
    }
}

CINO_INLINE
const float *DrawableTrimesh::vertex_color(const int vid) const
{
    return &(v_colors[vid*3]);
}

CINO_INLINE
const float *DrawableTrimesh::triangle_color(const int tid) const
{
    return &(t_colors[tid*3]);
}

CINO_INLINE
void DrawableTrimesh::set_vertex_color(const int vid, const float *color)
{
    int vid_ptr = vid * 3;
    v_colors.at(vid_ptr + 0) = color[0];
    v_colors.at(vid_ptr + 1) = color[1];
    v_colors.at(vid_ptr + 2) = color[2];
}

CINO_INLINE
void DrawableTrimesh::set_triangle_color(const int tid, const float *color)
{
    int tid_ptr = tid * 3;
    t_colors.at(tid_ptr + 0) = color[0];
    t_colors.at(tid_ptr + 1) = color[1];
    t_colors.at(tid_ptr + 2) = color[2];
}

CINO_INLINE
void DrawableTrimesh::unglue_face_colors()
{
    border_vertices.clear();

    for(int tid=0; tid<num_triangles(); ++tid)
    {
        std::vector<int> nbrs = adj_tri2tri(tid);

        for(int i=0; i<(int)nbrs.size(); ++i)
        {
            int nbr = nbrs[i];

            const float *tid_rgb = triangle_color(tid);
            const float *nbr_rgb = triangle_color(nbr);

            if (tid_rgb[0] != nbr_rgb[0] ||
                tid_rgb[1] != nbr_rgb[1] ||
                tid_rgb[2] != nbr_rgb[2])
            {
                ipair e = shared_edge(tid, nbr);
                border_vertices.push_back(e.first);
                border_vertices.push_back(e.second);
            }
        }
    }
    update_border();
}

CINO_INLINE
void DrawableTrimesh::unglue_boundary_edges()
{
    std::vector<ipair> boundary = get_boundary_edges();

    border_vertices.clear();
    for(int i=0; i<(int)boundary.size(); ++i)
    {
        border_vertices.push_back(boundary[i].first);
        border_vertices.push_back(boundary[i].second);
    }
    update_border();
}

CINO_INLINE
void DrawableTrimesh::color_wrt_triangle_scalar()
{    
    if (num_triangles() == 0) return;

    // labels may not be from 0 to n-1....
    //
    std::map<int,int> l_map;
    for(int l : t_label)
    {
        auto it = l_map.find(l);
        if (it == l_map.end()) l_map[l] = l_map.size();
    }

    t_colors.resize(num_triangles()*3);
    for(int tid=0; tid<num_triangles(); ++tid)
    {
        float rgb[3];
        scattered_color(l_map.size(), l_map[triangle_label(tid)], rgb);
        set_triangle_color(tid, rgb);
    }

    unglue_face_colors();
}

CINO_INLINE
std::vector<int> DrawableTrimesh::get_border_vertices() const
{
    return border_vertices;
}

CINO_INLINE
int DrawableTrimesh::add_vertex(const vec3d & v, const float scalar)
{
    v_colors.push_back(0.0);
    v_colors.push_back(0.0);
    v_colors.push_back(0.0);
    return Trimesh::add_vertex(v, scalar);
}

CINO_INLINE
int DrawableTrimesh::add_triangle(const int vid0, const int vid1, const int vid2, const int scalar)
{    
    t_colors.push_back(0.0);
    t_colors.push_back(0.0);
    t_colors.push_back(0.0);
    return Trimesh::add_triangle(vid0, vid1, vid2, scalar);
}

CINO_INLINE
void DrawableTrimesh::operator+=(const DrawableTrimesh & m)
{
    Trimesh::operator+=(m);

    int nv = num_vertices();

    for(int tid=0; tid<m.num_triangles(); ++tid)
    {
        t_colors.push_back(m.t_colors[3*tid + 0]);
        t_colors.push_back(m.t_colors[3*tid + 1]);
        t_colors.push_back(m.t_colors[3*tid + 2]);
    }
    for(int vid=0; vid<m.num_vertices(); ++vid)
    {
        v_colors.push_back(m.v_colors[3*vid + 0]);
        v_colors.push_back(m.v_colors[3*vid + 1]);
        v_colors.push_back(m.v_colors[3*vid + 2]);
    }
    for(int b : m.border_vertices)
    {
        border_vertices.push_back(nv + b);
    }
}

CINO_INLINE
void DrawableTrimesh::update_border()
{
    border_coords.clear();
    border_segs.clear();
    float delta = bb.diag() * 0.0005;
    for(size_t i=0; i<border_vertices.size(); i+=2)
    {
        vec3d v0 = vertex(border_vertices[i]);
        vec3d v1 = vertex(border_vertices[i+1]);
        vec3d n0 = vertex_normal(border_vertices[i]);
        vec3d n1 = vertex_normal(border_vertices[i+1]);

        v0 = v0 + n0 * delta;
        v1 = v1 + n1 * delta;

        int base_addr = border_coords.size()/3;

        border_segs.push_back(base_addr);
        border_segs.push_back(base_addr+1);

        border_coords.push_back(v0.x());
        border_coords.push_back(v0.y());
        border_coords.push_back(v0.z());
        border_coords.push_back(v1.x());
        border_coords.push_back(v1.y());
        border_coords.push_back(v1.z());
    }
}

}
