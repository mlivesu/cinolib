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
#ifndef CINO_TANGENT_SPACE_H
#define CINO_TANGENT_SPACE_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

CINO_INLINE
bool tangent_frame(const vec3d & p0, // 3D coordinates of triangle <p0,p1,p2>
                   const vec3d & p1,
                   const vec3d & p2,
                         vec3d & u,  // axes of a 2D orthonormal frame in the supporting
                         vec3d & v); // plane of triangle <p0,p1,p2>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool tangent_space_2d_coords(const vec3d & p0,  // 3D coordinates of triangle <p0,p1,p2>
                             const vec3d & p1,
                             const vec3d & p2,
                                   vec2d & P0,  // 2D coordinates of triangle <P0,P1,P2>
                                   vec2d & P1,  // in tangent space
                                   vec3d & P2);
}

#ifndef  CINO_STATIC_LIB
#include "tangent_space.cpp"
#endif

#endif // CINO_TANGENT_SPACE_H
