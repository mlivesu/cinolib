/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_INTERSECTION_H
#define CINO_INTERSECTION_H

#include <vector>
#include <cinolib/cino_inline.h>
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool line_triangle_intersection(const Line  & l,
                                const vec3d & V0,
                                const vec3d & V1,
                                const vec3d & V2,
                                      vec3d & inters,
                                const double  tol = 1e-5);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool ray_triangle_intersection(const Ray   & r,
                               const vec3d & V0,
                               const vec3d & V1,
                               const vec3d & V2,
                                     vec3d & inters,
                               const double  tol = 1e-5);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool least_squares_intersection(const std::vector<Plane> & planes,
                                      vec3d              & inters);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool intersection(const Ray     & r,
                  const Segment & s,
                        vec3d   & inters,
                  const double    tol = 1e-7);

}

#ifndef  CINO_STATIC_LIB
#include "intersection.cpp"
#endif

#endif // CINO_INTERSECTION_H
