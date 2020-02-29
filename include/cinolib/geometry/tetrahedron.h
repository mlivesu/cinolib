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
#ifndef CINO_TETRAHEDRON_H
#define CINO_TETRAHEDRON_H

#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/spatial_data_structure_item.h>

namespace cinolib
{

class Tetrahedron : public SpatialDataStructureItem
{
    public:

        Tetrahedron(const uint id, const vec3d v[4])
        {
            this->v[0] = v[0];
            this->v[1] = v[1];
            this->v[2] = v[2];
            this->v[3] = v[3];
            this->id = id;
        }

       ~Tetrahedron() {}

        // Implement SpatialDataStructureItem interface ::::::::::::::::::::::::::

        ItemType item_type                () const override;
        AABB     aabb                     () const override;
        vec3d    point_closest_to         (const vec3d & p) const override;
        bool     intersects_ray           (const vec3d & p, const vec3d & dir, double & t, vec3d & pos) const override;
        void     barycentric_coordinates  (const vec3d & p, double bc[]) const override;
        bool     contains_exact           (const vec3d & p, bool strict) const override;
        bool     intersects_segment_exact (const vec3d   s[]) const override;
        bool     intersects_triangle_exact(const vec3d   t[]) const override;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d v[4];
};

}

#ifndef  CINO_STATIC_LIB
#include "tetrahedron.cpp"
#endif

#endif // CINO_TETRAHEDRON_H
