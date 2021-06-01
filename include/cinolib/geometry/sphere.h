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
#ifndef CINO_SPHERE_H
#define CINO_SPHERE_H

#include <iostream>
#include <cinolib/geometry/spatial_data_structure_item.h>

namespace cinolib
{

class Sphere : public SpatialDataStructureItem
{
    public:

        Sphere(const uint id, const vec3d & c, const double r)
        {
            this->c   = c;
            this->r   = r;
            this->id  = id;
            item_type = SPHERE;
            double hr = r*0.5;
            aabb.push( c - vec3d(hr,hr,hr));
            aabb.push( c + vec3d(hr,hr,hr));
        }

        ~Sphere() {}

        // Implement SpatialDataStructureItem interface ::::::::::::::::::::::::::

        vec3d    point_closest_to       (const vec3d & p) const override;
        bool     intersects_ray         (const vec3d & p, const vec3d & dir, double & t, vec3d & pos) const override;
        void     barycentric_coordinates(const vec3d & p, double bc[]) const override;
        bool     contains               (const vec3d & p, const bool strict) const override;
        bool     intersects_segment     (const vec3d   s[], const bool ignore_if_valid_complex) const override;
        bool     intersects_triangle    (const vec3d   t[], const bool ignore_if_valid_complex) const override;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  c; // center
        double r; // radius
};

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Sphere & p);

}

#ifndef  CINO_STATIC_LIB
#include "sphere.cpp"
#endif

#endif // CINO_SPHERE_H
