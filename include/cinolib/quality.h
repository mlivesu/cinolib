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
#ifndef CINO_QUALITY
#define CINO_QUALITY

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

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

#endif // CINO_QUALITY
