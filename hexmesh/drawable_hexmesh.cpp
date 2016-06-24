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
#include "drawable_hexmesh.h"
#include "../textures/quality_ramp_texture.h"
#include "../textures/quality_ramp_texture_plus_isolines.h"
#include "../textures/isolines_texture.h"
#include "../colors.h"

#include <iostream>


namespace cinolib
{

CINO_INLINE
DrawableHexmesh::DrawableHexmesh() : Hexmesh()
{
    init();
}

CINO_INLINE
DrawableHexmesh::DrawableHexmesh(const char *filename) : Hexmesh(filename)
{
    init();
}

CINO_INLINE
DrawableHexmesh::DrawableHexmesh(const std::vector<double> & coords,
                                 const std::vector<u_int>  & hexa) : Hexmesh(coords, hexa)
{
    init();
}

CINO_INLINE
void DrawableHexmesh::init()
{
    type      = HEXMESH;
    draw_mode = DRAW_MESH | DRAW_OUT_FLAT | DRAW_OUT_FACECOLOR | DRAW_OUT_WIREFRAME
                          | DRAW_IN_FLAT  | DRAW_IN_FACECOLOR  | DRAW_IN_WIREFRAME;

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

    slice_mask = std::vector<bool>(num_hexahedra(), false);

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

    set_h_out_color(0.1, 0.8, 0.1);
    set_h_in_color(1.0, 1.0, 1.0);

    update_outer_visible_mesh();
}

CINO_INLINE
void DrawableHexmesh::operator+=(const DrawableHexmesh & m)
{
    Hexmesh::operator+=(m);

    slice_thresh[ X ] =  bb.max.x();
    slice_thresh[ Y ] =  bb.max.y();
    slice_thresh[ Z ] =  bb.max.z();

    slice_mask.resize(num_hexahedra());

    for(int hid=0; hid<m.num_hexahedra(); ++hid)
    {        
        h_out_colors.push_back(m.h_out_colors[3*hid + 0]);
        h_out_colors.push_back(m.h_out_colors[3*hid + 1]);
        h_out_colors.push_back(m.h_out_colors[3*hid + 2]);
        h_in_colors.push_back(m.h_in_colors[3*hid + 0]);
        h_in_colors.push_back(m.h_in_colors[3*hid + 1]);
        h_in_colors.push_back(m.h_in_colors[3*hid + 2]);
    }

    update_outer_visible_mesh();
}

CINO_INLINE
vec3d DrawableHexmesh::scene_center() const
{
    return bb.center();
}

CINO_INLINE
float DrawableHexmesh::scene_radius() const
{
    return bb.diag() * 0.5;
}

CINO_INLINE
void DrawableHexmesh::draw() const
{
    draw_out();

    if (draw_mode & DRAW_IN_SLICE)
    {
        draw_in();
    }
}

CINO_INLINE
void DrawableHexmesh::draw_out() const
{
    if (draw_mode & DRAW_MESH)
    {
        if (draw_mode & DRAW_OUT_WIREFRAME)
        {
            if (draw_mode & DRAW_OUT_POINTS)
            {
                glDisable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                render_pass_out();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            else

            if (draw_mode & DRAW_OUT_FLAT)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                glDepthRange(0.01, 1.0);
                render_pass_out();

                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                glDepthFunc(GL_LEQUAL);
                render_pass_out();
                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            else

            if (draw_mode & DRAW_OUT_SMOOTH)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);
                glDepthRange(0.01, 1.0);
                render_pass_out();

                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                glDepthFunc(GL_LEQUAL);
                render_pass_out();
                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        else

        {
            if (draw_mode & DRAW_OUT_POINTS)
            {
                glDisable(GL_LIGHTING);
                render_pass_out();
            }

            else

            if (draw_mode & DRAW_OUT_FLAT)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                render_pass_out();
            }

            else

            if (draw_mode & DRAW_OUT_SMOOTH)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);
                render_pass_out();
            }
        }
    }
}

