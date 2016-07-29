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
#ifndef DRAW_CYLINDER_H
#define DRAW_CYLINDER_H

#include <cinolib/cinolib.h>

#ifdef __APPLE__
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <cmath>

namespace cinolib
{

template <typename vec3>
CINO_INLINE
static void cylinder(const vec3  & a,
                     const vec3  & b,
                     float         top_radius,
                     float         bottom_radius,
                     const float * color)
{
    vec3 dir     = b - a; dir.normalize();
    vec3 z       = vec3(0,0,1);
    vec3 normal  = dir.cross(z);
    double angle = acos(dir.dot(z)) * 180 / M_PI;

    glColor3fv(color);
    glPushMatrix();
    glTranslated(a[0], a[1], a[2]);
    glRotatef(-angle, normal[0], normal[1], normal[2]);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    GLUquadric *cylinder = gluNewQuadric();
    gluQuadricNormals(cylinder, GLU_SMOOTH);
    gluQuadricOrientation(cylinder, GLU_OUTSIDE);
    gluCylinder(cylinder, top_radius, bottom_radius, (a-b).length(), 10, 5);
    glPopMatrix();
}

}

#endif // DRAW_CYLINDER_H
