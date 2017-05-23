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
#ifndef CINO_PLANE_H
#define CINO_PLANE_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>

#include <ostream>

namespace cinolib
{

// represents a plane with equation
// a*x + b*y + c*z = d
// or, using the Hessian Normal Form (https://en.wikipedia.org/wiki/Hesse_normal_form)
// n dot (x,y,z) - d = 0
//
class Plane
{
    public:

        vec3d  n; // plane normal (i.e. a,b,c, coefficients of the plane equation)
        double d; // d coefficient of the plane equation
        vec3d  p; // any point on the plane (useful for point_plane_dist)

        Plane(const vec3d & p0,
              const vec3d & p1,
              const vec3d & p2);

        Plane(const vec3d & point,
              const vec3d & normal);

        Plane(const std::vector<vec3d> & samples); // best fitting plane

        double a() const { return n.x(); }
        double b() const { return n.y(); }
        double c() const { return n.z(); }

        void set_plane(const vec3d & point,
                       const vec3d & normal);

        double operator[](const vec3d & p) const;

        double point_plane_dist(const vec3d & p) const;

        vec3d project_onto(const vec3d & p);
};

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Plane & plane);

}

#ifndef  CINO_STATIC_LIB
#include "plane.cpp"
#endif

#endif // CINO_PLANE_H
