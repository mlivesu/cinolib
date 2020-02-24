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
#ifndef CINO_DRAW_CYLINDER_H
#define CINO_DRAW_CYLINDER_H

#ifdef CINOLIB_USES_OPENGL

#ifdef __APPLE__
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <cinolib/cino_inline.h>
#include <cinolib/pi.h>

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
    vec3   dir     = b - a; dir.normalize();
    vec3   axis    = vec3(0,0,1);
    vec3   normal  = dir.cross(axis);
    double angle   = acos(dir.dot(axis)) * 180 / M_PI;

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
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
    glColor3f(1.f,1.f,1.f);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
}

}

#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_DRAW_CYLINDER_H
