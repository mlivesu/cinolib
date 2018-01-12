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
#ifndef CINO_POLYGON_KERNEL_H
#define CINO_POLYGON_KERNEL_H

#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/vec2.h>

namespace cinolib
{

/* Computes the kernel of a 2D polygon (the locus of points which are
 * visible from any corner). The procedure is based on the Boost Polygon
 * Library.
 *
 * For each oriented edge in the polygon we generate a big enough quad
 * representing the positive half-space, and we then intersect all the
 * positive half-spaces together using boolean operations.
 *
 * Polygon representation: a polygon is a vector of points ordered CCW to
 * form a closed loop (first and last points are adjacent).
 *
 * The function returns the area of the kernel (i.e. 0 means the polygon has no kernel)
*/

CINO_INLINE
double polygon_kernel(const std::vector<vec2d> & poly,
                            std::vector<vec2d> & kernel);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double polygon_kernel(const std::vector<vec3d> & poly,     // will discard z component
                            std::vector<vec3d> & kernel);  // z component will be zero

}

#ifndef  CINO_STATIC_LIB
#include "polygon_kernel.cpp"
#endif

#endif // CINO_POLYGON_KERNEL_H
