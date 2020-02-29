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
#include <cinolib/exact_geometric_predicates.h>

namespace cinolib
{

CINO_INLINE
ItemType Triangle::item_type() const
{
    return TRIANGLE;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
AABB Triangle::aabb() const
{
    std::vector<vec3d> tmp = {v[0], v[1], v[2]};
    return AABB(tmp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Triangle::point_closest_to(const vec3d & p) const
{
    return triangle_closest_point(p,v[0],v[1],v[2]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Triangle::contains_exact(const vec3d & p, bool strict) const
{
    int where = point_in_triangle_exact(p,v);
    if(strict) return (where==STRICTLY_INSIDE);
               return (where>=STRICTLY_INSIDE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Triangle::intersects_segment_exact(const vec3d s[]) const
{
    return segment_triangle_intersect_exact(v,s);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Triangle::intersects_triangle_exact(const vec3d t[]) const
{
    return triangle_triangle_intersect_exact(v,t);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Triangle::intersects_ray(const vec3d & p, const vec3d & dir, double & t, vec3d & pos) const
{
    bool  hits_backside;
    bool  coplanar;
    vec3d bary;
    if(Moller_Trumbore_intersection(p, dir, v[0], v[1], v[2], hits_backside, coplanar, t, bary))
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

}

