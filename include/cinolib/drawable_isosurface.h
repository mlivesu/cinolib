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
#ifndef CINO_DRAWABLE_ISOSURFACE_H
#define CINO_DRAWABLE_ISOSURFACE_H

#include <cinolib/drawable_object.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/isosurface.h>
#include <cinolib/meshes/tetmesh.h>
#include <cinolib/color.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class F = Polygon_std_attributes,
         class P = Polyhedron_std_attributes>
class DrawableIsosurface : public Isosurface<M,V,E,F,P>, public DrawableObject
{
    public:

        explicit DrawableIsosurface();
        explicit DrawableIsosurface(const Tetmesh<M,V,E,F,P> & m, const double iso_value);

        ~DrawableIsosurface(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void       draw(const float scene_size=1) const;
        vec3d      scene_center() const;
        float      scene_radius() const;
        ObjectType object_type()  const { return DRAWABLE_ISOSURFACE; }
        Color      color;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_isosurface.cpp"
#endif

#endif // CINO_DRAWABLE_ISOSURFACE_H
