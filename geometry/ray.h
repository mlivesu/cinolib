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
#ifndef RAY_H
#define RAY_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/plane.h>

namespace cinolib
{

class Ray
{
    public:

        Ray(const vec3d & start, const vec3d & dir);

        std::vector<Plane> to_planes() const;

        const vec3d & dir() const;

        bool on_positive_half_space(const vec3d & p) const;

        double dist_to_point(const vec3d & p) const;

    protected:

        vec3d start, direction;
};

}

#ifndef  CINO_STATIC_LIB
#include "ray.cpp"
#endif

#endif // RAY_H