CINO_INLINE
void DrawableHexmesh::draw_in() const
{
    if (draw_mode & DRAW_MESH)
    {
        if (draw_mode & DRAW_IN_WIREFRAME)
        {
            if (draw_mode & DRAW_IN_FLAT)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                glDepthRange(0.01, 1.0);
                render_pass_in();

                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                glDepthFunc(GL_LEQUAL);
                render_pass_in();
                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            else

            if (draw_mode & DRAW_IN_SMOOTH)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);
                glDepthRange(0.01, 1.0);
                render_pass_in();

                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDepthRange(0.0, 1.0);
                glDepthFunc(GL_LEQUAL);
                render_pass_in();
                glDepthFunc(GL_LESS);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        else

        {
            if (draw_mode & DRAW_IN_FLAT)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_FLAT);
                render_pass_in();
            }

            else

            if (draw_mode & DRAW_IN_SMOOTH)
            {
                glEnable(GL_LIGHTING);
                glShadeModel(GL_SMOOTH);
                render_pass_in();
            }
        }
    }
}

CINO_INLINE
void DrawableHexmesh::render_pass_out() const
{
    if (draw_mode & DRAW_OUT_POINTS)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, outer_visible_coords.data());

        glColor3f(1.0, 1.0, 1.0);
        glDrawArrays(GL_POINTS, 0, outer_visible_coords.size() / 3);

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else if (draw_mode & DRAW_OUT_SMOOTH || draw_mode & DRAW_OUT_FLAT)
    {
        // Old fashioned (orrible) rendering
        //
        if (draw_mode & DRAW_OUT_FACECOLOR)
        {
            int n_quads = outer_visible_quads.size()/4;
            for(int qid=0; qid<n_quads; ++qid)
            {
                int qid_vtx_ptr  = 4 * qid;
                int qid_col_ptr  = 3 * qid;
                int vid0     = outer_visible_quads[qid_vtx_ptr + 0];
                int vid1     = outer_visible_quads[qid_vtx_ptr + 1];
                int vid2     = outer_visible_quads[qid_vtx_ptr + 2];
                int vid3     = outer_visible_quads[qid_vtx_ptr + 3];
                int vid0_ptr = 3 * vid0;
                int vid1_ptr = 3 * vid1;
                int vid2_ptr = 3 * vid2;
                int vid3_ptr = 3 * vid3;

                glBegin(GL_QUADS);
                glColor3fv(&(outer_visible_t_colors[qid_col_ptr]));
                glNormal3dv(&(outer_visible_v_norms[vid0_ptr]));
                glVertex3dv(&(outer_visible_coords[vid0_ptr]));
                glNormal3dv(&(outer_visible_v_norms[vid1_ptr]));
                glVertex3dv(&(outer_visible_coords[vid1_ptr]));
                glNormal3dv(&(outer_visible_v_norms[vid2_ptr]));
                glVertex3dv(&(outer_visible_coords[vid2_ptr]));
                glNormal3dv(&(outer_visible_v_norms[vid3_ptr]));
                glVertex3dv(&(outer_visible_coords[vid3_ptr]));
                glEnd();
            }
        }
        else
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_DOUBLE, 0, outer_visible_coords.data());

            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_DOUBLE, 0, outer_visible_v_norms.data());

            if (draw_mode & DRAW_OUT_TEXTURE1D)
            {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(1, GL_FLOAT, 0, outer_visible_f_values.data());
                glColor3f(1.0,1.0,1.0);
            }

            glDrawElements(GL_QUADS, outer_visible_quads.size(), GL_UNSIGNED_INT, outer_visible_quads.data());

            if (draw_mode & DRAW_OUT_TEXTURE1D)   glDisableClientState(GL_TEXTURE_COORD_ARRAY); else
            if (draw_mode & DRAW_OUT_VERTEXCOLOR) glDisableClientState(GL_COLOR_ARRAY);

            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }

    if (draw_mode & DRAW_OUT_WIREFRAME)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, outer_visible_coords.data());

        glLineWidth(wireframe_out_width);
        glColor4fv(wireframe_out_color);

        glDrawElements(GL_QUADS, outer_visible_quads.size(), GL_UNSIGNED_INT, outer_visible_quads.data());

        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

