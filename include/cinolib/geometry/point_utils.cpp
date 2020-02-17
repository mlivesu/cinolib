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
#include <cinolib/geometry/point_utils.h>
#include <cinolib/Shewchuk_predicates.h>

namespace cinolib
{

CINO_INLINE
bool points_are_colinear_exact(const vec2d & p0,
                               const vec2d & p1,
                               const vec2d & p2)
{
    return orient2d(p0, p1, p2) == 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool points_are_colinear_exact(const vec3d & p0,
                               const vec3d & p1,
                               const vec3d & p2)
{
    // check if all the 2d orthogonal projections of p0-p1-p2 are colinear
    if(points_are_colinear_exact(vec2d(p0,DROP_X), vec2d(p1,DROP_X), vec2d(p2,DROP_X)) &&
       points_are_colinear_exact(vec2d(p0,DROP_Y), vec2d(p1,DROP_Y), vec2d(p2,DROP_Y)) &&
       points_are_colinear_exact(vec2d(p0,DROP_Z), vec2d(p1,DROP_Z), vec2d(p2,DROP_Z))) return true;

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool points_are_coplanar_exact(const vec3d & p0,
                               const vec3d & p1,
                               const vec3d & p2,
                               const vec3d & p3)
{
    return orient3d(p0, p1, p2, p3) == 0;
}

}
