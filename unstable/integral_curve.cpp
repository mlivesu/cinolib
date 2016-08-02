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
#include <cinolib/unstable/integral_curve.h>
#include <cinolib/gl/draw_cylinder.h>
#include <cinolib/gl/draw_arrow.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/colors.h>
#include <cinolib/unstable/intersection.h>
#include <cinolib/meshes/trimesh/drawable_trimesh.h>
#include <cinolib/meshes/tetmesh/drawable_tetmesh.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/geometry/segment.h>
#include <cinolib/geometry/tetrahedron.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
IntegralCurve<Mesh>::IntegralCurve(const Mesh        & m,
                                   const VectorField & grad,
                                   const int           source_tid,
                                   const int           source_vid)
    : DrawableCurve()
    , m_ptr(&m)
    , grad_ptr(&grad)
{
    type = CURVE;

    opt.source_tid            = source_tid;
    opt.source_vid            = source_vid;
    opt.source_pos            = m.vertex(source_vid);
    opt.convergence_criterion = STOP_AT_LOCAL_MAX;

    make_curve();
    for(CurveSample & s : curve) append_sample(s.pos);
}


template<class Mesh>
CINO_INLINE
IntegralCurve<Mesh>::IntegralCurve(const Mesh        & m,
                                   const VectorField & grad,
                                   const int           source_tid,
                                   const int           source_vid,
                                   const float         stop_at_this_value)
    : DrawableCurve()
    , m_ptr(&m)
    , grad_ptr(&grad)
{
    type = CURVE;

    opt.source_tid            = source_tid;
    opt.source_vid            = source_vid;
    opt.source_pos            = m.vertex(source_vid);
    opt.convergence_criterion = STOP_AT_GIVEN_VAL;
    opt.stop_at_this_value    = stop_at_this_value;

    make_curve();
    for(CurveSample & s : curve) append_sample(s.pos);
}

template<class Mesh>
CINO_INLINE
IntegralCurve<Mesh>::IntegralCurve(const Mesh        & m,
                                   const VectorField & grad,
                                   const int           source_tid,
                                   const int           source_vid,
                                   const int           stop_at_this_vertex)
    : DrawableCurve()
    , m_ptr(&m)
    , grad_ptr(&grad)
{
    type = CURVE;

    opt.source_tid            = source_tid;
    opt.source_vid            = source_vid;
    opt.source_pos            = m.vertex(source_vid);
    opt.convergence_criterion = STOP_AT_GIVEN_VTX;
    opt.stop_at_this_vertex   = stop_at_this_vertex; // if you ever run into it....

    make_curve();
    for(CurveSample & s : curve) append_sample(s.pos);
}


/**********************************
 *
 * TRIMESH TEMPLATE SPECIALIZATIONS
 *
************************************/


template<>
CINO_INLINE
void IntegralCurve<Trimesh>::find_exit_gate(const CurveSample & sample,
                                            const vec3d       & dir,
                                                  int         & exit_gate,
                                                  vec3d       & exit_pos) const
{
    assert(sample.elem_id >= 0);
    assert(sample.elem_id <  m_ptr->num_triangles());

    vec3d tri[3] =
    {
        m_ptr->triangle_vertex(sample.elem_id, 0),
        m_ptr->triangle_vertex(sample.elem_id, 1),
        m_ptr->triangle_vertex(sample.elem_id, 2),
    };

    triangle_traverse_with_ray(tri, sample.pos, dir, exit_pos, exit_gate);
}



template<>
CINO_INLINE
bool IntegralCurve<Trimesh>::gradient_skins_into(const CurveSample & curr,
                                                 const CurveSample & next) const
{
    if (next.elem_id == -1) return false;

    vec3d grad_dir = grad_ptr->vec_at(next.elem_id);
    grad_dir.normalize();

    int   next_exit_edge;
    vec3d next_exit_pos;
    find_exit_gate(next, grad_dir, next_exit_edge, next_exit_pos);

    int  next_vidA     = m_ptr->triangle_vertex_id(next.elem_id, TRI_EDGES[next_exit_edge][0]);
    int  next_vidB     = m_ptr->triangle_vertex_id(next.elem_id, TRI_EDGES[next_exit_edge][1]);
    int  next_next_tid = m_ptr->triangle_adjacent_along(next.elem_id, next_vidA, next_vidB);

    if (next_next_tid == curr.elem_id) return true;
    return false;
}


