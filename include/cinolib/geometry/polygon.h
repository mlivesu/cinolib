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
#ifndef CINO_POLYGON_H
#define CINO_POLYGON_H

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/vec2.h>

/*
 * Utilities for convex or concave, simply connected, non self-intersecting polygons
*/

namespace cinolib
{

CINO_INLINE
double polygon_signed_area(const std::vector<vec2d> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double polygon_unsigned_area(const std::vector<vec2d> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double polygon_is_CCW(const std::vector<vec2d> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double polygon_is_convex(const std::vector<vec2d> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d polygon_normal(const std::vector<vec3d> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// project the polygon onto its best fitting plane,
// align with XY, and drop Z coordinate
//
// NOTE: flattening does not preserve winding!
//
CINO_INLINE
void polygon_flatten(const std::vector<vec3d> & poly3d,
                           std::vector<vec2d> & poly2d);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Implementation of the ear-cut triangulation algorithm
//
CINO_INLINE
bool polygon_triangulate(std::vector<vec2d> & poly, std::vector<uint> & tris);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool polygon_triangulate(std::vector<vec3d> & poly, std::vector<uint> & tris);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// This is the fundamental building block of the ear cut algorithm for polygon triangulation
// http://cgm.cs.mcgill.ca/~godfried/teaching/cg-projects/97/Ian/algorithm1.html
//
CINO_INLINE
int polygon_find_ear(const std::vector<vec2d> & poly);

}

#ifndef  CINO_STATIC_LIB
#include "polygon.cpp"
#endif

#endif // CINO_POLYGON_H
