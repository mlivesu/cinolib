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
#include <cinolib/linear_map.h>
#include <cinolib/tangent_space.h>

namespace cinolib
{

CINO_INLINE
void linear_map(const double u0[2],
                const double v0[2],
                const double u1[2],
                const double v1[2],
                      double T[2][2])
{
    // compute the transformation as T = |u1 v1| * |u0 v0|^-1

    double uv0[2][2] = {{u0[0],v0[0]}, {u0[1],v0[1]}};
    double uv1[2][2] = {{u1[0],v1[0]}, {u1[1],v1[1]}};
    double inv[2][2];
    mat_inverse<2,double>(uv0,inv);
    mat_times<2,2,2,double>(uv1,inv,T);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void linear_map(const vec2d & u0,
                const vec2d & v0,
                const vec2d & u1,
                const vec2d & v1,
                      mat2d & T)
{
    // compute the transformation as T = |u1 v1| * |u0 v0|^-1

    mat2d uv0({u0[0],v0[0], u0[1],v0[1]});
    mat2d uv1({u1[0],v1[0], u1[1],v1[1]});

    T = uv1 * uv0.inverse();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void linear_map(const vec3d & a0,
                const vec3d & a1,
                const vec3d & a2,
                const vec3d & b0,
                const vec3d & b1,
                const vec3d & b2,
                      mat2d & T)
{
    // compute 2D coordinates in tangent space
    vec2d A0,A1,A2;
    vec2d B0,B1,B2;
    tangent_space_2d_coords(a0,a1,a2,A0,A1,A2);
    tangent_space_2d_coords(b0,b1,b2,B0,B1,B2);

    // compute 2D frames in tangent space
    vec2d u0 = A1 - A0;
    vec2d v0 = A2 - A0;
    vec2d u1 = B1 - B0;
    vec2d v1 = B2 - B0;

    // solve for the mapping
    linear_map(u0,v0,u1,v1,T);
}

}
