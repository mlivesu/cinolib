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
#ifndef QUALITY
#define QUALITY

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>
#include <float.h>


/*
 * Scaled Jacobian and Volume computations are all based on:
 *
 * The Verdict Geometric Quality Library
 * SANDIA Report SAND2007-1751
 *
*/


namespace cinolib
{

CINO_INLINE
double tet_scaled_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3);


CINO_INLINE
double tet_unsigned_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3);


CINO_INLINE
double hex_scaled_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

CINO_INLINE
double hex_unsigned_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);
}


#ifndef  CINO_STATIC_LIB
#include "quality.cpp"
#endif

#endif // QUALITY
