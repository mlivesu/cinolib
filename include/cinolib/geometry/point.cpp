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
#include <cinolib/geometry/point.h>
#include <cinolib/predicates.h>

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Point & p)
{
    in << p.v << "\n";
    return in;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Point::point_closest_to(const vec3d & /*p*/) const
{
    return v;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Point::intersects_ray(const vec3d & /*p*/, const vec3d & /*dir*/, double & /*t*/, vec3d & /*pos*/) const
{
    assert(false && "TODO");
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Point::barycentric_coordinates(const vec3d & /*p*/, double /*bc*/[]) const
{
    assert(false && "NONSENSE");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Point::contains(const vec3d & p, const bool /*strict*/) const
{
    return p.dist_sqrd(v)==0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Point::intersects_segment(const vec3d s[], const bool ignore_if_valid_complex) const
{
    auto res = point_in_segment_3d(v, s[0], s[1]);
    if(ignore_if_valid_complex) return (res==STRICTLY_INSIDE);
    return (res!=STRICTLY_OUTSIDE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Point::intersects_triangle(const vec3d t[], const bool ignore_if_valid_complex) const
{
    auto res = point_in_triangle_3d(v, t[0], t[1], t[2]);
    if(ignore_if_valid_complex) return (res==STRICTLY_INSIDE || res>=ON_EDGE0);
    return (res!=STRICTLY_OUTSIDE);
}

}
