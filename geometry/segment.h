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
#ifndef SEGMENT_H
#define SEGMENT_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/plane.h>
#include <vector>

namespace cinolib
{

class Segment : public std::pair<vec3d,vec3d>
{
    public:

        Segment(const vec3d & P0, const vec3d & P1);

        std::vector<Plane> to_planes() const;

        vec3d dir() const;

        double operator[](const vec3d & p) const;

        vec3d project_onto(const vec3d & p) const;

        double dist_to_point(const vec3d & p) const;

        bool is_in_between(const vec3d & p) const;
};

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Segment & s);

}

#ifndef  CINO_STATIC_LIB
#include "segment.cpp"
#endif

#endif // SEGMENT_H
