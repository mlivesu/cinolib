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
#ifndef CINO_TRIANGLE_UTILS_H
#define CINO_TRIANGLE_UTILS_H

#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/vec2.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d triangle_normal(const vec3d A, const vec3d B, const vec3d C);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
double triangle_area(const vec A, const vec B, const vec C);

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
                                      uint  & exit_edge);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://en.wikipedia.org/wiki/Law_of_sines
//
CINO_INLINE
double triangle_law_of_sines(const double angle_0,
                             const double angle_1,
                             const double length_0); // returns length_1

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
void triangle_barycentric_coords(const vec & A,
                                 const vec & B,
                                 const vec & C,
                                 const vec & P,
                                 double wgts[]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d triangle_closest_point(const vec3d & P,
                             const vec3d & A,
                             const vec3d & B,
                             const vec3d & C);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double point_to_triangle_dist(const vec3d & P,
                              const vec3d & A,
                              const vec3d & B,
                              const vec3d & C);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double point_to_triangle_dist_sqrd(const vec3d & P,
                                   const vec3d & A,
                                   const vec3d & B,
                                   const vec3d & C);
}

#ifndef  CINO_STATIC_LIB
#include "triangle_utils.cpp"
#endif

#endif // CINO_TRIANGLE_UTILS_H
