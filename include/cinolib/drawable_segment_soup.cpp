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
#include <cinolib/drawable_segment_soup.h>
#include <cinolib/cino_inline.h>
#include <cinolib/gl/draw_sphere.h>
#include <cinolib/gl/draw_cylinder.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
DrawableSegmentSoup::DrawableSegmentSoup()
{
    color        = Color::RED();
    thickness    = 1.0;
    use_gl_lines = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableSegmentSoup::draw(const float scene_size) const
{
    if (!use_gl_lines)
    {
        float cylind_rad = scene_size * 0.002 * thickness;

        for(vec3d p : *this) sphere<vec3d>(p, cylind_rad, color.rgba);

        for(uint i=0; i<size()/2; ++i)
        {
            cylinder<vec3d>(at(2*i+0), at(2*i+1), cylind_rad, cylind_rad, color.rgba);
        }
    }
    else
    {
        glColor3fv(color.rgba);
        for(uint i=0; i<size()/2; ++i)
        {
            vec3d a = at(2*i+0);
            vec3d b = at(2*i+1);
            glBegin(GL_LINES);
                glVertex3d(a.x(), a.y(), a.z());
                glVertex3d(b.x(), b.y(), b.z());
            glEnd();
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableSegmentSoup::set_cheap_rendering(const bool b)
{
    use_gl_lines = b;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableSegmentSoup::set_color(const Color & c)
{
    color = c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void DrawableSegmentSoup::set_thickness(float t)
{
    thickness = t;
}

}
