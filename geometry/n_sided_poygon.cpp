/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include "n_sided_poygon.h"

namespace cinolib
{

CINO_INLINE
Trimesh n_sided_polygon(const vec3d  & center,
                        const int      n_sides,
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
    for(int i=1; i<n_sides; ++i)
    {
        boundary[i] = boundary[i-1];
        rotate(boundary[i], vec3d(0,1,0), 2.0*M_PI/double(n_sides));
    }

    for(int i=0; i<n_sides; ++i)
    {
        coords.push_back(boundary[i].x());
        coords.push_back(boundary[i].y());
        coords.push_back(boundary[i].z());

        tris.push_back(0);
        tris.push_back(1+i);
        tris.push_back(1+(i+1)%n_sides);
    }

    logger.disable();
    Trimesh m(coords, tris);
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
