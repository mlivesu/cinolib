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
#ifndef CINO_MOLLER_TRUMBORE_INTERSECTION_H
#define CINO_MOLLER_TRUMBORE_INTERSECTION_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

// Moller-Trumbore algorithm for ray/triangle intersection
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
// https://en.wikipedia.org/wiki/Möller–Trumbore_intersection_algorithm
//
// Returns true if ray and triangle intersect, false otherwise.
// The intersection point (P) is returned in two forms:
//
//     1) parametric  form (t)   : P = ray_orig + t*ray_dir
//     2) barycentric form (bary): P = bary[0]*v0 + bary[1]*v1 + bary[2]*v2
//
// Use either t or bary at your convenience. They define the same point...
//
CINO_INLINE
bool Moller_Trumbore_intersection(const vec3d  & ray_orig,
                                  const vec3d  & ray_dir,
                                  const vec3d  & v0,
                                  const vec3d  & v1,
                                  const vec3d  & v2,
                                        bool   & hits_backside, // true if ray intersects triangle from the back (according to winding order)
                                        bool   & are_coplanar,  // true if ray and triangle are coplanar (no intersection will be computed)
                                        double & t,
                                        vec3d  & bary);
}

#ifndef  CINO_STATIC_LIB
#include "Moller_Trumbore_intersection.cpp"
#endif

#endif // CINO_MOLLER_TRUMBORE_INTERSECTION_H
