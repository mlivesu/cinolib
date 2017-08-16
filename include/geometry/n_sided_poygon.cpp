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
#include "n_sided_poygon.h"

namespace cinolib
{

CINO_INLINE
Trimesh<> n_sided_polygon(const vec3d  & center,
                          const uint     n_sides,
                          const double   radius,
                          const vec3d  & n)
{
    std::vector<double> coords;
    std::vector<uint> tris;

    // center
    coords.push_back(0);
    coords.push_back(0);
    coords.push_back(0);

    std::vector<vec3d> boundary(n_sides);
    boundary[0] = vec3d(radius,0,0);
    for(uint i=1; i<n_sides; ++i)
    {
        boundary[i] = boundary[i-1];
        rotate(boundary[i], vec3d(0,1,0), 2.0*M_PI/double(n_sides));
    }

    for(uint i=0; i<n_sides; ++i)
    {
        coords.push_back(boundary[i].x());
        coords.push_back(boundary[i].y());
        coords.push_back(boundary[i].z());

        tris.push_back(0);
        tris.push_back(1+i);
        tris.push_back(1+(i+1)%n_sides);
    }

    logger.disable();
    Trimesh<> m(coords, tris);
    logger.enable();

    m.translate(center);

    vec3d  Y(0,1,0);
    vec3d  axis  = Y.cross(n); axis.normalize();
    double angle = acos(Y.dot(n));
    if (!std::isnan(angle) && std::fabs(angle * 180.0/M_PI) > 1)
    {
        m.rotate(axis, angle);
    }

    return m;
}

}
