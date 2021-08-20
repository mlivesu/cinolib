/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
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
#include "trackball.h"
#include <cinolib/clamp.h>
#include <cinolib/deg_rad.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d trackball_to_sphere(const vec2d  & p,      // click point (screen coordinates)
                          const double   width,  // window width
                          const double   height, // window height
                          const double   radius) // trackball radius
{
    // normalized display coordinates
    double x_val =  (2.0*p.x() - width )/width;
    double y_val = -(2.0*p.y() - height)/height;
    double norm2 = x_val*x_val + y_val*y_val;
    return vec3d(x_val, y_val, (norm2 < 0.5*radius) ? sqrt(radius - norm2) : 0.5*radius/sqrt(norm2));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void trackball_to_rotations(const vec3d  & p0,     // input sphere point #1
                            const vec3d  & p1,     // input sphere point #2
                                  vec3d  & axis,   // output rotation axis
                                  double & angle,  // output rotation angle (degrees)
                            const double   radius) // trackball radius
{
    axis = p0.cross(p1);
    if(axis.norm_sqrd()<1e-7) axis = vec3d(1,0,0);
    axis.normalize();
    double t = p0.dist(p1)*0.5/radius;
    t = clamp(t, -1.0, 1.0);
    angle = 2.0*asin(t);
    angle = to_deg(angle);
}

}
