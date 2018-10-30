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
#ifndef CINO_MOLLER_TRUMBORE_INTERSECTION_H
#define CINO_MOLLER_TRUMBORE_INTERSECTION_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

// Moller-Trumbore algorithm for ray/triangle intersection
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
// https://en.wikipedia.org/wiki/Möller–Trumbore_intersection_algorithm
//
// Returns true if ray and triangle intersect, false otherwise.
// The intersection point (P) is returned in two forms:
//
//     1) parametric  form (t)   : P = ray_orig + t*ray_dir
//     2) barycentric form (bary): P = bary[0]*v0 + bary[1]*v1 + bary[2]*v2
//
// Use either t or bary at your convenience. They define the same point...
//
CINO_INLINE
bool Moller_Trumbore_intersection(const vec3d  & ray_orig,
                                  const vec3d  & ray_dir,
                                  const vec3d  & v0,
                                  const vec3d  & v1,
                                  const vec3d  & v2,
                                        bool   & hits_backside, // true if ray intersects triangle from the back (according to winding order)
                                        bool   & are_coplanar,  // true if ray and triangle are coplanar (no intersection will be computed)
                                        double & t,
                                        vec3d  & bary);
}

#ifndef  CINO_STATIC_LIB
#include "Moller_Trumbore_intersection.cpp"
#endif

#endif // CINO_MOLLER_TRUMBORE_INTERSECTION_H
