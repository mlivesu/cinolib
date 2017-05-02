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
#ifndef CINO_DRAW_SPHERE_H
#define CINO_DRAW_SPHERE_H

#include <cinolib/cinolib.h>

#ifdef __APPLE__
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace cinolib
{

template <typename vec3>
CINO_INLINE
static void sphere(const vec3  & center,
                   float         radius,
                   const float * color)
{
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glColor3fv(color);
    glPushMatrix();
    glTranslated(center[0], center[1], center[2]);
    GLUquadric *sphere = gluNewQuadric();
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluQuadricOrientation(sphere, GLU_OUTSIDE);
    gluSphere(sphere, radius, 30, 30);
    glPopMatrix();
}

}

#endif // CINO_DRAW_SPHERE_H
