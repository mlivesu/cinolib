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
#include "intersection.h"

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/algorithms/intersection.hpp>

#include <eigen3/Eigen/Dense>

namespace cinolib
{
namespace intersection
{

namespace bg = boost::geometry;
typedef   bg::model::point<double,3,bg::cs::cartesian> Point;
typedef   bg::model::segment<Point>                    Segment;

CINO_INLINE
bool segment2D(const vec2d        & s0_beg,
               const vec2d        & s0_end,
               const vec2d        & s1_beg,
               const vec2d        & s1_end,
               std::vector<vec2d> & inters)
{
    assert(inters.empty());

    std::vector<Point> res;
    bg::intersection(Segment(Point(s0_beg.x(), s0_beg.y()),
                             Point(s0_end.x(), s0_end.y())),
                     Segment(Point(s1_beg.x(), s1_beg.y()),
                             Point(s1_end.x(), s1_end.y())),
                     res);

    // if s1 and s2 are colinear returns the endpoints of the shared portion
    //
    for(Point p : res) inters.push_back(vec2d(p.get<0>(), p.get<1>()));

    return !inters.empty();
}


CINO_INLINE
bool line_triangle_intersection(const vec3d   P,
                                const vec3d   dir,
                                const vec3d   V0,
                                const vec3d   V1,
                                const vec3d   V2,
                                      vec3d & inters)
{
    // just to avoid numerical imprecision...
    if ((P-V0).length() == 0) { inters = V0; return true; }
    if ((P-V1).length() == 0) { inters = V1; return true; }
    if ((P-V2).length() == 0) { inters = V2; return true; }

    // compute ray plane intersection
    // https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
    //
    Plane tri_supp_plane(V0, V1, V2);
    double den = dir.dot(tri_supp_plane.n);
    if (fabs(den) < 1e-5) return false;
    double t = -(P.dot(tri_supp_plane.n)-tri_supp_plane.d)/den;
    inters   = P + t * dir;
    assert(tri_supp_plane.point_plane_dist(inters) < 1e-7);

    // check whether intersection is inside triangle
    // (agnostic of the vertices winding order)
    //
    vec3d tri_centr = (V0 + V1 + V2) / 3.0;
    vec3d V[3] = { V0, V1, V2 };
    for(int i=0; i<3; ++i)
    {
        vec3d e = V[(i+1)%3] - V[i];
        vec3d n = e.cross(tri_supp_plane.n);
        Plane test(V[i], n);
        if (test[tri_centr] * test[inters] < 0) return false;
    }
    return true;
}



CINO_INLINE
bool ray_triangle_intersection(const vec3d   P,
                               const vec3d   dir,
                               const vec3d   V0,
                               const vec3d   V1,
                               const vec3d   V2,
                                     vec3d & inters)
{
    if (line_triangle_intersection(P, dir, V0, V1, V2, inters))
    {
        vec3d u = inters - P;
        if (u.dot(dir) < 0) return false;
        return true;
    }

    return false;
}

}
}
