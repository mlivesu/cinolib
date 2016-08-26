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
#include <cinolib/unstable/integral_curve_debug.h>

#include <cinolib/gl/draw_cylinder.h>
#include <cinolib/gl/draw_arrow.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/colors.h>
#include <cinolib/unstable/intersection.h>
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
IntegralCurveDebug<Mesh>::IntegralCurveDebug( Mesh                & m,
                                             const VectorField         & grad,
                                             const int                   tid,
                                             const std::vector<double> & bary)
    : DrawableCurve()
    , m_ptr(&m)
    , grad_ptr(&grad)
{
    MeshSample seed;
    seed.tid  = tid;
    seed.bary = bary;

    make_curve(seed);
}

template<class Mesh>
CINO_INLINE
IntegralCurveDebug<Mesh>::IntegralCurveDebug( Mesh        & m,
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

    MeshSample seed;
    seed.tid  = tid;
    seed.bary = bary;

    make_curve(seed);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void IntegralCurveDebug<Mesh>::make_curve(const MeshSample & seed)
{
    link_to_mesh.push_back(seed);

    do
    {
        MeshSample s = move_forward( link_to_mesh.back() );
        link_to_mesh.push_back(s);
    }
    while (!is_converged(link_to_mesh.back()));

    for(const MeshSample & s : link_to_mesh)
    {
        vec3d s_pos(0,0,0);
        for(int off=0; off<m_ptr->verts_per_element; ++off)
        {
            s_pos += s.bary.at(off) * m_ptr->elem_vertex(s.tid, off);
        }
        append_sample(s_pos);
    }
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Mesh>::move_forward(const MeshSample & s)
{
    int id, id2;
    if (is_on_vertex(s,id))
    {
        return move_forward_from_vertex(id);
    }
    else
    if (is_on_edge(s,id))
    {
        vec3d pos(0,0,0);
        for(int off=0; off<m_ptr->verts_per_element; ++off)
        {
            pos += s.bary.at(off) * m_ptr->elem_vertex(s.tid, off);
        }
        return move_forward_from_edge(id, pos);
    }
    else
    if (is_on_face(s,id, id2))
    {
        vec3d pos(0,0,0);
        for(int off=0; off<m_ptr->verts_per_element; ++off)
        {
            pos += s.bary.at(off) * m_ptr->elem_vertex(s.tid, off);
        }
        return move_forward_from_face(id, id2, pos);
    }
    else
    if (is_on_cell(s,id))
    {
        assert(false);
        //return move_forward_from_cell(id);
    }
    else assert(false && "Integral curve - Something is off here...");

    MeshSample dummy;
    return dummy;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
bool IntegralCurveDebug<Mesh>::is_converged(const MeshSample & sample)
{
    if (sample.tid == -1)
    {
        link_to_mesh.pop_back();
        return true;
    }

    for(int off=0; off<m_ptr->verts_per_element; ++off)
    {
        int vid = m_ptr->elem_vertex_id(sample.tid, off);

        if (m_ptr->vertex_is_local_maxima(vid))
        {
            MeshSample s;
            s.tid  = sample.tid;
            s.bary = std::vector<double>(m_ptr->verts_per_element,0);
            s.bary.at(off) = 1.0;
            link_to_mesh.push_back(s);
            return true;
        }
    }
    return false;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
bool IntegralCurveDebug<Mesh>::is_on_vertex(const MeshSample & s, int & vid, const double tol) const
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
bool IntegralCurveDebug<Mesh>::is_on_edge(const MeshSample & s, int & eid, const double tol) const
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

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
bool IntegralCurveDebug<Mesh>::is_on_face(const MeshSample &, int &, int &, const double) const
{
    std::cerr << "WARNING - Functionality not supported for this mesh type yet!" << std::endl;
    assert(false);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
bool IntegralCurveDebug<Mesh>::is_on_cell(const MeshSample &, int &, const double) const
{
    std::cerr << "WARNING - Functionality not supported for this mesh type yet!" << std::endl;
    assert(false);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Mesh>::move_forward_from_vertex(const int)
{
    std::cerr << "WARNING - Functionality not supported for this mesh type yet!" << std::endl;
    assert(false);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Mesh>::move_forward_from_edge(const int, const vec3d &)
{
    std::cerr << "WARNING - Functionality not supported for this mesh type yet!" << std::endl;
    assert(false);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Mesh>::move_forward_from_face(const int, const int, const vec3d &)
{
    std::cerr << "WARNING - Functionality not supported for this mesh type yet!" << std::endl;
    assert(false);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Tetmesh>::move_forward_from_vertex(const int vid)
{
    vec3d p = m_ptr->vertex(vid);
    vec3d grad(0,0,0);
    for(int tid : m_ptr->adj_vtx2tet(vid)) grad += grad_ptr->vec_at(tid);
    grad.normalize();
    assert(grad.length() > 0);

    for(int tid : m_ptr->adj_vtx2tet(vid))
    {
        m_ptr->set_tet_label(tid, 1); // debug

        int     fid = m_ptr->tet_face_opposite_to(tid, vid);
        vec3d   v0  = m_ptr->tet_vertex(tid, TET_FACES[fid][0]);
        vec3d   v1  = m_ptr->tet_vertex(tid, TET_FACES[fid][1]);
        vec3d   v2  = m_ptr->tet_vertex(tid, TET_FACES[fid][2]);
        vec3d   inters;
        if (intersection::ray_triangle_intersection(Ray(p,grad), v0, v1, v2, inters))
        {
            MeshSample sample;
            sample.tid = tid;
            tet_barycentric_coords(m_ptr->tet_vertex(tid, 0),
                                   m_ptr->tet_vertex(tid, 1),
                                   m_ptr->tet_vertex(tid, 2),
                                   m_ptr->tet_vertex(tid, 3),
                                   inters, sample.bary);
            return sample;
        }
    }

    // this may actually happen if the point is on the surface and the gradient points outside of the domain...
    // assert(false && "Integral curve - Something is off here...");

    MeshSample dummy;
    return dummy;
}


template<>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Tetmesh>::move_forward_from_edge(const int eid, const vec3d & p)
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
            if (intersection::ray_triangle_intersection(Ray(p,grad), v0, v1, v2, inters))
            {
                MeshSample sample;
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

    // this may actually happen if the point is on the surface and the gradient points outside of the domain...
    // assert(false && "Integral curve - Something is off here...");

    MeshSample dummy;
    return dummy;
}


template<>
CINO_INLINE
bool IntegralCurveDebug<Tetmesh>::is_on_face(const MeshSample & s, int & tid, int & fid, const double tol) const
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


template<>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Tetmesh>::move_forward_from_face(const int tid, const int fid, const vec3d & p)
{
    std::vector<int> tets;
    tets.push_back(tid);

    vec3d grad = grad_ptr->vec_at(tid);

    int nbr = m_ptr->adjacent_tet_through_facet(tid, fid);
    if (nbr != -1)
    {
        tets.push_back(nbr);

        grad += grad_ptr->vec_at(nbr);
        grad.normalize();
        assert(grad.length() > 0);
    }

    std::vector<int> illegal_f;
    illegal_f.push_back(m_ptr->tet_vertex_id(tid, TET_FACES[fid][0]));
    illegal_f.push_back(m_ptr->tet_vertex_id(tid, TET_FACES[fid][1]));
    illegal_f.push_back(m_ptr->tet_vertex_id(tid, TET_FACES[fid][2]));
    std::sort(illegal_f.begin(), illegal_f.end());

    for(int curr_tid : tets)
    {
        m_ptr->set_tet_label(curr_tid, 1);

        for(int f=0; f<4; ++f)
        {
            std::vector<int> curr_f;
            curr_f.push_back(m_ptr->tet_vertex_id(curr_tid, TET_FACES[f][0]));
            curr_f.push_back(m_ptr->tet_vertex_id(curr_tid, TET_FACES[f][1]));
            curr_f.push_back(m_ptr->tet_vertex_id(curr_tid, TET_FACES[f][2]));
            std::sort(curr_f.begin(), curr_f.end());

            if (curr_f == illegal_f) continue;

            vec3d v0  = m_ptr->vertex(curr_f[0]);
            vec3d v1  = m_ptr->vertex(curr_f[1]);
            vec3d v2  = m_ptr->vertex(curr_f[2]);

            vec3d inters;
            if (intersection::ray_triangle_intersection(Ray(p,grad), v0, v1, v2, inters))
            {
                MeshSample sample;
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

    MeshSample dummy;
    return dummy;
}


}
