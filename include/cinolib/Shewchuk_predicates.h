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
