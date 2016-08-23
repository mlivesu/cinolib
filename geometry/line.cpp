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
#include <cinolib/geometry/line.h>

namespace cinolib
{

CINO_INLINE
Line::Line(const vec3d & p0, const vec3d & p1)
{
    assert(p0.dist(p1) > 0);
    this->p0 = p0;
    this->p1 = p1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

std::vector<Plane> Line::to_planes() const
{
    vec3d d = dir();

    vec3d n0(-d.y(),  d.x(),     0);
    vec3d n1(-d.z(),      0, d.x());
    vec3d n2(     0, -d.z(), d.y());

    std::vector<Plane> planes;
    if (n0.length() > 0) planes.push_back(Plane(p0, n0));
    if (n1.length() > 0) planes.push_back(Plane(p0, n1));
    if (n2.length() > 0) if (planes.size() < 2) planes.push_back(Plane(p0, n2));
    assert(planes.size() == 2);

    return planes;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

vec3d Line::dir() const
{
    vec3d d = p0-p1;
    d.normalize();
    return d;
}

}
