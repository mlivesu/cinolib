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
#ifndef CINO_SHEWCHUK_PREDICATES_WRAP
#define CINO_SHEWCHUK_PREDICATES_WRAP

#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

/* Wrap of the popular geometric predicates described by Shewchuk in:
 *
 * Routines for Arbitrary Precision Floating-point Arithmetic and
 * Fast Robust Geometric Predicates
 *
 * WARNING: remember to call exactinit() once in your application
 * prior using any of these predicates, otherwise machine epsilon
 * and error constants will not be set properly, and precision will
 * be as bad as the standard floating point system!
*/

extern "C"
{
    void   exactinit();
    double orient2d    (const double * pa, const double * pb, const double * pc);
    double orient2dfast(const double * pa, const double * pb, const double * pc);
    double orient3d    (const double * pa, const double * pb, const double * pc, const double * pd);
    double orient3dfast(const double * pa, const double * pb, const double * pc, const double * pd);
    double incircle    (const double * pa, const double * pb, const double * pc, const double * pd);
    double incirclefast(const double * pa, const double * pb, const double * pc, const double * pd);
    double insphere    (const double * pa, const double * pb, const double * pc, const double * pd, const double * pe);
    double inspherefast(const double * pa, const double * pb, const double * pc, const double * pd, const double * pe);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE double orient2d    (const vec2d & pa, const vec2d & pb, const vec2d & pc);
CINO_INLINE double orient2dfast(const vec2d & pa, const vec2d & pb, const vec2d & pc);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE double orient3d    (const vec3d & pa, const vec3d & pb, const vec3d & pc, const vec3d & pd);
CINO_INLINE double orient3dfast(const vec3d & pa, const vec3d & pb, const vec3d & pc, const vec3d & pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE double incircle    (const vec2d & pa, const vec2d & pb, const vec2d & pc, const vec2d & pd);
CINO_INLINE double incirclefast(const vec2d & pa, const vec2d & pb, const vec2d & pc, const vec2d & pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE double insphere    (const vec3d & pa, const vec3d & pb, const vec3d & pc, const vec3d & pd, const vec3d & pe);
CINO_INLINE double inspherefast(const vec3d & pa, const vec3d & pb, const vec3d & pc, const vec3d & pd, const vec3d & pe);

}

#ifndef  CINO_STATIC_LIB
#include "Shewchuk_predicates_wrap.cpp"
#endif

#endif // CINO_SHEWCHUK_PREDICATES_WRAP
