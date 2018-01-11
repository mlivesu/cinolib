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
#ifndef CINO_SIMPLICIAL_MAP_H
#define CINO_SIMPLICIAL_MAP_H

#include <cinolib/cino_inline.h>
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

#endif // CINO_SIMPLICIAL_MAP_H
