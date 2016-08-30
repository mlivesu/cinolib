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

#include <cinolib/gl/draw_cylinder.h>
#include <cinolib/gl/draw_arrow.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/colors.h>
#include <cinolib/intersection.h>
#include <cinolib/meshes/trimesh/drawable_trimesh.h>
#include <cinolib/meshes/tetmesh/drawable_tetmesh.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/geometry/segment.h>
#include <cinolib/geometry/ray.h>
#include <cinolib/geometry/tetrahedron.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
IntegralCurve<Mesh>::IntegralCurve(const Mesh                & m,
                                   const VectorField         & grad,
                                   const int                   tid,
                                   const std::vector<double> & bary)
    : DrawableCurve()
    , m_ptr(&m)
    , grad_ptr(&grad)
{
    Sample seed;
    seed.tid  = tid;
    seed.bary = bary;
    for(int off=0; off<m.verts_per_element; ++off) seed.pos += bary.at(off) * m.elem_vertex(tid, off);

    trace_curve(seed);
}

template<class Mesh>
CINO_INLINE
IntegralCurve<Mesh>::IntegralCurve(const Mesh        & m,
                                   const VectorField & grad,
                                   const int           vid)
    : DrawableCurve()
    , m_ptr(&m)
    , grad_ptr(&grad)
{
    assert(!m.adj_vtx2ele(vid).empty());
    int tid = m.adj_vtx2ele(vid).front();

    int off = 0;
    while(off<m.verts_per_element && m.elem_vertex_id(tid,off)!=vid) ++off;
    assert(m.elem_vertex_id(tid,off)==vid && "Cannot find incident element to begin with");

    std::vector<double> bary = std::vector<double>(m.verts_per_element,0);
    bary[off] = 1.0;

    Sample seed;
    seed.pos  = m.vertex(vid);
    seed.tid  = tid;
    seed.bary = bary;

    trace_curve(seed);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void IntegralCurve<Mesh>::trace_curve(const Sample & seed)
{
    append_sample(seed);

    do
    {
        Sample s = move_forward(samples().back());
        append_sample(s);
    }
    while (!is_converged(samples().back()));
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
bool IntegralCurve<Mesh>::is_converged(const Sample & sample)
{
    if (sample.tid == -1) // i.e. boundary reached. Remove dummy sample
    {
        pop_back();
        return true;
    }

    // i.e. local maxima reached. Connect to it
    // WARNING: there can be many! This picks up an arbitrary one...
    //
    for(int off=0; off<m_ptr->verts_per_element; ++off)
    {
        int vid = m_ptr->elem_vertex_id(sample.tid, off);

        if (m_ptr->vertex_is_local_maxima(vid))
        {
            Sample s;
            s.pos  = m_ptr->vertex(vid);
            s.tid  = sample.tid;
            s.bary = std::vector<double>(m_ptr->verts_per_element,0);
            s.bary.at(off) = 1.0;
            append_sample(s);
            return true;
        }
    }

    if ((int)samples().size() >= m_ptr->num_vertices())
    {
        std::cerr << "INTEGRAL CURVE ERROR - Infinite loop. Something is REALLY off here!" << std::endl;
        return true;
    }

    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
bool IntegralCurve<Mesh>::is_on_vertex(const Sample & s, int & vid, const double tol) const
{
    assert(s.tid!=-1);

    std::vector<int> non_zero_coords;
    for(size_t off=0; off<s.bary.size(); ++off)
    {
        if (s.bary.at(off) > tol) non_zero_coords.push_back(off);
    }

    if (non_zero_coords.size() == 1)
    {
        vid = m_ptr->elem_vertex_id(s.tid, non_zero_coords.front());
        return true;
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
bool IntegralCurve<Mesh>::is_on_edge(const Sample & s, int & eid, const double tol) const
{
    assert(s.tid!=-1);

    std::vector<int> non_zero_coords;
    for(size_t off=0; off<s.bary.size(); ++off)
    {
        if (s.bary.at(off) > tol) non_zero_coords.push_back(off);
    }

    if (non_zero_coords.size() == 2)
    {
        int vid0 = m_ptr->elem_vertex_id(s.tid, non_zero_coords.at(0));
        int vid1 = m_ptr->elem_vertex_id(s.tid, non_zero_coords.at(1));
        for(int id : m_ptr->adj_ele2edg(s.tid))
        {
            if (m_ptr->edge_contains_vertex(id, vid0) &&
                m_ptr->edge_contains_vertex(id, vid1))
            {
                eid = id;
                return true;
            }
        }
        assert(false && "Integral curve - Something is off here...");
    }
    return false;
}

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::                                              :::::::::::::::::::: //
// :::::::::::::::::::: TEMPLATE SPECIALIZATIONS FOR TRIANGLE MESHES :::::::::::::::::::: //
// ::::::::::::::::::::                                              :::::::::::::::::::: //
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<>
CINO_INLINE
bool IntegralCurve<Trimesh>::is_on_face(const Sample & s, int & tid, const double tol) const
{
    assert(s.tid!=-1);

    std::vector<int> non_zero_coords;
    for(size_t off=0; off<s.bary.size(); ++off)
    {
        if (s.bary.at(off) > tol) non_zero_coords.push_back(off);
    }

    if (non_zero_coords.size() == 3)
    {
        tid = s.tid;
        return true;
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::Sample IntegralCurve<Trimesh>::move_forward_from_vertex(const int vid)
{
    vec3d v = m_ptr->vertex(vid);
    vec3d n = m_ptr->vertex_normal(vid);
    Plane tangent_plane(v,n);

    vec3d grad(0,0,0);
    for(int tid : m_ptr->adj_vtx2tri(vid)) grad += grad_ptr->vec_at(tid);
    grad = tangent_plane.project_onto(v+grad) - v;
    grad.normalize();
    assert(grad.length() > 0);

    std::map<int,vec3d> tangent_space;
    for(int nbr : m_ptr->adj_vtx2vtx(vid))
    {
        tangent_space[nbr] = tangent_plane.project_onto(m_ptr->vertex(nbr));
    }
    tangent_space[vid] = m_ptr->vertex(vid);

    for(int tid : m_ptr->adj_vtx2tri(vid))
    {
        int     eid = m_ptr->edge_opposite_to(tid, vid);
        vec3d   e0  = tangent_space.at( m_ptr->edge_vertex_id(eid,0) );
        vec3d   e1  = tangent_space.at( m_ptr->edge_vertex_id(eid,1) );
        vec3d   inters;
        if (intersection(Ray(v,grad), Segment(e0,e1), inters))
        {
            Sample sample;
            sample.tid = tid;
            triangle_barycentric_coords(tangent_space.at( m_ptr->triangle_vertex_id(tid,0) ),
                                        tangent_space.at( m_ptr->triangle_vertex_id(tid,1) ),
                                        tangent_space.at( m_ptr->triangle_vertex_id(tid,2) ),
                                        inters, sample.bary);

            for(int i=0; i<3; ++i) sample.pos += sample.bary.at(i) * m_ptr->triangle_vertex(tid, i);

            return sample;
        }
    }
    assert(false && "Integral curve - Something is off here...");

    Sample dummy;
    return dummy;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::Sample IntegralCurve<Trimesh>::move_forward_from_edge(const int eid, const vec3d & p)
{
    assert(m_ptr->edge_is_manifold(eid));

    int   t0 = m_ptr->adj_edg2tri(eid).front();
    int   t1 = m_ptr->adj_edg2tri(eid).back();
    vec3d n  = m_ptr->triangle_normal(t0) + m_ptr->triangle_normal(t1); n.normalize();
    Plane tangent_plane(p,n);

    vec3d grad = grad_ptr->vec_at(t0) + grad_ptr->vec_at(t1);
    grad = tangent_plane.project_onto(p + grad) - p;
    grad.normalize();
    assert(grad.length() > 0);

    std::map<int,vec3d> tangent_space;
    int v0 = m_ptr->edge_vertex_id(eid,0);
    int v1 = m_ptr->edge_vertex_id(eid,1);
    int v2 = m_ptr->vertex_opposite_to(t0, v0, v1);
    int v3 = m_ptr->vertex_opposite_to(t1, v0, v1);
    tangent_space[v0] = m_ptr->vertex(v0);
    tangent_space[v1] = m_ptr->vertex(v1);
    tangent_space[v2] = tangent_plane.project_onto(m_ptr->vertex(v2));
    tangent_space[v3] = tangent_plane.project_onto(m_ptr->vertex(v3));

    std::vector< std::pair<int,int> > edges_to_check;
    for(int e : m_ptr->adj_tri2edg(t0)) if (e!=eid) edges_to_check.push_back(std::make_pair(e,t0));
    for(int e : m_ptr->adj_tri2edg(t1)) if (e!=eid) edges_to_check.push_back(std::make_pair(e,t1));

    for(auto obj : edges_to_check)
    {
        int     e   = obj.first;
        int     tid = obj.second;
        vec3d   v0  = tangent_space.at( m_ptr->edge_vertex_id(e,0) );
        vec3d   v1  = tangent_space.at( m_ptr->edge_vertex_id(e,1) );
        vec3d   inters;
        if (intersection(Ray(p,grad), Segment(v0,v1), inters))
        {
            Sample sample;
            sample.tid = tid;
            triangle_barycentric_coords(tangent_space.at( m_ptr->triangle_vertex_id(tid,0) ),
                                        tangent_space.at( m_ptr->triangle_vertex_id(tid,1) ),
                                        tangent_space.at( m_ptr->triangle_vertex_id(tid,2) ),
                                        inters, sample.bary);

            for(int i=0; i<3; ++i) sample.pos += sample.bary.at(i) * m_ptr->triangle_vertex(tid, i);

            return sample;
        }
    }
    assert(false && "Integral curve - Something is off here...");

    Sample dummy;
    return dummy;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::Sample IntegralCurve<Trimesh>::move_forward_from_face(const int tid, const vec3d & p)
{
    vec3d grad = grad_ptr->vec_at(tid);
    grad.normalize();

    for(int e=0; e<3; ++e)
    {
        vec3d   e0  = m_ptr->triangle_vertex(tid,  TRI_EDGES[e][0]);
        vec3d   e1  = m_ptr->triangle_vertex(tid,  TRI_EDGES[e][1]);
        vec3d   inters;
        if (intersection(Ray(p,grad), Segment(e0,e1), inters))
        {
            Sample sample;
            sample.pos = inters;
            sample.tid = tid;
            triangle_barycentric_coords(m_ptr->triangle_vertex(tid,0),
                                        m_ptr->triangle_vertex(tid,1),
                                        m_ptr->triangle_vertex(tid,2),
                                        inters, sample.bary);
            return sample;
        }
    }

    assert(false && "Integral curve - Something is off here...");
    Sample dummy;
    return dummy;
}


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
CINO_INLINE
Curve::Sample IntegralCurve<Trimesh>::move_forward(const Sample & s)
{
    int id;
    if (is_on_vertex(s,id))
    {
        return move_forward_from_vertex(id);
    }
    else if (is_on_edge(s,id))
    {
        return move_forward_from_edge(id, s.pos);
    }
    else if (is_on_face(s,id))
    {
        return move_forward_from_face(id, s.pos);
    }
    else assert(false && "Integral curve - Something is off here...");

    Sample dummy;
    return dummy;
}


// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::                                                 :::::::::::::::::::: //
// :::::::::::::::::::: TEMPLATE SPECIALIZATIONS FOR TETRAHEDRAL MESHES :::::::::::::::::::: //
// ::::::::::::::::::::                                                 :::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

template<>
CINO_INLINE
bool IntegralCurve<Tetmesh>::is_on_face(const Sample & s, int & tid, int & fid, const double tol) const
{
    assert(s.tid!=-1);

    std::vector<int> non_zero_coords;
    for(size_t off=0; off<s.bary.size(); ++off)
    {
        if (s.bary.at(off) > tol) non_zero_coords.push_back(off);
    }

    if (non_zero_coords.size() == 3)
    {
        std::vector<int> my_f;
        my_f.push_back(m_ptr->elem_vertex_id(s.tid, non_zero_coords.at(0)));
        my_f.push_back(m_ptr->elem_vertex_id(s.tid, non_zero_coords.at(1)));
        my_f.push_back(m_ptr->elem_vertex_id(s.tid, non_zero_coords.at(2)));
        std::sort(my_f.begin(), my_f.end());

        for(int f=0; f<4; ++f)
        {
            std::vector<int> curr_f;
            curr_f.push_back(m_ptr->elem_vertex_id(s.tid, TET_FACES[f][0]));
            curr_f.push_back(m_ptr->elem_vertex_id(s.tid, TET_FACES[f][1]));
            curr_f.push_back(m_ptr->elem_vertex_id(s.tid, TET_FACES[f][2]));
            std::sort(curr_f.begin(), curr_f.end());

            if (my_f == curr_f)
            {
                tid = s.tid;
                fid = f;
                return true;
            }
        }
        assert(false && "Integral curve - Something is off here...");
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
bool IntegralCurve<Tetmesh>::is_on_cell(const Sample & s, int & tid, const double tol) const
{
    assert(s.tid!=-1);

    std::vector<int> non_zero_coords;
    for(size_t off=0; off<s.bary.size(); ++off)
    {
        if (s.bary.at(off) > tol) non_zero_coords.push_back(off);
    }

    if (non_zero_coords.size() == 4)
    {
        tid = s.tid;
        return true;
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::Sample IntegralCurve<Tetmesh>::move_forward_from_vertex(const int vid)
{
    vec3d p = m_ptr->vertex(vid);
    vec3d grad(0,0,0);
    for(int tid : m_ptr->adj_vtx2tet(vid)) grad += grad_ptr->vec_at(tid);
    grad.normalize();
    assert(grad.length() > 0);

    for(int tid : m_ptr->adj_vtx2tet(vid))
    {
        int     fid = m_ptr->tet_face_opposite_to(tid, vid);
        vec3d   v0  = m_ptr->tet_vertex(tid, TET_FACES[fid][0]);
        vec3d   v1  = m_ptr->tet_vertex(tid, TET_FACES[fid][1]);
        vec3d   v2  = m_ptr->tet_vertex(tid, TET_FACES[fid][2]);
        vec3d   inters;
        if (ray_triangle_intersection(Ray(p,grad), v0, v1, v2, inters))
        {
            Sample sample;
            sample.pos = inters;
            sample.tid = tid;
            tet_barycentric_coords(m_ptr->tet_vertex(tid, 0),
                                   m_ptr->tet_vertex(tid, 1),
                                   m_ptr->tet_vertex(tid, 2),
                                   m_ptr->tet_vertex(tid, 3),
                                   inters, sample.bary);
            return sample;
        }
    }

    Sample dummy;
    return dummy;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::Sample IntegralCurve<Tetmesh>::move_forward_from_edge(const int eid, const vec3d & p)
{
    vec3d grad(0,0,0);
    for(int tid : m_ptr->adj_edg2tet(eid)) grad += grad_ptr->vec_at(tid);
    grad.normalize();
    assert(grad.length() > 0);

    int v0  = m_ptr->edge_vertex_id(eid, 0);
    int v1  = m_ptr->edge_vertex_id(eid, 1);

    for(int tid : m_ptr->adj_edg2tet(eid))
    {
        for(int f=0; f<4; ++f)
        {
            // skip faces containing the edge eid
            if (m_ptr->tet_vertex_id(tid, TET_FACES[f][0]) == v0 &&
                m_ptr->tet_vertex_id(tid, TET_FACES[f][1]) == v1) continue;
            if (m_ptr->tet_vertex_id(tid, TET_FACES[f][0]) == v1 &&
                m_ptr->tet_vertex_id(tid, TET_FACES[f][1]) == v0) continue;
            if (m_ptr->tet_vertex_id(tid, TET_FACES[f][1]) == v0 &&
                m_ptr->tet_vertex_id(tid, TET_FACES[f][2]) == v1) continue;
            if (m_ptr->tet_vertex_id(tid, TET_FACES[f][1]) == v1 &&
                m_ptr->tet_vertex_id(tid, TET_FACES[f][2]) == v0) continue;
            if (m_ptr->tet_vertex_id(tid, TET_FACES[f][2]) == v0 &&
                m_ptr->tet_vertex_id(tid, TET_FACES[f][0]) == v1) continue;
            if (m_ptr->tet_vertex_id(tid, TET_FACES[f][2]) == v1 &&
                m_ptr->tet_vertex_id(tid, TET_FACES[f][0]) == v0) continue;

            vec3d v0  = m_ptr->tet_vertex(tid, TET_FACES[f][0]);
            vec3d v1  = m_ptr->tet_vertex(tid, TET_FACES[f][1]);
            vec3d v2  = m_ptr->tet_vertex(tid, TET_FACES[f][2]);

            vec3d inters;
            if (ray_triangle_intersection(Ray(p,grad), v0, v1, v2, inters))
            {
                Sample sample;
                sample.pos = inters;
                sample.tid = tid;
                tet_barycentric_coords(m_ptr->tet_vertex(tid, 0),
                                       m_ptr->tet_vertex(tid, 1),
                                       m_ptr->tet_vertex(tid, 2),
                                       m_ptr->tet_vertex(tid, 3),
                                       inters, sample.bary);
                return sample;
            }
        }
    }

    Sample dummy;
    return dummy;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::Sample IntegralCurve<Tetmesh>::move_forward_from_face(const int tid, const int fid, const vec3d & p)
{
    std::vector<int> tets_to_process;
    tets_to_process.push_back(tid);
    vec3d grad = grad_ptr->vec_at(tid);

    int nbr = m_ptr->adjacent_tet_through_facet(tid, fid);
    if (nbr != -1)
    {
        tets_to_process.push_back(nbr);
        grad += grad_ptr->vec_at(nbr);
    }
    grad.normalize();
    if (grad.length() == 0) // need to understand when and why this may happen....
    {
        Sample dummy;
        return dummy;
    }

    std::vector<int> illegal_f;
    illegal_f.push_back(m_ptr->tet_vertex_id(tid, TET_FACES[fid][0]));
    illegal_f.push_back(m_ptr->tet_vertex_id(tid, TET_FACES[fid][1]));
    illegal_f.push_back(m_ptr->tet_vertex_id(tid, TET_FACES[fid][2]));
    std::sort(illegal_f.begin(), illegal_f.end());

    for(int curr_tid : tets_to_process)
    {
        for(int f=0; f<4; ++f)
        {
            std::vector<int> curr_f;
            curr_f.push_back(m_ptr->tet_vertex_id(curr_tid, TET_FACES[f][0]));
            curr_f.push_back(m_ptr->tet_vertex_id(curr_tid, TET_FACES[f][1]));
            curr_f.push_back(m_ptr->tet_vertex_id(curr_tid, TET_FACES[f][2]));
            std::sort(curr_f.begin(), curr_f.end());

            if (curr_f == illegal_f) continue;

            vec3d v0 = m_ptr->vertex(curr_f[0]);
            vec3d v1 = m_ptr->vertex(curr_f[1]);
            vec3d v2 = m_ptr->vertex(curr_f[2]);

            vec3d inters;
            if (ray_triangle_intersection(Ray(p,grad), v0, v1, v2, inters))
            {
                Sample sample;
                sample.pos = inters;
                sample.tid = curr_tid;
                tet_barycentric_coords(m_ptr->tet_vertex(curr_tid, 0),
                                       m_ptr->tet_vertex(curr_tid, 1),
                                       m_ptr->tet_vertex(curr_tid, 2),
                                       m_ptr->tet_vertex(curr_tid, 3),
                                       inters, sample.bary);
                return sample;
            }
        }
    }

    // this may actually happen if the point is on the surface and the gradient points outside of the domain...
    // assert(false && "Integral curve - Something is off here...");

    Sample dummy;
    return dummy;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::Sample IntegralCurve<Tetmesh>::move_forward_from_cell(const int tid, const vec3d & p)
{
    vec3d grad = grad_ptr->vec_at(tid);
    grad.normalize();

    for(int f=0; f<4; ++f)
    {
        vec3d v0 = m_ptr->tet_vertex(tid, TET_FACES[f][0]);
        vec3d v1 = m_ptr->tet_vertex(tid, TET_FACES[f][1]);
        vec3d v2 = m_ptr->tet_vertex(tid, TET_FACES[f][2]);
        vec3d inters;
        if (ray_triangle_intersection(Ray(p,grad), v0, v1, v2, inters))
        {
            Sample sample;
            sample.pos = inters;
            sample.tid = tid;
            tet_barycentric_coords(m_ptr->tet_vertex(tid,0),
                                   m_ptr->tet_vertex(tid,1),
                                   m_ptr->tet_vertex(tid,2),
                                   m_ptr->tet_vertex(tid,3),
                                   inters, sample.bary);
            return sample;
        }
    }

    assert(false && "Integral curve - Something is off here...");

    Sample dummy;
    return dummy;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::Sample IntegralCurve<Tetmesh>::move_forward(const Sample & s)
{
    int id, id2;
    if (is_on_vertex(s,id)) // vid
    {
        return move_forward_from_vertex(id);
    }
    else if (is_on_edge(s,id)) // eid
    {
        return move_forward_from_edge(id, s.pos);
    }
    else if (is_on_face(s,id,id2)) // tet id, facet id
    {
        return move_forward_from_face(id, id2, s.pos);
    }
    else if (is_on_cell(s,id)) // tet id
    {
        return move_forward_from_cell(id, s.pos);
    }
    else assert(false && "Integral curve - Something is off here...");

    Sample dummy;
    return dummy;
}

}
