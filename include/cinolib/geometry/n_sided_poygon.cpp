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
#include <cinolib/geometry/n_sided_poygon.h>

namespace cinolib
{

CINO_INLINE
std::vector<vec3d> n_sided_polygon(const vec3d & center,
                                   const uint    n_sides,
                                   const double  radius,
                                   const int     axis)
{
    std::vector<vec3d> verts(n_sides);

    switch (axis)
    {
        case X : verts[0] = vec3d(0,0,radius); break;
        case Y : verts[0] = vec3d(radius,0,0); break;
        case Z : verts[0] = vec3d(radius,0,0); break;
        default: assert(false);
    }

    for(uint i=1; i<n_sides; ++i)
    {
        verts[i] = verts[i-1];
        switch (axis)
        {
            case X : rotate(verts[i], vec3d(1,0,0), 2.0*M_PI/double(n_sides)); break;
            case Y : rotate(verts[i], vec3d(0,1,0), 2.0*M_PI/double(n_sides)); break;
            case Z : rotate(verts[i], vec3d(0,0,1), 2.0*M_PI/double(n_sides)); break;
            default: assert(false);
        }
    }
    for(vec3d & v : verts) v += center;
    return verts;
}

}