template<>
CINO_INLINE
IntegralCurve<Trimesh>::CurveSample IntegralCurve<Trimesh>::traverse_element(const CurveSample & sample)
{
    vec3d grad_dir = grad_ptr->vec_at(sample.elem_id);
    grad_dir.normalize();

    CurveSample next;
    find_exit_gate(sample, grad_dir, next.gate_id, next.pos);

    int  vidA     = m_ptr->triangle_vertex_id(sample.elem_id, TRI_EDGES[next.gate_id][0]);
    int  vidB     = m_ptr->triangle_vertex_id(sample.elem_id, TRI_EDGES[next.gate_id][1]);
    int  next_tid = m_ptr->triangle_adjacent_along(sample.elem_id, vidA, vidB);
    next.elem_id  = next_tid;

    if (gradient_skins_into(sample, next))
    {
        // follow the exit edge until the endpoint best aligned
        // with the gradient dir...
        //
        vec3d A      = m_ptr->vertex(vidA);
        vec3d B      = m_ptr->vertex(vidB);
        next.vert_id = ((B-A).dot(grad_dir)>0) ? vidB : vidA;
        next.pos     = m_ptr->vertex(next.vert_id);
        next.gate_id = -1;
        next.elem_id = -1;
        for(int tid : m_ptr->adj_vtx2tri(next.vert_id))
        {
            if (tid != next_tid && tid != sample.elem_id)
            {
                next.elem_id = tid;
            }
        }
        assert(next.elem_id != -1);
    }

    return next;
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


template<>
CINO_INLINE
void IntegralCurve<Trimesh>::make_curve()
{
    CurveSample cs;
    cs.pos     = opt.source_pos;
    cs.elem_id = opt.source_tid;
    cs.vert_id = opt.source_vid;
    curve.push_back(cs);

    bool border_reached, locmax_reached, target_reached;

    do
    {
        curve.push_back( traverse_element(curve.back()) );

        border_reached = (curve.back().elem_id == -1);
        locmax_reached = !border_reached && is_converged(curve.back().elem_id, STOP_AT_LOCAL_MAX);
        target_reached = !border_reached && is_converged(curve.back().elem_id, opt.convergence_criterion);
    }
    while (!border_reached && !locmax_reached && !target_reached);

    if (locmax_reached)
    {
        // append the final segment to the curve
        //
        for(int i=0; i<3; ++i)
        if (m_ptr->vertex_is_local_maxima(m_ptr->triangle_vertex_id(curve.back().elem_id,i)))
        {
            CurveSample cs;
            cs.vert_id = m_ptr->triangle_vertex_id(curve.back().elem_id, i);
            cs.pos     = m_ptr->vertex(cs.vert_id);
            cs.elem_id = curve.back().elem_id;
            curve.push_back(cs);
        }
    }
}


/**********************************
 *
 * TETMESH TEMPLATE SPECIALIZATIONS
 *
************************************/

template<>
CINO_INLINE
void IntegralCurve<Tetmesh>::find_exit_gate(const CurveSample & sample,
                                           const vec3d        & dir,
                                                 int          & exit_gate,
                                                 vec3d        & exit_pos) const
{
    assert(sample.elem_id >= 0);
    assert(sample.elem_id < m_ptr->num_tetrahedra());

    exit_pos  = sample.pos;
    exit_gate = -1;

    for(int facet=0; facet<4; ++facet)
    {
        vec3d f[3] =
        {
            m_ptr->tet_vertex(sample.elem_id, TET_FACES[facet][0]),
            m_ptr->tet_vertex(sample.elem_id, TET_FACES[facet][1]),
            m_ptr->tet_vertex(sample.elem_id, TET_FACES[facet][2])
        };

        vec3d inters;
        bool b = intersection::ray_triangle_intersection(sample.pos, dir, f[0], f[1], f[2], inters);

        if (b && inters.dist(sample.pos) > exit_pos.dist(sample.pos))
        {
            exit_pos  = inters;
            exit_gate = facet;
        }
    }

    if (exit_gate == -1)
    {
        exit_gate = sample.gate_id;
        exit_pos  = sample.pos;
    }
}


template<>
CINO_INLINE
IntegralCurve<Tetmesh>::CurveSample IntegralCurve<Tetmesh>::traverse_element(const CurveSample & curr)
{
    vec3d grad_dir = grad_ptr->vec_at(curr.elem_id);
    grad_dir.normalize();

    int   exit_gate;
    vec3d exit_pos;
    find_exit_gate(curr, grad_dir, exit_gate, exit_pos);

    if (exit_pos.dist(curr.pos) > 0)
    {
        CurveSample next;
        next.elem_id = m_ptr->adjacent_tet_through_facet(curr.elem_id, exit_gate);
        next.gate_id = m_ptr->shared_facet(next.elem_id, curr.elem_id);
        next.pos     = exit_pos;
        return next;
    }
    else
    {
        int tet_vid[4] =
        {
            m_ptr->tet_vertex_id(curr.elem_id, 0),
            m_ptr->tet_vertex_id(curr.elem_id, 1),
            m_ptr->tet_vertex_id(curr.elem_id, 2),
            m_ptr->tet_vertex_id(curr.elem_id, 3),
        };

        vec3d tet_xyz[4] =
        {
            m_ptr->tet_vertex(curr.elem_id, 0),
            m_ptr->tet_vertex(curr.elem_id, 1),
            m_ptr->tet_vertex(curr.elem_id, 2),
            m_ptr->tet_vertex(curr.elem_id, 3),
        };

        std::vector<int> elem_list;

        int    id;
        double dist;
        tet_closest_vertex(tet_xyz[0], tet_xyz[1], tet_xyz[2], tet_xyz[3], curr.pos, id, dist);

        if (dist < 1e-7) // we are at a vertex
        {
            int vid = tet_vid[id];
            for(int tid : m_ptr->adj_vtx2tet(vid)) elem_list.push_back(tid);
        }
        else
        {
            tet_closest_edge(tet_xyz[0], tet_xyz[1], tet_xyz[2], tet_xyz[3], curr.pos, id, dist);

            if (dist < 1e-7) // we are at an edge
            {
                int vidA = tet_vid[TET_EDGES[id][0]];
                int vidB = tet_vid[TET_EDGES[id][1]];
                int eid  = m_ptr->tet_edge_id(curr.elem_id, vidA, vidB);
                for(int tid : m_ptr->adj_edg2tet(eid)) elem_list.push_back(tid);
            }
            else // we are at a triangle
            {
                assert(curr.elem_id != -1);
                assert(curr.gate_id != -1);
                elem_list.push_back(curr.elem_id);
                int opp = m_ptr->adjacent_tet_through_facet(curr.elem_id, curr.gate_id);
                if (opp!=-1) elem_list.push_back(opp);
            }
        }

        assert(!elem_list.empty());

        vec3d avg_grad;
        for(int tid : elem_list)
        {
            avg_grad += grad_ptr->vec_at(tid);
        }
        avg_grad.normalize();

        for(int tid : elem_list)
        {
            CurveSample test;
            test.elem_id   = tid;
            test.pos       = curr.pos;

            vec3d exit_pos;
            int   exit_gate;
            find_exit_gate(test, avg_grad, exit_gate, exit_pos);

            if (curr.pos.dist(exit_pos) > 0)
            {
                CurveSample next;
                next.elem_id = m_ptr->adjacent_tet_through_facet(tid, exit_gate);
                next.gate_id = m_ptr->shared_facet(next.elem_id, tid);
                next.pos     = exit_pos;
                return next;
            }
        }

        std::cerr << "Mmmmm - this is still not perfect!" << std::endl;

        CurveSample next;
        next.pos = exit_pos;
        return next;
    }
}


template <>
CINO_INLINE
bool IntegralCurve<Tetmesh>::is_converged(const int curr_tid, const int convergence_criterion) const
{
    switch (convergence_criterion)
    {
        case STOP_AT_LOCAL_MAX :
        {
            for(int i=0; i<4; ++i)
            {
                int vid = m_ptr->tet_vertex_id(curr_tid,i);
                if (m_ptr->vertex_is_local_maxima(vid)) return true;
            }
        }
        break;

        case STOP_AT_GIVEN_VAL :
        {
            if (m_ptr->tet_min_u_text(curr_tid) > opt.stop_at_this_value) return true;
        }
        break;

        case STOP_AT_GIVEN_VTX :
        {
            if (m_ptr->tet_contains_vertex(curr_tid, opt.stop_at_this_vertex)) return true;
        }
        break;

        default: assert("Integral Curve Convergence Error" && false);
    }

    return false;
}


template<>
CINO_INLINE
void IntegralCurve<Tetmesh>::make_curve()
{
    CurveSample cs;
    cs.pos = opt.source_pos;
    cs.elem_id = opt.source_tid;
    cs.vert_id = opt.source_vid;
    curve.push_back(cs);

    bool border_reached, locmax_reached, target_reached;

    do
    {
        curve.push_back( traverse_element(curve.back()) );        

        border_reached = (curve.back().elem_id == -1);
        locmax_reached = !border_reached && is_converged(curve.back().elem_id, STOP_AT_LOCAL_MAX);
        target_reached = !border_reached && is_converged(curve.back().elem_id, opt.convergence_criterion);
    }
    while (!border_reached && !locmax_reached && !target_reached);

    // append the final segment to the curve
    //
    if (locmax_reached)
    {
        for(int i=0; i<4; ++i)
        if (m_ptr->vertex_is_local_maxima(m_ptr->tet_vertex_id(curve.back().elem_id,i)))
        {
            CurveSample cs;
            cs.pos = m_ptr->tet_vertex(curve.back().elem_id,i);
            cs.elem_id = curve.back().elem_id;
            cs.gate_id = -1;
            curve.push_back(cs);
        }
    }
}

}