CINO_INLINE
void DrawableHexmesh::render_pass_in() const
{
    if (draw_mode & DRAW_IN_SMOOTH || draw_mode & DRAW_IN_FLAT)
    {
        // Old fashioned (orrible) rendering
        //
        if (draw_mode & DRAW_IN_FACECOLOR)
        {
            int n_quads = inner_slice_quads.size()/4;
            for(int qid=0; qid<n_quads; ++qid)
            {
                int qid_vtx_ptr  = 4 * qid;
                int qid_col_ptr  = 3 * qid;
                int vid0     = inner_slice_quads[qid_vtx_ptr + 0];
                int vid1     = inner_slice_quads[qid_vtx_ptr + 1];
                int vid2     = inner_slice_quads[qid_vtx_ptr + 2];
                int vid3     = inner_slice_quads[qid_vtx_ptr + 3];
                int vid0_ptr = 3 * vid0;
                int vid1_ptr = 3 * vid1;
                int vid2_ptr = 3 * vid2;
                int vid3_ptr = 3 * vid3;

                glBegin(GL_QUADS);
                glColor3fv(&(inner_slice_h_colors[qid_col_ptr]));
                glNormal3dv(&(inner_slice_v_norms[vid0_ptr]));
                glVertex3dv(&(inner_slice_coords[vid0_ptr]));
                glNormal3dv(&(inner_slice_v_norms[vid1_ptr]));
                glVertex3dv(&(inner_slice_coords[vid1_ptr]));
                glNormal3dv(&(inner_slice_v_norms[vid2_ptr]));
                glVertex3dv(&(inner_slice_coords[vid2_ptr]));
                glNormal3dv(&(inner_slice_v_norms[vid3_ptr]));
                glVertex3dv(&(inner_slice_coords[vid3_ptr]));
                glEnd();
            }
        }
        else
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_DOUBLE, 0, inner_slice_coords.data());

            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_DOUBLE, 0, inner_slice_v_norms.data());

            if (draw_mode & DRAW_IN_TEXTURE1D)
            {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(1, GL_FLOAT, 0, inner_slice_f_values.data());
                glColor3f(1.0,1.0,1.0);
            }

            glDrawElements(GL_QUADS, inner_slice_quads.size(), GL_UNSIGNED_INT, inner_slice_quads.data());

            if (draw_mode & DRAW_IN_TEXTURE1D)   glDisableClientState(GL_TEXTURE_COORD_ARRAY); else
            if (draw_mode & DRAW_IN_VERTEXCOLOR) glDisableClientState(GL_COLOR_ARRAY);

            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }

    if (draw_mode & DRAW_IN_WIREFRAME)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, inner_slice_coords.data());

        glLineWidth(wireframe_in_width);
        glColor4fv(wireframe_in_color);

        glDrawElements(GL_QUADS, inner_slice_quads.size(), GL_UNSIGNED_INT, inner_slice_quads.data());

        glDisableClientState(GL_VERTEX_ARRAY);
    }

}

CINO_INLINE
void DrawableHexmesh::set_h_out_color(const float r, const float g, const float b)
{
    h_out_colors.resize(num_hexahedra()*3);
    for(int hid=0; hid<num_hexahedra(); ++hid)
    {
        int hid_ptr = hid * 3;
        h_out_colors[hid_ptr + 0] = r;
        h_out_colors[hid_ptr + 1] = g;
        h_out_colors[hid_ptr + 2] = b;
    }
}

CINO_INLINE
void DrawableHexmesh::set_h_in_color(const float r, const float g, const float b)
{
    h_in_colors.resize(num_hexahedra()*3);
    for(int hid=0; hid<num_hexahedra(); ++hid)
    {
        int hid_ptr = hid * 3;
        h_in_colors[hid_ptr + 0] = r;
        h_in_colors[hid_ptr + 1] = g;
        h_in_colors[hid_ptr + 2] = b;
    }
}

