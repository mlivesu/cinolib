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
IntegralCurveDebug<Mesh>::IntegralCurveDebug(const Mesh                & m,
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
IntegralCurveDebug<Mesh>::IntegralCurveDebug(const Mesh        & m,
                                             const VectorField & grad,
                                             const int           vid)
    : DrawableCurve()
    , m_ptr(&m)
    , grad_ptr(&grad)
{
    assert(!m.adj_vtx2tri(vid).empty());
    int tid = m.adj_vtx2tri(vid).front();

    int off = 0;
    while(off<m.verts_per_element && m.elem_vertex_id(tid,off)!=vid) ++off;
    assert(m.elem_vertex_id(tid,off)==vid && "Cannot find incident triangle to begin with");

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
Curve::MeshSample IntegralCurveDebug<Mesh>::move_forward(const MeshSample & s) const
{
    int id;
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
            pos += s.bary.at(off) * m_ptr->triangle_vertex(s.tid, off);
        }
        return move_forward_from_edge(id, pos);
    }
    else
    if (is_on_face(s,id))
    {
        assert(false);
        //return move_forward_from_face(id);
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
    std::vector<int> non_zero_coords;
    for(size_t off=0; off<s.bary.size(); ++off)
    {
        if (s.bary.at(off) > tol) non_zero_coords.push_back(off);
    }

    if (non_zero_coords.size() == 2)
    {
        int vid0 =  m_ptr->elem_vertex_id(s.tid, non_zero_coords.at(0));
        int vid1 =  m_ptr->elem_vertex_id(s.tid, non_zero_coords.at(1));
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
bool IntegralCurveDebug<Mesh>::is_on_face(const MeshSample &, int &, const double) const
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
Curve::MeshSample IntegralCurveDebug<Mesh>::move_forward_from_vertex(const int &) const
{
    std::cerr << "WARNING - Functionality not supported for this mesh type yet!" << std::endl;
    assert(false);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Mesh>::move_forward_from_edge(const int &, const vec3d &) const
{
    std::cerr << "WARNING - Functionality not supported for this mesh type yet!" << std::endl;
    assert(false);
}

}
