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
