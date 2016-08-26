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
#include "cinolib/unstable/integral_curve_debug.h"

#include <cinolib/unstable/intersection.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/geometry/segment.h>
#include <cinolib/geometry/ray.h>

namespace cinolib
{

template<>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Trimesh>::move_forward_from_vertex(const int vid)
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
        if (intersection::intersection(Ray(v,grad), Segment(e0,e1), inters))
        {
            MeshSample sample;
            sample.tid = tid;
            triangle_barycentric_coords(tangent_space.at( m_ptr->triangle_vertex_id(tid,0) ),
                                        tangent_space.at( m_ptr->triangle_vertex_id(tid,1) ),
                                        tangent_space.at( m_ptr->triangle_vertex_id(tid,2) ),
                                        inters, sample.bary);
            return sample;
        }
    }
    assert(false && "Integral curve - Something is off here...");

    MeshSample dummy;
    return dummy;
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
Curve::MeshSample IntegralCurveDebug<Trimesh>::move_forward_from_edge(const int eid, const vec3d & p)
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
        vec3d   e0  = tangent_space.at( m_ptr->edge_vertex_id(e,0) );
        vec3d   e1  = tangent_space.at( m_ptr->edge_vertex_id(e,1) );
        vec3d   inters;
        if (intersection::intersection(Ray(p,grad), Segment(e0,e1), inters))
        {
            MeshSample sample;
            sample.tid = tid;
            triangle_barycentric_coords(tangent_space.at( m_ptr->triangle_vertex_id(tid,0) ),
                                        tangent_space.at( m_ptr->triangle_vertex_id(tid,1) ),
                                        tangent_space.at( m_ptr->triangle_vertex_id(tid,2) ),
                                        inters, sample.bary);
            return sample;
        }
    }
    assert(false && "Integral curve - Something is off here...");

    MeshSample dummy;
    return dummy;
}

}
