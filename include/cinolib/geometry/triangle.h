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
#ifndef CINO_TRIANGLE_H
#define CINO_TRIANGLE_H

#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/aabb.h>
#include <cinolib/geometry/spatial_data_structure_item.h>
#include <cinolib/geometry/triangle_utils.h>

namespace cinolib
{

class Triangle : public SpatialDataStructureItem
{
    public:

        Triangle(const std::vector<vec3d> v) : v0(v.at(0)), v1(v.at(1)), v2(v.at(2)) {}

        Triangle(const vec3d & v0,
                 const vec3d & v1,
                 const vec3d & v2) : v0(v0), v1(v1), v2(v2) {}

       ~Triangle() {}

        // Implement SpatialDataStructureItem interface ::::::::::::::::::::::::::

        ItemType item_type() const;
        AABB     aabb() const;
        vec3d    point_closest_to(const vec3d & p) const;
        bool     intersects_ray(const vec3d & p, const vec3d & dir, double & t, vec3d & pos) const;
        void     barycentric_coordinates(const vec3d & p, double bc[]) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d v0, v1, v2;
};

}

#ifndef  CINO_STATIC_LIB
#include "triangle.cpp"
#endif

#endif // CINO_TRIANGLE_H
