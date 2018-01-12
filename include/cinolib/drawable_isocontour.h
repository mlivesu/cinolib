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
#ifndef CINO_DRAWABLE_ISOCONTOUR_H
#define CINO_DRAWABLE_ISOCONTOUR_H

#include <cinolib/drawable_object.h>
#include <cinolib/isocontour.h>
#include <cinolib/color.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class P = Polygon_std_attributes>
class DrawableIsocontour : public Isocontour<M,V,E,P>, public DrawableObject
{
    public:

        explicit DrawableIsocontour();
        explicit DrawableIsocontour(AbstractPolygonMesh<M,V,E,P> & m, double iso_value);
        ~DrawableIsocontour() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void  draw(const float scene_size=1) const;
        vec3d scene_center() const;
        float scene_radius() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type()  const { return DRAWABLE_CURVE; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Color color;
        float thickness;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_isocontour.cpp"
#endif

#endif // CINO_DRAWABLE_ISOCONTOUR_H
