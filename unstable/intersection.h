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
#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/geometry/line.h>
#include <cinolib/geometry/ray.h>
#include <cinolib/geometry/segment.h>

namespace cinolib
{

CINO_INLINE
bool segment2D_intersection(const vec2d        & s0_beg,
                            const vec2d        & s0_end,
                            const vec2d        & s1_beg,
                            const vec2d        & s1_end,
                            std::vector<vec2d> & inters);


CINO_INLINE
bool line_triangle_intersection(const Line  & l,
                                const vec3d & V0,
                                const vec3d & V1,
                                const vec3d & V2,
                                      vec3d & inters,
                                const double  tol = 1e-5);

CINO_INLINE
bool ray_triangle_intersection(const Ray   & r,
                               const vec3d & V0,
                               const vec3d & V1,
                               const vec3d & V2,
                                     vec3d & inters,
                               const double  tol = 1e-5);

CINO_INLINE
bool least_squares_intersection(const std::vector<Plane> & planes,
                                      vec3d              & inters);

CINO_INLINE
bool intersection(const Ray     & r,
                  const Segment & s,
                        vec3d   & inters,
                  const double    tol = 1e-7);

}

#ifndef  CINO_STATIC_LIB
#include "intersection.cpp"
#endif

#endif // INTERSECTION_H
