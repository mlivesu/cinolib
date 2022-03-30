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

#include <cinolib/geometry/spatial_data_structure_item.h>

namespace cinolib
{

class Triangle : public SpatialDataStructureItem
{
    public:

        Triangle(const uint id, const vec3d v[3])
        {
            this->v[0] = v[0];
            this->v[1] = v[1];
            this->v[2] = v[2];
            this->id   = id;
            item_type  = TRIANGLE;
            aabb.push(v[0]);
            aabb.push(v[1]);
            aabb.push(v[2]);
        }

        Triangle(const uint id, const vec3d & v0, const vec3d & v1, const vec3d & v2)
        {
            this->v[0] = v0;
            this->v[1] = v1;
            this->v[2] = v2;
            this->id   = id;
            item_type  = TRIANGLE;
            aabb.push(v0);
            aabb.push(v1);
            aabb.push(v2);
        }

        ~Triangle(){}

        // Implement SpatialDataStructureItem interface ::::::::::::::::::::::::::

        vec3d    point_closest_to       (const vec3d & p) const override;
        bool     intersects_ray         (const vec3d & p, const vec3d & dir, double & t, vec3d & pos) const override;
        void     barycentric_coordinates(const vec3d & p, double bc[]) const override;
        bool     contains               (const vec3d & p, const bool strict) const override;
        bool     intersects_segment     (const vec3d   s[], const bool ignore_if_valid_complex) const override;
        bool     intersects_triangle    (const vec3d   t[], const bool ignore_if_valid_complex) const override;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d v[3];
};

}

#ifndef  CINO_STATIC_LIB
#include "triangle.cpp"
#endif

#endif // CINO_TRIANGLE_H
