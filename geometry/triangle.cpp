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
#include <cinolib/geometry/triangle.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/plane.h>

namespace cinolib
{

CINO_INLINE
vec3d triangle_normal(const vec3d A, const vec3d B, const vec3d C)
{
    vec3d n = (B-A).cross(C-A);
    n.normalize();
    return n;
}



// Given a triangle t(A,B,C) and a ray r(P,dir) compute both
// the edge and position where r exits from t
//
// NOTE: r is assumed to live "within" t, like in a gradient field for example...
//
CINO_INLINE
void triangle_traverse_with_ray(const vec3d   tri[3],
                                const vec3d   P,
                                const vec3d   dir,
                                      vec3d & exit_pos,
                                      int   & exit_edge)
{
    // 1) Find the exit edge
    //

    vec3d uvw[3] = { tri[0]-P, tri[1]-P, tri[2]-P };

    std::set< std::pair<double,int> > sorted_by_angle;
    for(int i=0; i<3; ++i)
    {
        sorted_by_angle.insert(std::make_pair(dir.angle_rad(uvw[i]),i));
    }

    int   vert  = (*sorted_by_angle.begin()).second;
    vec3d tn    = triangle_normal(tri[0], tri[1], tri[2]);
    vec3d cross = dir.cross(uvw[vert]);

    exit_edge = (cross.dot(tn) >= 0) ? (vert+2)%3 : vert; // it's because e_i = ( v_i, v_(i+1)%3 )

    //
    // 2) Find the exit point using the law of sines
    //

    vec3d  app     = tri[TRI_EDGES[exit_edge][1]];
    vec3d  V1      = tri[TRI_EDGES[exit_edge][0]];
    vec3d  V0      = P;
    vec3d  e0_dir  = app - V1; e0_dir.normalize();
    vec3d  e1_dir  = -dir;
    vec3d  e2_dir  = V1 - V0; e2_dir.normalize();

    double V0_ang  = e2_dir.angle_rad(-e1_dir);
    double V2_ang  = e1_dir.angle_rad(-e0_dir);
    double e2_len  = (V1 - V0).length();
    double e0_len  = triangle_law_of_sines(V2_ang, V0_ang, e2_len);

    exit_pos =  V1 + e0_len * e0_dir;
}


// https://en.wikipedia.org/wiki/Law_of_sines
//
CINO_INLINE
double triangle_law_of_sines(const double angle_0, const double angle_1, const double length_0) // returns length_1
{
    return sin(angle_1) * length_0 / sin(angle_0);
}



// http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
//
CINO_INLINE
bool triangle_barycentric_coords(const vec3d  & A,
                                 const vec3d  & B,
                                 const vec3d  & C,
                                 const vec3d  & P,
                                 std::vector<double> & wgts,
                                 const double   tol)
{
    wgts = std::vector<double>(3, 0.0);

    vec3d  u    = B - A;
    vec3d  v    = C - A;
    vec3d  w    = P - A;
    double d00  = u.dot(u);
    double d01  = u.dot(v);
    double d11  = v.dot(v);
    double d20  = w.dot(u);
    double d21  = w.dot(v);
    double den  = d00 * d11 - d01 * d01;

    if (den==0) return false; // degenerate

    wgts[2] = (d00 * d21 - d01 * d20) / den; assert(!std::isnan(wgts[2]));
    wgts[1] = (d11 * d20 - d01 * d21) / den; assert(!std::isnan(wgts[1]));
    wgts[0] = 1.0f - wgts[1] - wgts[2];      assert(!std::isnan(wgts[0]));

    for(double w : wgts) if (w < -tol || w > 1.0 + tol) return false; // outside
    return true; // inside
}


CINO_INLINE
bool triangle_bary_is_vertex(const std::vector<double> & bary,
                             int                       & vid,
                             const double                tol)
{
    assert(bary.size()==3);
    if (bary[0]>tol && bary[1]<=tol && bary[2]<=tol) { vid = 0; return true; }
    if (bary[1]>tol && bary[0]<=tol && bary[2]<=tol) { vid = 1; return true; }
    if (bary[2]>tol && bary[0]<=tol && bary[1]<=tol) { vid = 2; return true; }
    return false;
}


CINO_INLINE
bool triangle_bary_is_edge(const std::vector<double> & bary,
                             int                     & eid,
                             const double              tol)
{
    assert(bary.size()==3);
    if (bary[0]>tol && bary[1]>tol && bary[2]<=tol) { eid = 0; return true; }
    if (bary[1]>tol && bary[2]>tol && bary[0]<=tol) { eid = 1; return true; }
    if (bary[2]>tol && bary[0]>tol && bary[1]<=tol) { eid = 2; return true; }
    return false;
}

}

