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
#include <cinolib/geometry/triangle.h>
#include <cinolib/Moller_Trumbore_intersection.h>
#include <cinolib/predicates.h>
#include <cinolib/geometry/triangle_utils.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Triangle::point_closest_to(const vec3d & p) const
{
    return triangle_closest_point(p,v[0],v[1],v[2]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Triangle::intersects_ray(const vec3d & p, const vec3d & dir, double & t, vec3d & pos) const
{
    bool  hits_backside;
    bool  coplanar;
    vec3d bary;
    if(Moller_Trumbore_intersection(p, dir, v[0], v[1], v[2], hits_backside, coplanar, t, bary) && t>=0)
    {
        pos = p + t * dir;
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Triangle::barycentric_coordinates(const vec3d & p, double bc[]) const
{
    triangle_barycentric_coords(v[0], v[1], v[2], p, bc);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Triangle::contains(const vec3d & p, const bool strict) const
{
    int where = point_in_triangle_3d(p,v[0], v[1], v[2]);
    if(strict) return (where==STRICTLY_INSIDE);
    return (where>=STRICTLY_INSIDE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Triangle::intersects_segment(const vec3d s[], const bool ignore_if_valid_complex) const
{
    auto res = segment_triangle_intersect_3d(s[0],s[1], v[0], v[1], v[2]);
    if(ignore_if_valid_complex) return (res > SIMPLICIAL_COMPLEX);
    return (res>=SIMPLICIAL_COMPLEX);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Triangle::intersects_triangle(const vec3d t[], const bool ignore_if_valid_complex) const
{
    auto res = triangle_triangle_intersect_3d(v[0], v[1], v[2], t[0], t[1], t[2]);
    if(ignore_if_valid_complex) return (res > SIMPLICIAL_COMPLEX);
    return (res>=SIMPLICIAL_COMPLEX);
}

}

