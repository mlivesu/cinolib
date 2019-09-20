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

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit Plane(const vec3d & point  = vec3d(0,0,0),
                       const vec3d & normal = vec3d(0,0,1));

        explicit Plane(const vec3d & p0,
                       const vec3d & p1,
                       const vec3d & p2);

        explicit Plane(const std::vector<vec3d> & samples); // best fitting plane

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double a() const { return n.x(); }
        double b() const { return n.y(); }
        double c() const { return n.z(); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_plane(const vec3d & point,
                       const vec3d & normal);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double operator[](const vec3d & p) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double point_plane_dist_signed(const vec3d & p) const;
        double point_plane_dist       (const vec3d & p) const;
        vec3d  project_onto           (const vec3d & p) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Plane & plane);

}

#ifndef  CINO_STATIC_LIB
#include "plane.cpp"
#endif

#endif // CINO_PLANE_H
