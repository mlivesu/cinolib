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
#include <cinolib/meshes/drawable_isosurface.h>

namespace cinolib
{

CINO_INLINE
DrawableIsosurface::DrawableIsosurface(const Tetmesh<> & m, const float iso_value) : Isosurface(m, iso_value)
{}

CINO_INLINE
void DrawableIsosurface::draw(const float) const
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    for(size_t i=0; i<tris.size(); i+=3)
    {
        int vid0     = tris[i+0];
        int vid1     = tris[i+1];
        int vid2     = tris[i+2];
        int vid0_ptr = 3 * vid0;
        int vid1_ptr = 3 * vid1;
        int vid2_ptr = 3 * vid2;

        glBegin(GL_TRIANGLES);
        glColor3f(1.0,0.0,0.0);
        glNormal3dv(&(t_norms[i]));
        glVertex3dv(&(coords[vid0_ptr]));
        glVertex3dv(&(coords[vid1_ptr]));
        glVertex3dv(&(coords[vid2_ptr]));
        glEnd();
    }
}

CINO_INLINE
vec3d DrawableIsosurface::scene_center() const
{
    return m_ptr->bbox().center();
}

CINO_INLINE
float DrawableIsosurface::scene_radius() const
{
    return m_ptr->bbox().diag();
}

}
