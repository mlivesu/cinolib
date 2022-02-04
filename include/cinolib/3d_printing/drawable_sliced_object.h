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

        ~DrawableSlicedObj(){}

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
