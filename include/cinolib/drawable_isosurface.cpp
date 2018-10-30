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
