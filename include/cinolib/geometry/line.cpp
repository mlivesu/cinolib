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
#include "cinolib/geometry/line.h"
#include <cinolib/cino_inline.h>

namespace cinolib
{

CINO_INLINE
Line::Line(const vec3d & p0, const vec3d & p1)
{
    assert(p0.dist(p1) > 0);
    this->p0 = p0;
    this->p1 = p1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<Plane> Line::to_planes() const
{
    vec3d d = dir();

    vec3d n0(-d.y(),  d.x(),     0);
    vec3d n1(-d.z(),      0, d.x());
    vec3d n2(     0, -d.z(), d.y());

    std::vector<Plane> planes;
    if (n0.length() > 0) planes.push_back(Plane(p0, n0));
    if (n1.length() > 0) planes.push_back(Plane(p0, n1));
    if (n2.length() > 0) if (planes.size() < 2) planes.push_back(Plane(p0, n2));
    assert(planes.size() == 2);

    return planes;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Line::dir() const
{
    vec3d d = p0-p1;
    d.normalize();
    return d;
}

}
