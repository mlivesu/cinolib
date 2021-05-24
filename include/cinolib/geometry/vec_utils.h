/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
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
#ifndef CINO_VEC_UTILS_H
#define CINO_VEC_UTILS_H

#include <cinolib/cino_inline.h>
#include <sys/types.h>

namespace cinolib
{

template<uint d, typename T> CINO_INLINE void vec_plus     (const T * a, const T * b, T * c);
template<uint d, typename T> CINO_INLINE void vec_minus    (const T * a, const T * b, T * c);
template<uint d, typename T> CINO_INLINE void vec_multiply (const T * a, const T & b, T * c);
template<uint d, typename T> CINO_INLINE void vec_divide   (const T * a, const T & b, T * c);
template<uint d, typename T> CINO_INLINE void vec_plus     (      T * a, const T * b);
template<uint d, typename T> CINO_INLINE void vec_minus    (      T * a, const T * b);
template<uint d, typename T> CINO_INLINE void vec_multiply (      T * a, const T & b);
template<uint d, typename T> CINO_INLINE void vec_divide   (      T * a, const T & b);
template<uint d, typename T> CINO_INLINE void vec_flip_sign(T *a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T> CINO_INLINE bool vec_equals(const T * a, const T * b);
template<uint d, typename T> CINO_INLINE bool vec_less  (const T * a, const T * b);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T> CINO_INLINE T    vec_dot  (const T * a, const T * b);
template<        typename T> CINO_INLINE void vec_cross(const T * a, const T * b, const T * c);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T> CINO_INLINE double vec_normalize  (T * a);
template<uint d, typename T> CINO_INLINE double vec_length     (const T * a);
template<uint d, typename T> CINO_INLINE double vec_dist       (const T * a, const T * b);
template<uint d, typename T> CINO_INLINE T      vec_length_sqrd(const T * a);
template<uint d, typename T> CINO_INLINE T      vec_dist_sqrd  (const T * a, const T * b);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T> CINO_INLINE void vec_set_val (T * a, const T & val);
template<uint d, typename T> CINO_INLINE void vec_set_ZERO(T * a);
template<uint d, typename T> CINO_INLINE void vec_set_MIN (T * a);
template<uint d, typename T> CINO_INLINE void vec_set_MAX (T * a);
template<uint d, typename T> CINO_INLINE void vec_set_INF (T * a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T> CINO_INLINE T    vec_min_entry(const T * a);
template<uint d, typename T> CINO_INLINE T    vec_max_entry(const T * a);
template<uint d, typename T> CINO_INLINE void vec_min      (const T * a, const T * b, T * c);
template<uint d, typename T> CINO_INLINE void vec_max      (const T * a, const T * b, T * c);
template<uint d, typename T> CINO_INLINE void vec_clamp    (const T * a, const T & min, const T & max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// if BOTH vecs are known to be unit length, setting unit_length to TRUE will skip the normalization step
template<uint d, typename T> CINO_INLINE double vec_angle_deg(const T * a, const T * b, const bool unit_length = false);
template<uint d, typename T> CINO_INLINE double vec_angle_rad(const T * a, const T * b, const bool unit_length = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T> CINO_INLINE bool vec_is_null      (const T * a);
template<uint d, typename T> CINO_INLINE bool vec_is_nan       (const T * a);
template<uint d, typename T> CINO_INLINE bool vec_is_inf       (const T * a);
template<uint d, typename T> CINO_INLINE bool vec_is_degenerate(const T * a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T> CINO_INLINE void vec_swap (T * a, const uint i, const uint j);
template<uint d, typename T> CINO_INLINE void vec_copy (const T * a, T * b);
template<uint d, typename T> CINO_INLINE void vec_print(const T * a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}

#ifndef  CINO_STATIC_LIB
#include "vec_utils.cpp"
#endif

#endif // CINO_VEC_UTILS_H
