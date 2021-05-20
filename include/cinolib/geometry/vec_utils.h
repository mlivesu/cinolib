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
#ifndef CINO_VEC_UTILS_H
#define CINO_VEC_UTILS_H

#include <cinolib/cino_inline.h>
#include <sys/types.h>

namespace cinolib
{

namespace vec
{

template<uint D, typename T> CINO_INLINE void   plus     (const T *a, const T *b, T *c);
template<uint D, typename T> CINO_INLINE void   minus    (const T *a, const T *b, T *c);
template<uint D, typename T> CINO_INLINE void   scale    (const T *a, const T  b, T *c);
template<uint D, typename T> CINO_INLINE void   divide   (const T *a, const T  b, T *c);
template<uint D, typename T> CINO_INLINE void   flip_sign(T *a);
template<uint D, typename T> CINO_INLINE double normalize(T *a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE void swap(T *a, const uint i, const uint j);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE bool equals(const T *a, const T *b);
template<uint D, typename T> CINO_INLINE bool less  (const T *a, const T *b);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE T    dot  (const T *a, const T *b);
template<uint D, typename T> CINO_INLINE void cross(const T *a, const T *b, const T *c);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE double length     (const T *a);
template<uint D, typename T> CINO_INLINE double length_sqrd(const T *a);
template<uint D, typename T> CINO_INLINE double dist       (const T *a, const T *b);
template<uint D, typename T> CINO_INLINE double dist_sqrd  (const T *a, const T *b);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE void set_ZERO(T *a);
template<uint D, typename T> CINO_INLINE void set_MIN (T *a);
template<uint D, typename T> CINO_INLINE void set_MAX (T *a);
template<uint D, typename T> CINO_INLINE void set_INF (T *a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE T    min_entry(const T *a);
template<uint D, typename T> CINO_INLINE T    max_entry(const T *a);
template<uint D, typename T> CINO_INLINE void min      (const T *a, const T *b, T *c);
template<uint D, typename T> CINO_INLINE void max      (const T *a, const T *b, T *c);
template<uint D, typename T> CINO_INLINE void clamp    (const T *a, const T min, const T max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE double angle_deg(const T *a, const T *b);
template<uint D, typename T> CINO_INLINE double angle_rad(const T *a, const T *b);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE bool is_null      (const T *a);
template<uint D, typename T> CINO_INLINE bool has_nan      (const T *a);
template<uint D, typename T> CINO_INLINE bool has_inf      (const T *a);
template<uint D, typename T> CINO_INLINE bool is_degenerate(const T *a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE void copy(const T *a, T *b);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T> CINO_INLINE void print(const T* a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}

}

#ifndef  CINO_STATIC_LIB
#include "vec_utils.cpp"
#endif

#endif // CINO_VEC_UTILS_H
