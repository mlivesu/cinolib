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
#ifndef CINO_RAY_H
#define CINO_RAY_H

#include <vector>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/plane.h>

namespace cinolib
{

class Ray
{
    public:

        explicit Ray(const vec3d & start, const vec3d & dir);

        std::vector<Plane> to_planes() const;

        const vec3d & dir() const;

        const vec3d & begin() const;

        bool on_positive_half_space(const vec3d & p) const;

        double dist_to_point(const vec3d & p) const;

    protected:

        vec3d start, direction;
};

}

#ifndef  CINO_STATIC_LIB
#include "ray.cpp"
#endif

#endif // CINO_RAY_H
