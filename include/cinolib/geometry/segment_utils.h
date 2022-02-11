/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#ifndef CINO_SEGMENT_UTILS_H
#define CINO_SEGMENT_UTILS_H

#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

/* Given two segments S0(s00,s01) and S1(s10,s11) finds the pair
 * of points in S0,S1 that are closest to each other. The function
 * returns the distance between such points. Parameters p_on_s0,
 * p_on_s1, s and t allow to precisely locate points on each segment,
 * both in explicit and parametric form. In case the two segments are
 * intersecting p_on_s0 and p_on_s1 coincide.
 *
 *      Ref: Real Time Collision Detection, Section 5.1.9
*/
template<class T>
CINO_INLINE
T closest_points_between_segments(const mat<3,1,T> & s00,
                                  const mat<3,1,T> & s01,
                                  const mat<3,1,T> & s10,
                                  const mat<3,1,T> & s11,
                                        T          & s,
                                        T          & t,
                                        mat<3,1,T> & p_on_s0,
                                        mat<3,1,T> & p_on_s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* WARNING: this assumes that the segments DO INTERSECT!
 * For exact and approximate intersection checks, please
 * refer to cinolib/predicates.h
*/
template<class T>
CINO_INLINE
mat<3,1,T> segment_intersection(const mat<3,1,T> & s00,
                                const mat<3,1,T> & s01,
                                const mat<3,1,T> & s10,
                                const mat<3,1,T> & s11);
}

#ifndef  CINO_STATIC_LIB
#include "segment_utils.cpp"
#endif

#endif // CINO_SEGMENT_UTILS_H
