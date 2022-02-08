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
#include <cinolib/geometry/tetrahedron.h>
#include <cinolib/geometry/tetrahedron_utils.h>
#include <cinolib/predicates.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Tetrahedron::point_closest_to(const vec3d & p) const
{
    return tetrahedron_closest_point(p,v[0],v[1],v[2],v[3]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Tetrahedron::intersects_ray(const vec3d & p, const vec3d & dir, double & t, vec3d & pos) const
{
    bool   backside;
    bool   coplanar;
    vec3d  bary;
    double tt[4] = { -1, -1, -1, -1 };
    Moller_Trumbore_intersection(p, dir, v[0], v[2], v[1], backside, coplanar, tt[0], bary);
    Moller_Trumbore_intersection(p, dir, v[0], v[1], v[3], backside, coplanar, tt[1], bary);
    Moller_Trumbore_intersection(p, dir, v[0], v[3], v[2], backside, coplanar, tt[2], bary);
    Moller_Trumbore_intersection(p, dir, v[1], v[2], v[3], backside, coplanar, tt[3], bary);
    // NOTE: here tt is used as a boolean flag for intersection. If an intersection occurs,
    // the corresponding tt value will become positive
    if(*std::max_element(tt, tt+4)>=0)
    {
        // the smallest positive tt locates the correct intersection point
        t = 0;
        for(uint i=0; i<4; ++i)
        {
            if(tt[i]>0 && tt[i]<t) t = tt[i];
        }
        pos = p + t*dir;
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Tetrahedron::barycentric_coordinates(const vec3d &p, double bc[]) const
{
    tet_barycentric_coords(v[0], v[1], v[2], v[3], p, bc);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Tetrahedron::contains(const vec3d & p, const bool strict) const
{
    int where = point_in_tet(p, v[0], v[1], v[2], v[3]);
    if(strict) return (where==STRICTLY_INSIDE);
    return (where>=STRICTLY_INSIDE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Tetrahedron::intersects_segment(const vec3d [], const bool) const
{
    assert(false && "TODO!");
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Tetrahedron::intersects_triangle(const vec3d [], const bool) const
{
    assert(false && "TODO!");
    return false;
}

}

