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
#include "drawable_aabb.h"

#ifdef __APPLE__
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace cinolib
{

CINO_INLINE
DrawableAABB::DrawableAABB(const vec3d min, const vec3d max)
    : AABB(min, max)
{
    verts = this->corners();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
DrawableAABB::DrawableAABB(const std::vector<vec3d> &p_list, const double scaling_factor)
    : AABB(p_list, scaling_factor)
{
    verts = this->corners();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
DrawableAABB::DrawableAABB(const std::vector<AABB> &b_list, const double scaling_factor)
    : AABB(b_list, scaling_factor)
{
    verts = this->corners();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableAABB::draw(const float ) const
{
    glLineWidth(thickness);
    glDisable(GL_LIGHTING);
    glColor3fv(color.rgba);
    glBegin(GL_LINES);
        glVertex3dv(verts.at(0).ptr()); glVertex3dv(verts.at(1).ptr());
        glVertex3dv(verts.at(0).ptr()); glVertex3dv(verts.at(3).ptr());
        glVertex3dv(verts.at(0).ptr()); glVertex3dv(verts.at(4).ptr());
        glVertex3dv(verts.at(1).ptr()); glVertex3dv(verts.at(2).ptr());
        glVertex3dv(verts.at(1).ptr()); glVertex3dv(verts.at(5).ptr());
        glVertex3dv(verts.at(2).ptr()); glVertex3dv(verts.at(3).ptr());
        glVertex3dv(verts.at(2).ptr()); glVertex3dv(verts.at(6).ptr());
        glVertex3dv(verts.at(3).ptr()); glVertex3dv(verts.at(7).ptr());
        glVertex3dv(verts.at(4).ptr()); glVertex3dv(verts.at(5).ptr());
        glVertex3dv(verts.at(4).ptr()); glVertex3dv(verts.at(7).ptr());
        glVertex3dv(verts.at(5).ptr()); glVertex3dv(verts.at(6).ptr());
        glVertex3dv(verts.at(6).ptr()); glVertex3dv(verts.at(7).ptr());
    glEnd();
    glEnable(GL_LIGHTING);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d DrawableAABB::scene_center() const
{
    return this->center();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
float DrawableAABB::scene_radius() const
{
    return this->diag();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableAABB::set_color(const Color & c)
{
    color = c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableAABB::set_thickness(float t)
{
    thickness = t;
}

}
