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
#ifndef CINO_SEGMENT_H
#define CINO_SEGMENT_H

#include <iostream>
#include <utility>
#include <cinolib/bbox.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/geometry/spatial_data_structure_item.h>

namespace cinolib
{

class Segment : public SpatialDataStructureItem
{
    public:

        Segment(const vec3d & v0,
                const vec3d & v1) : v0(v0), v1(v1) {}

        Segment(const std::pair<vec3d,vec3d> & p) : v0(p.first), v1(p.second) {}

        ~Segment() {}

        // Implement SpatialDataStructureItem interface ::::::::::::::::::::::::::

        ItemType item_type() const;
        Bbox     aabb() const;
        vec3d    point_closest_to(const vec3d & p) const;
        bool     intersects_ray(const vec3d & p, const vec3d & dir, double & t, vec3d & pos) const;
        void     barycentric_coordinates(const vec3d & p, double bc[]) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d v0, v1;
};


//class Segment : public std::pair<vec3d,vec3d>
//{
//    public:

//        explicit Segment(const vec3d & P0, const vec3d & P1);

//        std::vector<Plane> to_planes() const;

//        vec3d dir() const;

//        double operator[](const vec3d & p) const;

//        vec3d project_onto(const vec3d & p) const;

//        double dist_to_point(const vec3d & p) const;

//        bool is_in_between(const vec3d & p) const;
//};

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Segment & s);

}

#ifndef  CINO_STATIC_LIB
#include "segment.cpp"
#endif

#endif // CINO_SEGMENT_H
