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
#ifndef CINO_BBOX_H
#define CINO_BBOX_H

#include <cinolib/min_max_inf.h>
#include <cinolib/geometry/vec3.h>
#include <algorithm>

namespace cinolib
{

// TODO: make it dimension independent (d as template argument)
class Bbox
{
    public:

        explicit Bbox(const vec3d min = vec3d( inf_double,  inf_double,  inf_double),
                      const vec3d max = vec3d(-inf_double, -inf_double, -inf_double))
        : min(min), max(max) {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit Bbox(const std::vector<vec3d> & p_list);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void               reset();
        void               update(const std::vector<vec3d> & p_list);
        vec3d              center()    const;
        double             diag()      const;
        double             delta_x()   const;
        double             delta_y()   const;
        double             delta_z()   const;
        vec3d              delta()     const;
        double             min_entry() const;
        double             max_entry() const;
        bool               contains(const vec3d & p, const bool strictly_contains = true)  const;
        bool               intersects(const Bbox & box) const;
        std::vector<vec3d> corners(const double scaling_factor = 1.0) const;
        std::vector<uint>  tris()  const;
        std::vector<uint>  quads() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d min, max;
};

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb);

}

#ifndef  CINO_STATIC_LIB
#include "bbox.cpp"
#endif

#endif // CINO_BBOX_H
