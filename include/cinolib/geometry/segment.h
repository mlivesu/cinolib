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
#ifndef CINO_SEGMENT_H
#define CINO_SEGMENT_H

#include <vector>
#include <iostream>
#include <utility>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/plane.h>

namespace cinolib
{

class Segment : public std::pair<vec3d,vec3d>
{
    public:

        explicit Segment(const vec3d & P0, const vec3d & P1);

        std::vector<Plane> to_planes() const;

        vec3d dir() const;

        double operator[](const vec3d & p) const;

        vec3d project_onto(const vec3d & p) const;

        double dist_to_point(const vec3d & p) const;

        bool is_in_between(const vec3d & p) const;
};

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Segment & s);

}

#ifndef  CINO_STATIC_LIB
#include "segment.cpp"
#endif

#endif // CINO_SEGMENT_H
