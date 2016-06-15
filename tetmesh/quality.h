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

#include "../cino_inline.h"
#include "../vec3.h"
#include <float.h>


/* Scaled Jacobian and Volume computation are both based on:
 *
 * The Verdict Geometric Quality Library
 * SANDIA Report SAND2007-1751
 *
*/


namespace cinolib
{

static const double sqrt_2 = 1.414213562373095;


CINO_INLINE
double tet_scaled_jacobian(const vec3d & a, const vec3d & b, const vec3d & c, const vec3d & d);

CINO_INLINE
double tet_unsigned_volume(const vec3d & a, const vec3d & b, const vec3d & c, const vec3d & d);

}


#ifndef  CINO_STATIC_LIB
#include "quality.cpp"
#endif


#endif // QUALITY