CINO_INLINE
void DrawableHexmesh::set_slice_parameters(const float thresh, const int item, const bool dir, const bool mode)
{
    if (draw_mode & DRAW_IN_SLICE)
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
void DrawableHexmesh::update_slice(const bool mode)
{
    for(int tid=0; tid<num_hexahedra(); ++tid)
    {
        vec3d  c = hex_centroid(tid);
        double q = hex_quality(tid);
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
void DrawableHexmesh::update_inner_slice()
{
    inner_slice_coords.clear();
    inner_slice_quads.clear();
    inner_slice_v_norms.clear();
    inner_slice_f_values.clear();
    inner_slice_h_colors.clear();

    for(int hid=0; hid<num_hexahedra(); ++hid)
    {
        if ((draw_mode & DRAW_IN_SLICE) && (slice_mask[hid])) continue;

        for(auto nbr : adj_hexa2hexa(hid))
        {
            if (slice_mask[nbr])
            {
                int shared_f = shared_facet(hid, nbr);
                assert(shared_f > -1);

                int vid0     = hex_vertex_id(hid, HEXA_FACES[shared_f][0]);
                int vid1     = hex_vertex_id(hid, HEXA_FACES[shared_f][1]);
                int vid2     = hex_vertex_id(hid, HEXA_FACES[shared_f][2]);
                int vid3     = hex_vertex_id(hid, HEXA_FACES[shared_f][3]);
                int vid0_ptr = 3 * vid0;
                int vid1_ptr = 3 * vid1;
                int vid2_ptr = 3 * vid2;
                int vid3_ptr = 3 * vid3;

                int base_addr = inner_slice_quads.size();

                inner_slice_quads.push_back(base_addr);
                inner_slice_quads.push_back(base_addr + 1);
                inner_slice_quads.push_back(base_addr + 2);
                inner_slice_quads.push_back(base_addr + 3);

                if (draw_mode & DRAW_IN_EL_QUALITY)
                {
                    double q = hex_quality(hid);
                    float  r,g,b;

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

                    inner_slice_h_colors.push_back(r);
                    inner_slice_h_colors.push_back(g);
                    inner_slice_h_colors.push_back(b);
                }
                else
                {
                    int hid_ptr = hid * 3;
                    inner_slice_h_colors.push_back(h_in_colors[hid_ptr + 0]);
                    inner_slice_h_colors.push_back(h_in_colors[hid_ptr + 1]);
                    inner_slice_h_colors.push_back(h_in_colors[hid_ptr + 2]);
                }

                inner_slice_coords.push_back(coords[vid0_ptr + 0]);
                inner_slice_coords.push_back(coords[vid0_ptr + 1]);
                inner_slice_coords.push_back(coords[vid0_ptr + 2]);
                inner_slice_coords.push_back(coords[vid1_ptr + 0]);
                inner_slice_coords.push_back(coords[vid1_ptr + 1]);
                inner_slice_coords.push_back(coords[vid1_ptr + 2]);
                inner_slice_coords.push_back(coords[vid2_ptr + 0]);
                inner_slice_coords.push_back(coords[vid2_ptr + 1]);
                inner_slice_coords.push_back(coords[vid2_ptr + 2]);
                inner_slice_coords.push_back(coords[vid3_ptr + 0]);
                inner_slice_coords.push_back(coords[vid3_ptr + 1]);
                inner_slice_coords.push_back(coords[vid3_ptr + 2]);

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
                inner_slice_v_norms.push_back(n.x());
                inner_slice_v_norms.push_back(n.y());
                inner_slice_v_norms.push_back(n.z());

                inner_slice_f_values.push_back(vertex_u_text(vid0));
                inner_slice_f_values.push_back(vertex_u_text(vid1));
                inner_slice_f_values.push_back(vertex_u_text(vid2));
                inner_slice_f_values.push_back(vertex_u_text(vid3));
            }
        }
    }
}

CINO_INLINE
void DrawableHexmesh::update_outer_visible_mesh()
{
    outer_visible_coords.clear();
    outer_visible_quads.clear();
    outer_visible_v_norms.clear();
    outer_visible_f_values.clear();
    outer_visible_t_colors.clear();

    for(int qid=0; qid<num_srf_quads(); ++qid)
    {
        if ((draw_mode & DRAW_IN_SLICE) && (slice_mask[adj_quad2hexa(qid)])) continue;

        int qid_vid_ptr   = 4 * qid;
        int vid0          = quads[qid_vid_ptr + 0];
        int vid1          = quads[qid_vid_ptr + 1];
        int vid2          = quads[qid_vid_ptr + 2];
        int vid3          = quads[qid_vid_ptr + 3];
        int vid0_ptr      = 3 * vid0;
        int vid1_ptr      = 3 * vid1;
        int vid2_ptr      = 3 * vid2;
        int vid3_ptr      = 3 * vid3;

        int base_addr = outer_visible_quads.size();

        outer_visible_quads.push_back(base_addr);
        outer_visible_quads.push_back(base_addr + 1);
        outer_visible_quads.push_back(base_addr + 2);
        outer_visible_quads.push_back(base_addr + 3);

        outer_visible_coords.push_back(coords[vid0_ptr + 0]);
        outer_visible_coords.push_back(coords[vid0_ptr + 1]);
        outer_visible_coords.push_back(coords[vid0_ptr + 2]);
        outer_visible_coords.push_back(coords[vid1_ptr + 0]);
        outer_visible_coords.push_back(coords[vid1_ptr + 1]);
        outer_visible_coords.push_back(coords[vid1_ptr + 2]);
        outer_visible_coords.push_back(coords[vid2_ptr + 0]);
        outer_visible_coords.push_back(coords[vid2_ptr + 1]);
        outer_visible_coords.push_back(coords[vid2_ptr + 2]);
        outer_visible_coords.push_back(coords[vid3_ptr + 0]);
        outer_visible_coords.push_back(coords[vid3_ptr + 1]);
        outer_visible_coords.push_back(coords[vid3_ptr + 2]);

        int qid_norm_ptr = 3 * qid;
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 0]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 1]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 2]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 0]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 1]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 2]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 0]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 1]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 2]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 0]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 1]);
        outer_visible_v_norms.push_back(q_norm[qid_norm_ptr + 2]);

        outer_visible_f_values.push_back(vertex_u_text(vid0));
        outer_visible_f_values.push_back(vertex_u_text(vid1));
        outer_visible_f_values.push_back(vertex_u_text(vid2));
        outer_visible_f_values.push_back(vertex_u_text(vid3));

        if (draw_mode & DRAW_OUT_EL_QUALITY)
        {
            int    hid = adj_quad2hexa(qid);
            double q   = hex_quality(hid);

            float r,g,b;

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

            outer_visible_t_colors.push_back(r);
            outer_visible_t_colors.push_back(g);
            outer_visible_t_colors.push_back(b);
        }
        else
        {
            int hid_ptr = adj_quad2hexa(qid) * 3;
            outer_visible_t_colors.push_back(h_out_colors[hid_ptr + 0]);
            outer_visible_t_colors.push_back(h_out_colors[hid_ptr + 1]);
            outer_visible_t_colors.push_back(h_out_colors[hid_ptr + 2]);
        }
    }
}

