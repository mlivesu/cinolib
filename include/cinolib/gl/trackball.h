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
#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

/* Tools to map mouse trajectories in a 2D canvas into rotations of a sphere.
 *
 * trackball_to_sphere takes in input a click point on the screen and maps it
 * into a point on a sphere
 *
 * trackball_to_rotations takes in input two points on the sphere, and returns
 * the corresponding rotation, with axis angle representation.
 *
 * The trackball radius controls the speed of rotation. Smaller radii ensure a
 * faster rotation. The standrd algorithm maps to the unit sphere. I personally
 * find a smaller sphere with radius 0.5 to be better in terms of usability.
 *
 *  References:
 *
 *      Arcball rotation control
 *      Ken Shoemake
 *      Graphics Gems IV, 1993
 *
 *      http://courses.cms.caltech.edu/cs171/assignments/hw3/hw3-notes/notes-hw3.html
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d trackball_to_sphere(const vec2d  & p,             // click point (screen coordinates)
                          const double   width,         // window width
                          const double   height,        // window height
                          const double   radius = 0.5); // trackball radius

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void trackball_to_rotations(const vec3d  & p0,            // input sphere point #1
                            const vec3d  & p1,            // input sphere point #2
                                  vec3d  & axis,          // output rotation axis
                                  double & angle,         // output rotation angle (degrees)
                            const double   radius = 0.5); // trackball radius

}

#ifndef  CINO_STATIC_LIB
#include "trackball.cpp"
#endif

#endif // TRACKBALL_H
