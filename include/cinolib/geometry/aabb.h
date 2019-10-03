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
#ifndef CINO_AABB_H
#define CINO_AABB_H

#include <cinolib/min_max_inf.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

class AABB
{
    public:

        explicit AABB(const std::vector<vec3d> & p_list, const double scaling_factor = 1.0); // AABB that contains all verts in p_list

        explicit AABB(const std::vector<AABB> & b_list, const double scaling_factor = 1.0); // AABB that contains all AABBs in b_list

        explicit AABB(const vec3d min = vec3d( inf_double,  inf_double,  inf_double),
                      const vec3d max = vec3d(-inf_double, -inf_double, -inf_double));

        virtual ~AABB() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void reset();
        void update(const std::vector<vec3d> & p_list, const double scaling_factor = 1.0);
        void scale(const double s);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double diag()    const;
        vec3d  center()  const;
        vec3d  delta()   const;
        double delta_x() const;
        double delta_y() const;
        double delta_z() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double min_entry() const;
        double max_entry() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  point_closest_to(const vec3d & p) const;
        double dist_sqrd       (const vec3d & p) const;
        double dist            (const vec3d & p) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool contains(const vec3d & p, const bool strict = false) const;
        bool intersects_box(const AABB  & box, const bool strict = false) const;
        bool intersects_ray(const vec3d & p, const vec3d & dir, double & t_min, vec3d & pos) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<vec3d> corners(const double scaling_factor = 1.0) const;
        std::vector<uint>  tris()  const;
        std::vector<uint>  quads() const;
        std::vector<uint>  edges() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d min, max;
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const AABB & bb);

}

#ifndef  CINO_STATIC_LIB
#include "aabb.cpp"
#endif

#endif // CINO_AABB_H