CINO_INLINE
void DrawableHexmesh::set_out_wireframe_color(float r, float g, float b)
{
    wireframe_out_color[0] = r;
    wireframe_out_color[1] = g;
    wireframe_out_color[2] = b;
}

CINO_INLINE
void DrawableHexmesh::set_in_wireframe_color(float r, float g, float b)
{
    wireframe_in_color[0] = r;
    wireframe_in_color[1] = g;
    wireframe_in_color[2] = b;
}

CINO_INLINE
void DrawableHexmesh::set_out_wireframe_width(float width)
{
    wireframe_out_width = width;
}

CINO_INLINE
void DrawableHexmesh::set_in_wireframe_width(float width)
{
    wireframe_in_width = width;
}

CINO_INLINE
void DrawableHexmesh::set_out_wireframe_transparency(float alpha)
{
    wireframe_out_color[3] = alpha;
}

CINO_INLINE
void DrawableHexmesh::set_in_wireframe_transparency(float alpha)
{
    wireframe_in_color[3] = alpha;
}

CINO_INLINE
void DrawableHexmesh::set_draw_mesh(bool b)
{
    if (b) draw_mode |=  DRAW_MESH;
    else   draw_mode &= ~DRAW_MESH;
}

CINO_INLINE
void DrawableHexmesh::set_out_wireframe(bool b)
{
    if (b) draw_mode |=  DRAW_OUT_WIREFRAME;
    else   draw_mode &= ~DRAW_OUT_WIREFRAME;
}

