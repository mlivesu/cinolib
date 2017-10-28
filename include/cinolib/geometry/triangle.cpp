/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/geometry/triangle.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/plane.h>
#include <set>

namespace cinolib
{

CINO_INLINE
vec3d triangle_normal(const vec3d A, const vec3d B, const vec3d C)
{
    vec3d n = (B-A).cross(C-A);
    n.normalize();
    return n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
double triangle_area(const vec A, const vec B, const vec C)
{
    return (0.5 * (B-A).cross(C-A).length());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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
                                      uint   & exit_edge)
{
    // 1) Find the exit edge
    //

    vec3d uvw[3] = { tri[0]-P, tri[1]-P, tri[2]-P };

    std::set< std::pair<double,int> > sorted_by_angle;
    for(uint i=0; i<3; ++i)
    {
        sorted_by_angle.insert(std::make_pair(dir.angle_rad(uvw[i]),i));
    }

    uint  vert  = (*sorted_by_angle.begin()).second;
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://en.wikipedia.org/wiki/Law_of_sines
//
CINO_INLINE
double triangle_law_of_sines(const double angle_0, const double angle_1, const double length_0) // returns length_1
{
    return sin(angle_1) * length_0 / sin(angle_0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
//
template <class vec>
CINO_INLINE
bool triangle_barycentric_coords(const vec & A,
                                 const vec & B,
                                 const vec & C,
                                 const vec & P,
                                 std::vector<double> & wgts,
                                 const double   tol)
{
    wgts = std::vector<double>(3, 0.0);

    vec    u    = B - A;
    vec    v    = C - A;
    vec    w    = P - A;
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
bool triangle_point_is_inside(const vec    & A,
                              const vec    & B,
                              const vec    & C,
                              const vec    & P,
                              const double   tol)
{
    // NOTE : it assumes the four points are coplanar!
    std::vector<double> wgts;
    return triangle_barycentric_coords(A, B, C, P, wgts, tol);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_bary_is_vertex(const std::vector<double> & bary,
                             uint                      & vid,
                             const double                tol)
{
    assert(bary.size()==3);
    if (bary[0]>tol && bary[1]<=tol && bary[2]<=tol) { vid = 0; return true; }
    if (bary[1]>tol && bary[0]<=tol && bary[2]<=tol) { vid = 1; return true; }
    if (bary[2]>tol && bary[0]<=tol && bary[1]<=tol) { vid = 2; return true; }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_bary_is_edge(const std::vector<double> & bary,
                           uint                      & eid,
                           const double                tol)
{
    assert(bary.size()==3);
    if (bary[0]>tol && bary[1]>tol && bary[2]<=tol) { eid = 0; return true; }
    if (bary[1]>tol && bary[2]>tol && bary[0]<=tol) { eid = 1; return true; }
    if (bary[2]>tol && bary[0]>tol && bary[1]<=tol) { eid = 2; return true; }
    return false;
}

}

