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
#include <cinolib/meshes/tetmesh/drawable_tetmesh.h>
#include <cinolib/textures/isolines_texture.h>
#include <cinolib/textures/quality_ramp_texture.h>
#include <cinolib/textures/quality_ramp_texture_plus_isolines.h>
#include <cinolib/colors.h>


namespace cinolib
{

CINO_INLINE
DrawableTetmesh::DrawableTetmesh() : Tetmesh()
{
    init();
}

CINO_INLINE
DrawableTetmesh::DrawableTetmesh(const char *filename) : Tetmesh(filename)
{
    init();
}

CINO_INLINE
DrawableTetmesh::DrawableTetmesh(const std::vector<double> & coords,
                                 const std::vector<u_int>  & tets) : Tetmesh(coords, tets)
{
    init();
}

CINO_INLINE
void DrawableTetmesh::init()
{
    type          = TETMESH;

    draw_mode_out = DRAW_MESH | DRAW_FLAT | DRAW_FACECOLOR | DRAW_WIREFRAME;
    draw_mode_in  =             DRAW_FLAT | DRAW_FACECOLOR | DRAW_WIREFRAME;

    wireframe_out_width    = 1;
    wireframe_out_color[0] = 0.1;
    wireframe_out_color[1] = 0.1;
    wireframe_out_color[2] = 0.1;
    wireframe_out_color[3] = 1.0;

    wireframe_in_width    = 1;
    wireframe_in_color[0] = 0.1;
    wireframe_in_color[1] = 0.1;
    wireframe_in_color[2] = 0.1;
    wireframe_in_color[3] = 1.0;

    slice_mask = std::vector<bool>(num_tetrahedra(), false);

    slice_thresh[ X ] =   bb.max.x();
    slice_thresh[ Y ] =   bb.max.y();
    slice_thresh[ Z ] =   bb.max.z();
    slice_thresh[ Q ] =   1.0;
    slice_thresh[ L ] =  -1.0;

    slice_dir[ X ] = true;
    slice_dir[ Y ] = true;
    slice_dir[ Z ] = true;
    slice_dir[ Q ] = true;
    slice_dir[ L ] = true;

    set_t_out_color(0.1, 0.8, 0.1);
    set_t_in_color(1.0, 1.0, 1.0);

    update_outer_visible_mesh();
}

CINO_INLINE
void DrawableTetmesh::operator+=(const DrawableTetmesh & m)
{
    Tetmesh::operator+=(m);

    slice_thresh[ X ] =  bb.max.x();
    slice_thresh[ Y ] =  bb.max.y();
    slice_thresh[ Z ] =  bb.max.z();

    slice_mask.resize(num_tetrahedra());

    for(int tid=0; tid<m.num_tetrahedra(); ++tid)
    {        
        t_out_colors.push_back(m.t_out_colors[3*tid + 0]);
        t_out_colors.push_back(m.t_out_colors[3*tid + 1]);
        t_out_colors.push_back(m.t_out_colors[3*tid + 2]);
        t_in_colors.push_back(m.t_in_colors[3*tid + 0]);
        t_in_colors.push_back(m.t_in_colors[3*tid + 1]);
        t_in_colors.push_back(m.t_in_colors[3*tid + 2]);
    }

    update_outer_visible_mesh();
}

CINO_INLINE
vec3d DrawableTetmesh::scene_center() const
{
    return bb.center();
}

CINO_INLINE
float DrawableTetmesh::scene_radius() const
{
    return bb.diag() * 0.5;
}

CINO_INLINE
void DrawableTetmesh::draw() const
{
    RenderFaceData data_out;
    data_out.face_type       = GL_TRIANGLES;
    data_out.draw_mode       = draw_mode_out;
    data_out.coords          = &outer_visible_coords;
    data_out.faces           = &outer_visible_tris;
    data_out.v_norms         = &outer_visible_v_norms;
    data_out.f_colors        = &outer_visible_t_colors;
    data_out.text1D          = &outer_visible_f_values;
    data_out.wireframe_color = wireframe_out_color;
    data_out.wireframe_width = wireframe_out_width;

    render_faces(data_out);

    if (draw_mode_in & DRAW_MESH)
    {
        RenderFaceData data_in;
        data_in.face_type       = GL_TRIANGLES;
        data_in.draw_mode       = draw_mode_in;
        data_in.coords          = &inner_slice_coords;
        data_in.faces           = &inner_slice_tris;
        data_in.v_norms         = &inner_slice_v_norms;
        data_in.f_colors        = &inner_slice_t_colors;
        data_in.text1D          = &inner_slice_f_values;
        data_in.wireframe_color = wireframe_in_color;
        data_in.wireframe_width = wireframe_in_width;

        render_faces(data_in);
    }
}


CINO_INLINE
void DrawableTetmesh::set_t_out_color(const float r, const float g, const float b)
{
    t_out_colors.resize(num_tetrahedra()*3);
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        int tid_ptr = tid * 3;
        t_out_colors[tid_ptr + 0] = r;
        t_out_colors[tid_ptr + 1] = g;
        t_out_colors[tid_ptr + 2] = b;
    }
}

