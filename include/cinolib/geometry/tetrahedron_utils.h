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
#ifndef CINO_TETRAHEDRON_UTILS_H
#define CINO_TETRAHEDRON_UTILS_H

#include <cinolib/geometry/vec3.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void tet_barycentric_coords(const vec3d & A,
                            const vec3d & B,
                            const vec3d & C,
                            const vec3d & D,
                            const vec3d & P,
                            double wgts[]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Given a point P and a tetrahedron ABCD, finds the point in ABCD that
// is closest to P. This code was taken directly from Ericson's seminal
// book "Real Time Collision Detection", Section 5.1.6
//
CINO_INLINE
vec3d tetrahedron_closest_point(const vec3d & P,
                                const vec3d & A,
                                const vec3d & B,
                                const vec3d & C,
                                const vec3d & D);
}

#ifndef  CINO_STATIC_LIB
#include "tetrahedron_utils.cpp"
#endif

#endif // CINO_TETRAHEDRON_UTILS_H
