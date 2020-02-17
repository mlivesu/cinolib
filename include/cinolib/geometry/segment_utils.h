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
#ifndef CINO_SEGMENT_UTILS_H
#define CINO_SEGMENT_UTILS_H

#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/vec2.h>

namespace cinolib
{

// true if point p lies (strictly) inside segment v0-v1
CINO_INLINE
bool segment_contains_point_exact(const vec2d & v0,
                                  const vec2d & v1,
                                  const vec2d & p,
                                  const bool    strict,
                                  const bool    skip_colinearity_test = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if point p lies (strictly) inside segment v0-v1
CINO_INLINE
bool segment_contains_point_exact(const vec3d & v0,
                                  const vec3d & v1,
                                  const vec3d & p,
                                  const bool    strict,
                                  const bool    skip_colinearity_test = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if point segments u and v intersect. Shared endpoints are not
// counted as intersections if allow_shared_endpoints is true
CINO_INLINE
bool segments_intersect_exact(const vec2d & u0,
                              const vec2d & u1,
                              const vec2d & v0,
                              const vec2d & v1,
                              const bool    allow_shared_endpoints);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if point segments u and v intersect. Shared endpoints are not
// counted as intersections if allow_shared_endpoints is true
CINO_INLINE
bool segments_intersect_exact(const vec3d & u0,
                              const vec3d & u1,
                              const vec3d & v0,
                              const vec3d & v1,
                              const bool    allow_shared_endpoints);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
}

#ifndef  CINO_STATIC_LIB
#include "segment_utils.cpp"
#endif

#endif // CINO_SEGMENT_UTILS_H
