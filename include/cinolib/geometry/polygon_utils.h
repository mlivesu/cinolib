/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_POLYGON_UTILS_H
#define CINO_POLYGON_UTILS_H

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
bool polygon_flatten(const std::vector<vec3d> & poly3d,
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
#include "polygon_utils.cpp"
#endif

#endif // CINO_POLYGON_UTILS_H
