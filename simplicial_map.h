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
#ifndef SIMPLICIAL_MAP_H
#define SIMPLICIAL_MAP_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{


/* Compute the affine map T between two tetrahedra A and B,
 * that is:
 *
 *          B = T(A) = M * A + t
 *
 * with M being a 3x3 matrix encoding the rotation/scaling
 * part of the mapping and t be a translation.
*/
CINO_INLINE
void affine_simplicial_map(const vec3d & A0,
                           const vec3d & A1,
                           const vec3d & A2,
                           const vec3d & A3,
                           const vec3d & B0,
                           const vec3d & B1,
                           const vec3d & B2,
                           const vec3d & B3,
                           double        m[3][3],
                           double        t[3]);
}

#ifndef  CINO_STATIC_LIB
#include "simplicial_map.cpp"
#endif

#endif // SIMPLICIAL_MAP_H