CINO_INLINE
void DrawableTetmesh::set_t_in_color(const float r, const float g, const float b)
{
    t_in_colors.resize(num_tetrahedra()*3);
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        int tid_ptr = tid * 3;
        t_in_colors[tid_ptr + 0] = r;
        t_in_colors[tid_ptr + 1] = g;
        t_in_colors[tid_ptr + 2] = b;
    }
}

CINO_INLINE
void DrawableTetmesh::set_slice_parameters(const float thresh, const int item, const bool dir, const bool mode)
{
    if (draw_mode_in & DRAW_MESH)
    {
        switch (item)
        {
            case X:
            case Y:
            case Z: slice_thresh[item] = bb.min[item] + bb.delta()[item] * thresh; break;
            case Q: slice_thresh[item] = thresh; break;
            case L: slice_thresh[item] = thresh; break;
            default: assert(0);
        }
        slice_dir[item] = dir;
        update_slice(mode);
    }
}

CINO_INLINE
void DrawableTetmesh::update_slice(const bool mode)
{
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        vec3d  c = tet_centroid(tid);
        double q = tet_quality(tid);
        int    l = static_cast<int>(slice_thresh[L]);

        bool pass_x = (slice_dir[X]) ? (c.x() <= slice_thresh[X]) : (c.x() >= slice_thresh[X]);
        bool pass_y = (slice_dir[Y]) ? (c.y() <= slice_thresh[Y]) : (c.y() >= slice_thresh[Y]);
        bool pass_z = (slice_dir[Z]) ? (c.z() <= slice_thresh[Z]) : (c.z() >= slice_thresh[Z]);
        bool pass_q = (slice_dir[Q]) ? (q     <= slice_thresh[Q]) : (q     >= slice_thresh[Q]);
        bool pass_l = (l == -1 || t_label[tid] != l);

        slice_mask[tid] = (mode) ? (pass_x && pass_y && pass_z && pass_q && pass_l) : (!pass_x || !pass_y || !pass_z || !pass_q || !pass_l);
    }

    update_outer_visible_mesh();
    update_inner_slice();
}