CINO_INLINE
void DrawableHexmesh::set_in_wireframe(bool b)
{
    if (b) draw_mode |=  DRAW_IN_WIREFRAME;
    else   draw_mode &= ~DRAW_IN_WIREFRAME;
}

CINO_INLINE
void DrawableHexmesh::set_flat_shading()
{
    draw_mode |=  DRAW_OUT_FLAT;
    draw_mode &= ~DRAW_OUT_SMOOTH;
    draw_mode &= ~DRAW_OUT_POINTS;
}

CINO_INLINE
void DrawableHexmesh::set_smooth_shading()
{
    draw_mode |=  DRAW_OUT_SMOOTH;
    draw_mode &= ~DRAW_OUT_FLAT;
    draw_mode &= ~DRAW_OUT_POINTS;
}

CINO_INLINE
void DrawableHexmesh::set_points_shading()
{
    draw_mode |=  DRAW_OUT_POINTS;
    draw_mode &= ~DRAW_OUT_FLAT;
    draw_mode &= ~DRAW_OUT_SMOOTH;
}

CINO_INLINE
void DrawableHexmesh::set_draw_slice(bool b)
{
    if (b) draw_mode |=  DRAW_IN_SLICE;
    else   draw_mode &= ~DRAW_IN_SLICE;
}

CINO_INLINE
void DrawableHexmesh::set_enable_out_face_color()
{
    draw_mode |=  DRAW_OUT_FACECOLOR;
    draw_mode &= ~DRAW_OUT_VERTEXCOLOR;
    draw_mode &= ~DRAW_OUT_TEXTURE1D;
    draw_mode &= ~DRAW_OUT_EL_QUALITY;
    glDisable(GL_TEXTURE_1D);
}


CINO_INLINE
void DrawableHexmesh::set_enable_in_face_color()
{
    draw_mode |=  DRAW_IN_FACECOLOR;
    draw_mode &= ~DRAW_IN_VERTEXCOLOR;
    draw_mode &= ~DRAW_IN_TEXTURE1D;
    draw_mode &= ~DRAW_IN_EL_QUALITY;
    glDisable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableHexmesh::set_enable_out_quality_color()
{
    draw_mode |=  DRAW_OUT_EL_QUALITY;
    draw_mode |=  DRAW_OUT_FACECOLOR;
    draw_mode &= ~DRAW_OUT_VERTEXCOLOR;
    draw_mode &= ~DRAW_OUT_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableHexmesh::set_enable_in_quality_color()
{
    draw_mode |=  DRAW_IN_EL_QUALITY;
    draw_mode |=  DRAW_IN_FACECOLOR;
    draw_mode &= ~DRAW_IN_VERTEXCOLOR;
    draw_mode &= ~DRAW_IN_TEXTURE1D;
    glDisable(GL_TEXTURE_1D);
}

CINO_INLINE
void DrawableHexmesh::set_enable_out_texture1D(int texture)
{
    draw_mode |=  DRAW_OUT_TEXTURE1D;
    draw_mode &= ~DRAW_OUT_VERTEXCOLOR;
    draw_mode &= ~DRAW_OUT_FACECOLOR;

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
void DrawableHexmesh::set_enable_in_texture1D(int texture)
{
    draw_mode |=  DRAW_IN_TEXTURE1D;
    draw_mode &= ~DRAW_IN_VERTEXCOLOR;
    draw_mode &= ~DRAW_IN_FACECOLOR;

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
void DrawableHexmesh::color_wrt_hex_scalar()
{
    h_out_colors.resize(num_hexahedra()*3);

    int n_colors = *max_element(t_label.begin(), t_label.end()) + 1;

    for(int hid=0; hid<num_hexahedra(); ++hid)
    {
        float rgb[3];
        scattered_color(n_colors, hex_label(hid), rgb);

        int hid_ptr = hid * 3;
        h_out_colors[hid_ptr + 0] = rgb[0];
        h_out_colors[hid_ptr + 1] = rgb[1];
        h_out_colors[hid_ptr + 2] = rgb[2];

        h_in_colors[hid_ptr + 0] = rgb[0];
        h_in_colors[hid_ptr + 1] = rgb[1];
        h_in_colors[hid_ptr + 2] = rgb[2];
    }
    update_slice();
}

}
