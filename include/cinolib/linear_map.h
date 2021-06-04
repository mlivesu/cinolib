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
#ifndef CINO_LINEAR_MAP_H
#define CINO_LINEAR_MAP_H

#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

// Computes the linear map that connects 2D frames (u0,v0) and (u1,v1)
CINO_INLINE
void linear_map(const double u0[2],
                const double v0[2],
                const double u1[2],
                const double v1[2],
                      double T[2][2]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Computes the linear map that connects 2D frames (u0,v0) and (u1,v1)
CINO_INLINE
void linear_map(const vec2d & u0,
                const vec2d & v0,
                const vec2d & u1,
                const vec2d & v1,
                      mat2d & T);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Computes the linear component of the affine map that connects triangles (a0,a1,a2) and (b0,b1,b2)
CINO_INLINE
void linear_map(const vec3d & a0,
                const vec3d & a1,
                const vec3d & a2,
                const vec3d & b0,
                const vec3d & b1,
                const vec3d & b2,
                      mat2d & T);
}

#ifndef  CINO_STATIC_LIB
#include "linear_map.cpp"
#endif

#endif // CINO_LINEAR_MAP_H
