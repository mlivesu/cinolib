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
#ifndef CINO_INTERSECTION_H
#define CINO_INTERSECTION_H

#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/geometry/line.h>
#include <cinolib/geometry/ray.h>
#include <cinolib/geometry/segment.h>

namespace cinolib
{

CINO_INLINE
bool segment2D_intersection(const vec2d        & s0_beg,
                            const vec2d        & s0_end,
                            const vec2d        & s1_beg,
                            const vec2d        & s1_end,
                            std::vector<vec2d> & inters);


CINO_INLINE
bool line_triangle_intersection(const Line  & l,
                                const vec3d & V0,
                                const vec3d & V1,
                                const vec3d & V2,
                                      vec3d & inters,
                                const double  tol = 1e-5);

CINO_INLINE
bool ray_triangle_intersection(const Ray   & r,
                               const vec3d & V0,
                               const vec3d & V1,
                               const vec3d & V2,
                                     vec3d & inters,
                               const double  tol = 1e-5);

CINO_INLINE
bool least_squares_intersection(const std::vector<Plane> & planes,
                                      vec3d              & inters);

CINO_INLINE
bool intersection(const Ray     & r,
                  const Segment & s,
                        vec3d   & inters,
                  const double    tol = 1e-7);

}

#ifndef  CINO_STATIC_LIB
#include "intersection.cpp"
#endif

#endif // CINO_INTERSECTION_H