CINO_INLINE
void DrawableTetmesh::update_inner_slice()
{
    inner_slice_coords.clear();
    inner_slice_tris.clear();
    inner_slice_v_norms.clear();
    inner_slice_f_values.clear();
    inner_slice_t_colors.clear();

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        if ((draw_mode_in & DRAW_MESH) && (slice_mask[tid])) continue;

        for(auto nbr : adj_tet2tet(tid))
        {
            if (slice_mask[nbr])
            {
                int shared_f = shared_facet(tid, nbr);
                assert(shared_f > -1);

                int vid0     = tet_vertex_id(tid, TET_FACES[shared_f][0]);
                int vid1     = tet_vertex_id(tid, TET_FACES[shared_f][1]);
                int vid2     = tet_vertex_id(tid, TET_FACES[shared_f][2]);
                int vid0_ptr = 3 * vid0;
                int vid1_ptr = 3 * vid1;
                int vid2_ptr = 3 * vid2;

                int base_addr = inner_slice_tris.size();

                inner_slice_tris.push_back(base_addr);
                inner_slice_tris.push_back(base_addr + 1);
                inner_slice_tris.push_back(base_addr + 2);

                int tid_ptr = tid * 3;
                inner_slice_t_colors.push_back(t_in_colors[tid_ptr + 0]);
                inner_slice_t_colors.push_back(t_in_colors[tid_ptr + 1]);
                inner_slice_t_colors.push_back(t_in_colors[tid_ptr + 2]);

                inner_slice_coords.push_back(coords[vid0_ptr + 0]);
                inner_slice_coords.push_back(coords[vid0_ptr + 1]);
                inner_slice_coords.push_back(coords[vid0_ptr + 2]);
                inner_slice_coords.push_back(coords[vid1_ptr + 0]);
                inner_slice_coords.push_back(coords[vid1_ptr + 1]);
                inner_slice_coords.push_back(coords[vid1_ptr + 2]);
                inner_slice_coords.push_back(coords[vid2_ptr + 0]);
                inner_slice_coords.push_back(coords[vid2_ptr + 1]);
                inner_slice_coords.push_back(coords[vid2_ptr + 2]);

                vec3d v0 = vertex(vid0);
                vec3d v1 = vertex(vid1);
                vec3d v2 = vertex(vid2);
                vec3d u  = v1 - v0; u.normalize();
                vec3d v  = v2 - v0; v.normalize();
                vec3d n = u.cross(v);
                n.normalize();

                inner_slice_v_norms.push_back(n.x());
                inner_slice_v_norms.push_back(n.y());
                inner_slice_v_norms.push_back(n.z());
                inner_slice_v_norms.push_back(n.x());
                inner_slice_v_norms.push_back(n.y());
                inner_slice_v_norms.push_back(n.z());
                inner_slice_v_norms.push_back(n.x());
                inner_slice_v_norms.push_back(n.y());
                inner_slice_v_norms.push_back(n.z());

                inner_slice_f_values.push_back(vertex_u_text(vid0));
                inner_slice_f_values.push_back(vertex_u_text(vid1));
                inner_slice_f_values.push_back(vertex_u_text(vid2));
            }
        }
    }
}

CINO_INLINE
void DrawableTetmesh::update_outer_visible_mesh()
{
    outer_visible_coords.clear();
    outer_visible_tris.clear();
    outer_visible_v_norms.clear();
    outer_visible_f_values.clear();
    outer_visible_t_colors.clear();

    for(int tid=0; tid<num_srf_triangles(); ++tid)
    {
        if ((draw_mode_in & DRAW_MESH) && (slice_mask[adj_tri2tet(tid)])) continue;

        int tid_ptr  = 3 * tid;
        int vid0     = tris[tid_ptr + 0];
        int vid1     = tris[tid_ptr + 1];
        int vid2     = tris[tid_ptr + 2];
        int vid0_ptr = 3 * vid0;
        int vid1_ptr = 3 * vid1;
        int vid2_ptr = 3 * vid2;

        int base_addr = outer_visible_tris.size();

        outer_visible_tris.push_back(base_addr);
        outer_visible_tris.push_back(base_addr + 1);
        outer_visible_tris.push_back(base_addr + 2);

        outer_visible_coords.push_back(coords[vid0_ptr + 0]);
        outer_visible_coords.push_back(coords[vid0_ptr + 1]);
        outer_visible_coords.push_back(coords[vid0_ptr + 2]);
        outer_visible_coords.push_back(coords[vid1_ptr + 0]);
        outer_visible_coords.push_back(coords[vid1_ptr + 1]);
        outer_visible_coords.push_back(coords[vid1_ptr + 2]);
        outer_visible_coords.push_back(coords[vid2_ptr + 0]);
        outer_visible_coords.push_back(coords[vid2_ptr + 1]);
        outer_visible_coords.push_back(coords[vid2_ptr + 2]);

        outer_visible_v_norms.push_back(t_norm[tid_ptr + 0]);
        outer_visible_v_norms.push_back(t_norm[tid_ptr + 1]);
        outer_visible_v_norms.push_back(t_norm[tid_ptr + 2]);
        outer_visible_v_norms.push_back(t_norm[tid_ptr + 0]);
        outer_visible_v_norms.push_back(t_norm[tid_ptr + 1]);
        outer_visible_v_norms.push_back(t_norm[tid_ptr + 2]);
        outer_visible_v_norms.push_back(t_norm[tid_ptr + 0]);
        outer_visible_v_norms.push_back(t_norm[tid_ptr + 1]);
        outer_visible_v_norms.push_back(t_norm[tid_ptr + 2]);

        outer_visible_f_values.push_back(vertex_u_text(vid0));
        outer_visible_f_values.push_back(vertex_u_text(vid1));
        outer_visible_f_values.push_back(vertex_u_text(vid2));

        int tet_id_ptr = adj_tri2tet(tid) * 3;
        outer_visible_t_colors.push_back(t_out_colors[tet_id_ptr + 0]);
        outer_visible_t_colors.push_back(t_out_colors[tet_id_ptr + 1]);
        outer_visible_t_colors.push_back(t_out_colors[tet_id_ptr + 2]);
    }
}

