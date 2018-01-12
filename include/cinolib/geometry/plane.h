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
#ifndef CINO_PLANE_H
#define CINO_PLANE_H

#include <iostream>
#include <vector>
#include <cinolib/geometry/vec3.h>

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

        explicit Plane(const vec3d & p0,
                       const vec3d & p1,
                       const vec3d & p2);

        explicit Plane(const vec3d & point,
                       const vec3d & normal);

        explicit Plane(const std::vector<vec3d> & samples); // best fitting plane

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
