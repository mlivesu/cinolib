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
#ifndef CINO_QUALITY_HEX
#define CINO_QUALITY_HEX

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
double hex_diagonal(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                    const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_edge_ratio(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                      const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                    const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_max_edge_ratio(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                          const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_max_aspect_Frobenius(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                                const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_mean_aspect_Frobenius(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_oddy(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_relative_size_squared(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7,
                                 const double & avgV);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_scaled_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_shape(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_shape_and_size(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                          const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7,
                          const double & avg_V);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_shear(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_shear_and_size(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                          const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7,
                          const double & avg_V);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_skew(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_stretch(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                   const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_taper(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                  const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_unsigned_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7);

}

#ifndef  CINO_STATIC_LIB
#include "quality_hex.cpp"
#endif

#endif // CINO_QUALITY_HEX
