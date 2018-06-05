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
#ifndef CINO_DRAWABLE_SLICED_OBJ
#define CINO_DRAWABLE_SLICED_OBJ

// Boost polygons and Triangle are used to create and tessellate slices...
#ifdef CINOLIB_USES_BOOST
#ifdef CINOLIB_USES_TRIANGLE

#include <cinolib/meshes/meshes.h>
#include <cinolib/sliced_object.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class P = Polygon_std_attributes>
class DrawableSlicedObj : public AbstractDrawablePolygonMesh<SlicedObj<M,V,E,P>>
{
    public:

        explicit DrawableSlicedObj() : SlicedObj<M,V,E,P>() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit DrawableSlicedObj(const char * filename, const double hatch_size = 0.01) : SlicedObj<M,V,E,P>(filename, hatch_size)
        {
            this->init_drawable_stuff();
            this->show_marked_edge_color(Color::BLACK());
            this->show_marked_edge_width(3.0);
            this->show_wireframe(false);
            this->updateGL();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit DrawableSlicedObj(const std::vector<std::vector<std::vector<vec3d>>> & internal_polylines,
                                   const std::vector<std::vector<std::vector<vec3d>>> & external_polylines,
                                   const std::vector<std::vector<std::vector<vec3d>>> & open_polylines,
                                   const std::vector<std::vector<std::vector<vec3d>>> & hatches,
                                   const double hatch_size = 0.01)
        : SlicedObj<M,V,E,P>(internal_polylines, external_polylines, open_polylines, hatches, hatch_size)
        {
            this->init_drawable_stuff();
            this->show_marked_edge_color(Color::BLACK());
            this->show_marked_edge_width(3.0);
            this->show_wireframe(false);
            this->updateGL();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type() const { return DRAWABLE_SLICED_OBJ; }
};

}

#endif // CINO_USES_TRIANGLE
#endif // CINO_USES_BOOST

#endif // CINO_DRAWABLE_SLICED_OBJ
