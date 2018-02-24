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
#ifndef CINO_RAY_TRIANGLE_INTERSECTION_H
#define CINO_RAY_TRIANGLE_INTERSECTION_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

/* Computes the intersection between a ray and a triangle in R^3.
 * The ray is defined by an origin point in space and a direction;
 * the triangle is definded by its three vertices. Returns true if
 * ray and triangle intersect, false otherwise.
 *
 * WARNING: if ray and triangle are coplanar the function will return
 * false (and no intersection point will be computed).
*/

CINO_INLINE
bool ray_triangle_intersection(const vec3d  & ray_orig,
                               const vec3d  & ray_dir,
                               const vec3d  & v0,
                               const vec3d  & v1,
                               const vec3d  & v2,
                                     vec3d  & res);
}

#ifndef  CINO_STATIC_LIB
#include "ray_triangle_intersection.cpp"
#endif

#endif // CINO_RAY_TRIANGLE_INTERSECTION_H