CINO_INLINE
void DrawableTetmesh::set_out_wireframe_color(float r, float g, float b)
{
    wireframe_out_color[0] = r;
    wireframe_out_color[1] = g;
    wireframe_out_color[2] = b;
}

CINO_INLINE
void DrawableTetmesh::set_in_wireframe_color(float r, float g, float b)
{
    wireframe_in_color[0] = r;
    wireframe_in_color[1] = g;
    wireframe_in_color[2] = b;
}

CINO_INLINE
void DrawableTetmesh::set_out_wireframe_width(float width)
{
    wireframe_out_width = width;
}

CINO_INLINE
void DrawableTetmesh::set_in_wireframe_width(float width)
{
    wireframe_in_width = width;
}

CINO_INLINE
void DrawableTetmesh::set_out_wireframe_transparency(float alpha)
{
    wireframe_out_color[3] = alpha;
}

CINO_INLINE
void DrawableTetmesh::set_in_wireframe_transparency(float alpha)
{
    wireframe_in_color[3] = alpha;
}

CINO_INLINE
void DrawableTetmesh::set_draw_mesh(bool b)
{
    if (b)
    {
        draw_mode_out |=  DRAW_MESH;
    }
    else
    {
        draw_mode_out &= ~DRAW_MESH;
        draw_mode_in  &= ~DRAW_MESH;
    }
}

CINO_INLINE
void DrawableTetmesh::set_out_wireframe(bool b)
{
    if (b) draw_mode_out |=  DRAW_WIREFRAME;
    else   draw_mode_out &= ~DRAW_WIREFRAME;
}

CINO_INLINE
void DrawableTetmesh::set_in_wireframe(bool b)
{
    if (b) draw_mode_in |=  DRAW_WIREFRAME;
    else   draw_mode_in &= ~DRAW_WIREFRAME;
}

CINO_INLINE
void DrawableTetmesh::set_flat_shading()
{
    draw_mode_out |=  DRAW_FLAT;
    draw_mode_out &= ~DRAW_SMOOTH;
    draw_mode_out &= ~DRAW_POINTS;

    draw_mode_in  |=  DRAW_FLAT;
    draw_mode_in  &= ~DRAW_SMOOTH;
    draw_mode_in  &= ~DRAW_POINTS;
}

CINO_INLINE
void DrawableTetmesh::set_smooth_shading()
{
    draw_mode_out |=  DRAW_SMOOTH;
    draw_mode_out &= ~DRAW_FLAT;
    draw_mode_out &= ~DRAW_POINTS;

    draw_mode_in  |=  DRAW_SMOOTH;
    draw_mode_in  &= ~DRAW_FLAT;
    draw_mode_in  &= ~DRAW_POINTS;
}

CINO_INLINE
void DrawableTetmesh::set_points_shading()
{
    draw_mode_out |=  DRAW_POINTS;
    draw_mode_out &= ~DRAW_FLAT;
    draw_mode_out &= ~DRAW_SMOOTH;

    draw_mode_in  |=  DRAW_POINTS;
    draw_mode_in  &= ~DRAW_FLAT;
    draw_mode_in  &= ~DRAW_SMOOTH;
}

CINO_INLINE
void DrawableTetmesh::set_draw_slice(bool b)
{
    if (b) draw_mode_in |=  DRAW_MESH;
    else   draw_mode_in &= ~DRAW_MESH;
}

