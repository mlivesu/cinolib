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
#include "quality.h"

namespace cinolib
{

CINO_INLINE
double tet_scaled_jacobian(const vec3d & a, const vec3d & b, const vec3d & c, const vec3d & d)
{
    vec3d L0 = b - a;
    vec3d L1 = c - b;
    vec3d L2 = a - c;
    vec3d L3 = d - a;
    vec3d L4 = d - b;
    vec3d L5 = d - c;

    double L0_length = L0.length();
    double L1_length = L1.length();
    double L2_length = L2.length();
    double L3_length = L3.length();
    double L4_length = L4.length();
    double L5_length = L5.length();

    double J = (L2.cross(L0)).dot(L3);

    double lambda[] =
    {
        L0_length * L2_length * L3_length,
        L0_length * L1_length * L4_length,
        L1_length * L2_length * L5_length,
        L3_length * L4_length * L5_length,
        J
    };

    double max = *std::max_element(lambda, lambda + 5);

    if (max < DBL_MIN) return -1.0;
    return  (J * sqrt_2 / max);
}

CINO_INLINE
double tet_unsigned_volume(const vec3d & a, const vec3d & b, const vec3d & c, const vec3d & d)
{
    vec3d L0 = b - a;
    vec3d L2 = a - c;
    vec3d L3 = d - a;

    return fabs((L2.cross(L0)).dot(L3) / 6.0);
}

}
