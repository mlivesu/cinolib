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
    draw_mode          = DRAW_MESH | DRAW_SMOOTH | DRAW_FACECOLOR | DRAW_BORDERS;

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
void DrawableTrimesh::render_pass() const
{
    if (draw_mode & DRAW_POINTS)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, coords.data());

        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer (3, GL_FLOAT, 0, v_colors.data());

        glDrawArrays(GL_POINTS, 0, num_vertices());

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else if (draw_mode & DRAW_SMOOTH || draw_mode & DRAW_FLAT)
    {
        // Old fashioned (orrible) rendering
        //
        if (draw_mode & DRAW_FACECOLOR)
        {
            int n_tris = tris.size()/3;
            for(int tid=0; tid<n_tris; ++tid)
            {
                int tid_ptr  = 3 * tid;
                int vid0     = tris[tid_ptr + 0];
                int vid1     = tris[tid_ptr + 1];
                int vid2     = tris[tid_ptr + 2];
                int vid0_ptr = 3 * vid0;
                int vid1_ptr = 3 * vid1;
                int vid2_ptr = 3 * vid2;

                glBegin(GL_TRIANGLES);
                glColor3fv(&(t_colors[tid_ptr]));
                glNormal3dv(&(v_norm[vid0_ptr]));
                glVertex3dv(&(coords[vid0_ptr]));
                glNormal3dv(&(v_norm[vid1_ptr]));
                glVertex3dv(&(coords[vid1_ptr]));
                glNormal3dv(&(v_norm[vid2_ptr]));
                glVertex3dv(&(coords[vid2_ptr]));
                glEnd();
            }
        }
        else
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_DOUBLE, 0, coords.data());

            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_DOUBLE, 0, v_norm.data());

            if (draw_mode & DRAW_VERTEXCOLOR)
            {
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(3, GL_FLOAT, 0, v_colors.data());
            }
            else if (draw_mode & DRAW_TEXTURE1D)
            {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(1, GL_FLOAT, 0, u_text.data());
                glColor3f(1.0,1.0,1.0);
            }

            glDrawElements(GL_TRIANGLES, tris.size(), GL_UNSIGNED_INT, tris.data());

            if (draw_mode & DRAW_TEXTURE1D)   glDisableClientState(GL_TEXTURE_COORD_ARRAY); else
            if (draw_mode & DRAW_VERTEXCOLOR) glDisableClientState(GL_COLOR_ARRAY);

            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }

    if (draw_mode & DRAW_WIREFRAME)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, coords.data());

        glLineWidth(wireframe_width);
        glColor4fv(wireframe_color);

        glDrawElements(GL_TRIANGLES, tris.size(), GL_UNSIGNED_INT, tris.data());

        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (draw_mode & DRAW_BORDERS)
    {
        glDisable(GL_LIGHTING);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        float delta = bb.diag() * 0.0005;
        for(int i=0; i<(int)borders.size(); i+=2)
        {
            vec3d v0 = vertex(borders[i]);
            vec3d v1 = vertex(borders[i+1]);

            vec3d n0 = vertex_normal(borders[i]);
            vec3d n1 = vertex_normal(borders[i+1]);

            v0 = v0 + n0 * delta;
            v1 = v1 + n1 * delta;

            glLineWidth(border_width);
            glColor4fv(border_color);

            glBegin(GL_LINES);
            glVertex3f(v0.x(), v0.y(), v0.z());
            glVertex3f(v1.x(), v1.y(), v1.z());
            glEnd();
        }
    }
}


CINO_INLINE
void DrawableTrimesh::draw() const
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable(GL_CULL_FACE);

    if (draw_mode & DRAW_MESH)
    {
        if (draw_mode & DRAW_WIREFRAME)
        {
            if (draw_mode & DRAW_POINTS)
            {
                glDisable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                render_pass();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            else

            if (draw_mode & DRAW_FLAT)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                glDepthRange(0.01, 1.0);
                render_pass();

                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                glDepthFunc(GL_LEQUAL);
                render_pass();
                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            else

            if (draw_mode & DRAW_SMOOTH)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);
                glDepthRange(0.01, 1.0);
                render_pass();

                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                glDepthFunc(GL_LEQUAL);
                render_pass();
                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        else

        {
            if (draw_mode & DRAW_POINTS)
            {
                glDisable(GL_LIGHTING);
                render_pass();
            }

            else

            if (draw_mode & DRAW_FLAT)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                render_pass();
            }

            else

            if (draw_mode & DRAW_SMOOTH)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);
                render_pass();
            }
        }
    }
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
    if (b) draw_mode |=  DRAW_BORDERS;
    else   draw_mode &= ~DRAW_BORDERS;
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
void DrawableTrimesh::unglue_face_colors()
{
    borders.clear();

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
                borders.push_back(e.first);
                borders.push_back(e.second);
            }
        }
    }
    //logger << borders.size() / 2 << " border egdes found" << endl;
}

CINO_INLINE
void DrawableTrimesh::unglue_boundary_edges()
{
    std::vector<ipair> boundary = get_boundary_edges();

    borders.clear();
    for(int i=0; i<(int)boundary.size(); ++i)
    {
        borders.push_back(boundary[i].first);
        borders.push_back(boundary[i].second);
    }
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
    return borders;
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
    for(int b : m.borders)
    {
        borders.push_back(nv + b);
    }
}

}
