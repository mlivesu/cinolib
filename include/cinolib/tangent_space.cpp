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
#include <cinolib/tangent_space.h>

namespace cinolib
{

CINO_INLINE
bool tangent_frame(const vec3d & p0, // 3D coordinates of triangle <p0,p1,p2>
                   const vec3d & p1,
                   const vec3d & p2,
                         vec3d & u,  // axes of a 2D orthonormal frame in the supporting
                         vec3d & v)  // plane of triangle <p0,p1,p2>
{
    bool degenerate = false;
    u  = p1-p0;
    if (u.normalize() < 1e-8) degenerate = true;
    v  = p2-p0;
    v -= u*u.dot(v);
    if (v.normalize() < 1e-8) degenerate = true;
    if (degenerate)
    {
        u = v = vec3d(0,0,0);
        std::cerr << "WARNING: degenerate triangle. Undefined tangent space!" << std::endl;
        return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool tangent_space_2d_coords(const vec3d & p0,  // 3D coordinates of triangle <p0,p1,p2>
                             const vec3d & p1,
                             const vec3d & p2,
                                   vec2d & P0,  // 2D coordinates of triangle <P0,P1,P2>
                                   vec2d & P1,  // in tangent space
                                   vec2d & P2)
{
    vec3d u,v;
    if (tangent_frame(p0, p1, p2, u, v))
    {
        P0 = vec2d(0,0);
        P1 = vec2d((p1-p0).dot(u), (p1-p0).dot(v));
        P2 = vec2d((p2-p0).dot(u), (p2-p0).dot(v));
        return true;
    }
    P0 = P1 = P2 = vec2d(0,0);
    return false;
}

}
