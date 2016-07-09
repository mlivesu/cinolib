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
#include "integral_curve.h"
#include "gl/draw_cylinder.h"
#include "gl/draw_arrow.h"
#include "colors.h"
#include "intersection.h"
#include "trimesh/drawable_trimesh.h"

namespace cinolib
{

template<class Mesh>
CINO_INLINE
IntegralCurve<Mesh>::IntegralCurve(const Mesh        & m,
                                   const VectorField & grad,
                                   const int           source_tid,
                                   const vec3d       & source_pos)
    : m_ptr(&m)
    , grad_ptr(&grad)
{
    type = INTEGRAL_CURVE;

    opt.source_tid = source_tid;
    opt.source_pos = source_pos;
    opt.convergence_criterion = STOP_AT_LOCAL_MAX;

    make_curve();
}


template<class Mesh>
CINO_INLINE
IntegralCurve<Mesh>::IntegralCurve(const Mesh        & m,
                                   const VectorField & grad,
                                   const int           source_tid,
                                   const vec3d       & source_pos,
                                   const float         stop_at_this_value)
    : m_ptr(&m)
    , grad_ptr(&grad)
{
    type = INTEGRAL_CURVE;

    opt.source_tid            = source_tid;
    opt.source_pos            = source_pos;
    opt.convergence_criterion = STOP_AT_GIVEN_VAL;
    opt.stop_at_this_value    = stop_at_this_value;

    make_curve();
}

template<class Mesh>
CINO_INLINE
IntegralCurve<Mesh>::IntegralCurve(const Mesh        & m,
                                   const VectorField & grad,
                                   const int           source_tid,
                                   const vec3d       & source_pos,
                                   const int           stop_at_this_vertex)
    : m_ptr(&m)
    , grad_ptr(&grad)
{
    type = INTEGRAL_CURVE;

    opt.source_tid            = source_tid;
    opt.source_pos            = source_pos;
    opt.convergence_criterion = STOP_AT_GIVEN_VTX;
    opt.stop_at_this_vertex   = stop_at_this_vertex; // if you ever run into it....

    make_curve();
}


template<class Mesh>
CINO_INLINE
void IntegralCurve<Mesh>::make_curve()
{
    int   curr_tid = opt.source_tid;
    vec3d curr_pos = opt.source_pos;
    CurveSample cs;
    cs.pos = curr_pos;
    cs.fid = curr_tid;
    cs.eid = -1;
    curve_samples.push_back(cs);

    do
    {
        int   next_tid;
        vec3d next_pos;
        traverse_element(curr_tid, curr_pos, next_tid, next_pos);
        curr_tid = next_tid;
        if ((curr_pos - next_pos).length() > 0)
        {
            curr_pos = next_pos;

            CurveSample cs;
            cs.pos = curr_pos;
            cs.fid = curr_tid;
            cs.eid = -1;
            curve_samples.push_back(cs);
        }
    }
    while (!is_converged(curr_tid, STOP_AT_LOCAL_MAX) && // in any case you can't go any further...
           !is_converged(curr_tid, opt.convergence_criterion));

    // append the final segment to the curve
    //
    if (is_converged(curr_tid, STOP_AT_LOCAL_MAX))
    {
        for(int i=0; i<3; ++i)
        if (m_ptr->vertex_is_local_maxima(m_ptr->triangle_vertex_id(curr_tid,i)))
        {
            CurveSample cs;
            cs.pos = m_ptr->triangle_vertex(curr_tid,i);
            cs.fid = curr_tid;
            cs.eid = -1;
            curve_samples.push_back(cs);
        }
    }
}


template<>
CINO_INLINE
void IntegralCurve<Trimesh>::find_exit_door(const int     tid,
                                            const vec3d & pos,
                                            const vec3d & target_dir,
                                                  int   & exit_edge,
                                                  int   & exit_backup) const
{
    /* Split the triangle into three sub triangles meeting at <pos>,
     * then find, among the three edges incident to <pos>, the one
     * closest to the target dir. The exit_edge will be either at the
     * left or at the right of such edge. To understand on which side,
     * compare the cross between edge and target dir with the triangle
     * normal. That's it
     *
     * NOTE: the exit backup serves to handle the corner case described
     * in handle_corner_case<Trimesh>()
    */

    vec3d uvw[3] =
    {
        m_ptr->triangle_vertex(tid,0) - pos,
        m_ptr->triangle_vertex(tid,1) - pos,
        m_ptr->triangle_vertex(tid,2) - pos
    };
    uvw[0].normalize();
    uvw[1].normalize();
    uvw[2].normalize();

    int    vert      = -1;
    double min_angle = FLT_MAX;
    for(int i=0; i<3; ++i)
    {
        double alpha = acos( uvw[i].dot(target_dir) );

        if (alpha < min_angle)
        {
            min_angle = alpha;
            vert      = i;
        }
    }
    assert(min_angle < FLT_MAX);

    vec3d cross = target_dir.cross(uvw[vert]);
    exit_edge   = vert;
    exit_backup = (vert+2)%3;
    if (cross.dot(m_ptr->triangle_normal(tid)) >= 0) std::swap(exit_edge, exit_backup);
}


template<>
CINO_INLINE
void IntegralCurve<Trimesh>::handle_corner_cases(const int     curr_tid,
                                                 const vec3d & A,
                                                 const vec3d & B,
                                                 const int     exit_backup,
                                                       int   & next_tid,
                                                       vec3d & next_pos) const
{
    // CORNER CASE: due to the piece-wise constant form of the gradient, it may
    // happen that the entry edge and the exit edge of a triangle coincide. This
    // usually happens close to singularities, where the field skins into.
    // In such cases, the right thing to do is to flow along the edge

    vec3d next_target_dir = grad_ptr->vec_at(next_tid);
    next_target_dir.normalize();

    int next_exit_edge, next_exit_backup;
    find_exit_door(next_tid, next_pos, next_target_dir, next_exit_edge, next_exit_backup);

    int next_vidA     = m_ptr->triangle_vertex_id(next_tid, TRI_EDGES[next_exit_edge][0]);
    int next_vidB     = m_ptr->triangle_vertex_id(next_tid, TRI_EDGES[next_exit_edge][1]);
    int next_next_tid = m_ptr->triangle_adjacent_along(next_tid, next_vidA, next_vidB);

    if (next_next_tid == curr_tid) // if I return to myself...
    {
        next_exit_edge = exit_backup;
        next_pos       = ((B-A).dot(next_target_dir)>0) ? B : A;
    }
}


template<>
CINO_INLINE
void IntegralCurve<Trimesh>::traverse_element(const int     tid,
                                              const vec3d & pos,
                                                    int   & next_tid,
                                                    vec3d & next_pos) const
{
    vec3d target_dir = grad_ptr->vec_at(tid);
    target_dir.normalize();

    int exit_edge, exit_backup;
    find_exit_door(tid, pos, target_dir, exit_edge, exit_backup);

    int    vidA  = m_ptr->triangle_vertex_id(tid, TRI_EDGES[exit_edge][0]);
    int    vidB  = m_ptr->triangle_vertex_id(tid, TRI_EDGES[exit_edge][1]);
    vec3d  A     = m_ptr->vertex(vidA);
    vec3d  B     = m_ptr->vertex(vidB);

    vec3d  V0      = pos;
    vec3d  V1      = A;
    vec3d  e0_dir  = B - A; e0_dir.normalize();
    vec3d  e1_dir  = -target_dir;
    vec3d  e2_dir  = V1 - V0; e2_dir.normalize();

    double V0_ang  = acos(e2_dir.dot(-e1_dir));
    double V2_ang  = acos(e1_dir.dot(-e0_dir));
    double e2_len  = (V1 - V0).length();
    double e0_len  = sin(V0_ang) * e2_len / sin(V2_ang);

    next_pos       = V1 + e0_len * e0_dir;
    next_tid       = m_ptr->triangle_adjacent_along(tid, vidA, vidB);

    handle_corner_cases(tid, A, B, exit_backup, next_tid, next_pos);
}


template<class Mesh>
CINO_INLINE
void IntegralCurve<Mesh>::draw() const
{
    double cylind_rad = m_ptr->bbox().diag()*0.001;

    for(size_t i=1; i<curve_samples.size(); ++i)
    {
        cylinder<vec3d>(curve_samples[i-1].pos, curve_samples[i].pos, cylind_rad, cylind_rad, RED);
        //arrow<vec3d>(curve_samples[i-1].pos, curve_samples[i].pos, cylind_rad, RED);
    }
}

template <>
CINO_INLINE
bool IntegralCurve<Trimesh>::is_converged(const int curr_tid, const int convergence_criterion) const
{
    switch (convergence_criterion)
    {
        case STOP_AT_LOCAL_MAX :
        {
            for(int i=0; i<3; ++i)
            {
                int vid = m_ptr->triangle_vertex_id(curr_tid,i);
                if (m_ptr->vertex_is_local_maxima(vid)) return true;
            }
        }
        break;

        case STOP_AT_GIVEN_VAL :
        {
            if (m_ptr->triangle_min_u_text(curr_tid) > opt.stop_at_this_value) return true;
        }
        break;

        case STOP_AT_GIVEN_VTX :
        {
            if (m_ptr->triangle_contains_vertex(curr_tid, opt.stop_at_this_vertex)) return true;
        }
        break;

        default: assert("Integral Curve Convergence Error" && false);
    }

    return false;
}

}
