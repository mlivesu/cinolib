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
#include <cinolib/drawable_isosurface.h>
#include <cinolib/cino_inline.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
DrawableIsosurface<M,V,E,F,P>::DrawableIsosurface() : Isosurface<M,V,E,F,P>()
{
    color = Color::RED();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
DrawableIsosurface<M,V,E,F,P>::DrawableIsosurface(const Tetmesh<M,V,E,F,P> & m, const double iso_value)
    : Isosurface<M,V,E,F,P>(m, iso_value)
{
    color = Color::RED();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void DrawableIsosurface<M,V,E,F,P>::draw(const float) const
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    for(uint i=0; i<this->tris.size(); i+=3)
    {
        uint vid0 = this->tris[i+0];
        uint vid1 = this->tris[i+1];
        uint vid2 = this->tris[i+2];

        glBegin(GL_TRIANGLES);
        glColor3fv(color.rgba);
        glNormal3dv(this->norms.at(i/3).ptr());
        glVertex3dv(this->verts.at(vid0).ptr());
        glVertex3dv(this->verts.at(vid1).ptr());
        glVertex3dv(this->verts.at(vid2).ptr());
        glEnd();
    }

    glColor3f(1.0,1.0,1.0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d DrawableIsosurface<M,V,E,F,P>::scene_center() const
{
    return vec3d();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
float DrawableIsosurface<M,V,E,F,P>::scene_radius() const
{
    return 0.0;
}

}