CINO_INLINE
void DrawableTetmesh::set_enable_out_face_color()
{
    draw_mode_out |=  DRAW_FACECOLOR;
    draw_mode_out &= ~DRAW_VERTEXCOLOR;
    draw_mode_out &= ~DRAW_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
}


CINO_INLINE
void DrawableTetmesh::set_enable_in_face_color()
{
    draw_mode_in |=  DRAW_FACECOLOR;
    draw_mode_in &= ~DRAW_VERTEXCOLOR;
    draw_mode_in &= ~DRAW_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableTetmesh::set_enable_out_quality_color()
{
    draw_mode_out |=  DRAW_FACECOLOR;
    draw_mode_out &= ~DRAW_VERTEXCOLOR;
    draw_mode_out &= ~DRAW_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);

    color_tets_wrt_quality(false, true);
}

CINO_INLINE
void DrawableTetmesh::set_enable_in_quality_color()
{
    draw_mode_in |=  DRAW_FACECOLOR;
    draw_mode_in &= ~DRAW_VERTEXCOLOR;
    draw_mode_in &= ~DRAW_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);

    color_tets_wrt_quality(true, false);
}

CINO_INLINE
void DrawableTetmesh::set_enable_out_texture1D(int texture)
{
    draw_mode_out |=  DRAW_TEXTURE1D;
    draw_mode_out &= ~DRAW_VERTEXCOLOR;
    draw_mode_out &= ~DRAW_FACECOLOR;

    if (texture_out_id > 0)
    {
        glDeleteTextures(1, &texture_out_id);
    }

    glGenTextures(1, &texture_out_id);
    glBindTexture(GL_TEXTURE_1D, texture_out_id);

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
void DrawableTetmesh::set_enable_in_texture1D(int texture)
{
    draw_mode_in |=  DRAW_TEXTURE1D;
    draw_mode_in &= ~DRAW_VERTEXCOLOR;
    draw_mode_in &= ~DRAW_FACECOLOR;

    if (texture_in_id > 0)
    {
        glDeleteTextures(1, &texture_in_id);
    }

    glGenTextures(1, &texture_in_id);
    glBindTexture(GL_TEXTURE_1D, texture_in_id);

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
void DrawableTetmesh::color_wrt_tet_scalar()
{
    t_out_colors.resize(num_tetrahedra()*3);

    int n_colors = *max_element(t_label.begin(), t_label.end()) + 1;

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        float rgb[3];
        scattered_color(n_colors, tet_label(tid), rgb);

        int tid_ptr = tid * 3;
        t_out_colors[tid_ptr + 0] = rgb[0];
        t_out_colors[tid_ptr + 1] = rgb[1];
        t_out_colors[tid_ptr + 2] = rgb[2];

        t_in_colors[tid_ptr + 0] = rgb[0];
        t_in_colors[tid_ptr + 1] = rgb[1];
        t_in_colors[tid_ptr + 2] = rgb[2];
    }
    update_slice();
}


CINO_INLINE
void DrawableTetmesh::color_tets_wrt_quality(const bool in, const bool out)
{
    if (out) t_out_colors.resize(num_tetrahedra()*3);
    if (in)  t_in_colors.resize(num_tetrahedra()*3);

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        float r,g,b;
        double q = tet_quality(tid);

        if (q < 0)
        {
            r = 1.0;
            g = 0.0;
            b = 0.0;
        }
        else if (q <= 0.5)
        {
            q *= 2.0;
            r = WHITE[0] * q + RED[0] * (1.0 - q);
            g = WHITE[1] * q + RED[1] * (1.0 - q);
            b = WHITE[2] * q + RED[2] * (1.0 - q);
        }
        else
        {
            q = 2.0 * q - 1.0;
            r = BLUE[0] * q + WHITE[0] * (1.0 - q);
            g = BLUE[1] * q + WHITE[1] * (1.0 - q);
            b = BLUE[2] * q + WHITE[2] * (1.0 - q);
        }

        if (out)
        {
            t_out_colors[3*tid+0] = r;
            t_out_colors[3*tid+1] = g;
            t_out_colors[3*tid+2] = b;
        }

        if (in)
        {
            t_in_colors[3*tid+0] = r;
            t_in_colors[3*tid+1] = g;
            t_in_colors[3*tid+2] = b;
        }
    }
}

}
