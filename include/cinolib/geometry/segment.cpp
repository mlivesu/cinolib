/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/geometry/segment.h>

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Segment & s)
{
    in << s.first << "\t" << s.second << "\n";
    return in;
}


CINO_INLINE
Segment::Segment(const vec3d & P0, const vec3d & P1)
{
    first  = P0;
    second = P1;
}


CINO_INLINE
double Segment::operator[](const vec3d & p) const
{
    return dist_to_point(p);
}


CINO_INLINE
vec3d Segment::project_onto(const vec3d &p) const
{
    vec3d v = second - first;
    vec3d w = p  - first;

    float cos_wv = w.dot(v);
    float cos_vv = v.dot(v);

    if (cos_wv <= 0.0)    return first;
    if (cos_vv <= cos_wv) return second;

    float b  = cos_wv / cos_vv;
    vec3d Pb = first + v*b;

    return Pb;
}


CINO_INLINE
double Segment::dist_to_point(const vec3d & p) const
{
    return p.dist(project_onto(p));
}


CINO_INLINE
bool Segment::is_in_between(const vec3d &p) const
{
    vec3d v = second - first;
    vec3d w = p  - first;

    float cos_wv = w.dot(v);
    float cos_vv = v.dot(v);

    if (cos_wv <= 0.0)    return false;
    if (cos_vv <= cos_wv) return false;
    return true;
}

CINO_INLINE
std::vector<Plane> Segment::to_planes() const
{
    vec3d d = dir();

    vec3d n0(-d.y(),  d.x(),     0);
    vec3d n1(-d.z(),      0, d.x());
    vec3d n2(     0, -d.z(), d.y());

    std::vector<Plane> planes;
    if (n0.length() > 0) planes.push_back(Plane(first, n0));
    if (n1.length() > 0) planes.push_back(Plane(first, n1));
    if (n2.length() > 0) if (planes.size() < 2) planes.push_back(Plane(first, n2));
    assert(planes.size() == 2);

    return planes;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Segment::dir() const
{
    vec3d d = first-second;
    d.normalize();
    return d;
}

}
