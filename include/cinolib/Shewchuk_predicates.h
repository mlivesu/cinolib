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
#ifndef CINO_SHEWCHUK_PREDICATES
#define CINO_SHEWCHUK_PREDICATES

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

CINO_INLINE double orient2d    (double const * pa, double const * pb, double const * pc);
CINO_INLINE double orient2d    (const  vec2d & pa, const  vec2d & pb, const  vec2d & pc);
CINO_INLINE double orient2dfast(double const * pa, double const * pb, double const * pc);
CINO_INLINE double orient2dfast(const  vec2d & pa, const  vec2d & pb, const  vec2d & pc);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE double orient3d    (double const * pa, double const * pb, double const * pc, double const * pd);
CINO_INLINE double orient3d    (const  vec3d & pa, const  vec3d & pb, const  vec3d & pc, const  vec3d & pd);
CINO_INLINE double orient3dfast(double const * pa, double const * pb, double const * pc, double const * pd);
CINO_INLINE double orient3dfast(const  vec3d & pa, const  vec3d & pb, const  vec3d & pc, const  vec3d & pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE double incircle    (double const * pa, double const * pb, double const * pc, double const * pd);
CINO_INLINE double incircle    (const  vec2d & pa, const  vec2d & pb, const  vec2d & pc, const  vec2d & pd);
CINO_INLINE double incirclefast(double const * pa, double const * pb, double const * pc, double const * pd);
CINO_INLINE double incirclefast(const  vec2d & pa, const  vec2d & pb, const  vec2d & pc, const  vec2d & pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE double insphere    (double const * pa, double const * pb, double const * pc, double const * pd, double const * pe);
CINO_INLINE double insphere    (const  vec3d & pa, const  vec3d & pb, const  vec3d & pc, const  vec3d & pd, const  vec3d & pe);
CINO_INLINE double inspherefast(double const * pa, double const * pb, double const * pc, double const * pd, double const * pe);
CINO_INLINE double inspherefast(const  vec3d & pa, const  vec3d & pb, const  vec3d & pc, const  vec3d & pd, const  vec3d & pe);

}

#ifndef  CINO_STATIC_LIB
#include "Shewchuk_predicates.cpp"
#endif

#endif // CINO_SHEWCHUK_PREDICATES
