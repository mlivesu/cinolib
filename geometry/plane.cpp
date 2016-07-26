/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include "plane.h"

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Plane & plane)
{
    in << "[Plane] " << plane.n.x() << " * x + "
                     << plane.n.y() << " * y + "
                     << plane.n.z() << " * z = "
                     << plane.d << "\n";
    return in;
}


CINO_INLINE
Plane::Plane(const vec3d & p0, const vec3d & p1, const vec3d & p2)
{
    vec3d u = p1 - p0;
    vec3d v = p2 - p0;
    set_plane(p0, u.cross(v));
    assert(fabs(operator[](p0)) < 1e-10);
    assert(fabs(operator[](p1)) < 1e-10);
    assert(fabs(operator[](p2)) < 1e-10);
}


CINO_INLINE
Plane::Plane(const vec3d & point, const vec3d & normal)
{
    set_plane(point, normal);
    assert(fabs(operator[](point)) < 1e-10);
}


CINO_INLINE
void Plane::set_plane(const vec3d & point, const vec3d & normal)
{
    p = point;
    n = normal;
    n.normalize();
    d = n.dot(point);
    assert(fabs(operator[](point)) < 1e-10);
}


CINO_INLINE
double Plane::operator[](const vec3d & p) const
{
    return (n.dot(p) - d);
}


// http://mathworld.wolfram.com/Point-PlaneDistance.html (eq. 13)
//
CINO_INLINE
double Plane::point_plane_dist(const vec3d & p) const
{
    assert(fabs(n.length()-1.0) < 1e-10);
    vec3d u = p - this->p;
    double dist = fabs(u.dot(n));
    assert(dist >= 0);
    return dist;
}

}
