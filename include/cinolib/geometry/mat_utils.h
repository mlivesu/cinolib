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
#ifndef CINO_MAT_UTILS_H
#define CINO_MAT_UTILS_H

#include <cinolib/cino_inline.h>
#include <sys/types.h>

namespace cinolib
{

template<uint r, uint c, typename T> CINO_INLINE void mat_plus     (const T * m1, const T * m2, T * m3);
template<uint r, uint c, typename T> CINO_INLINE void mat_minus    (const T * m1, const T * m2, T * m3);
template<uint r, uint c, typename T> CINO_INLINE void mat_multiply (const T * m1, const T & s,  T * m2);
template<uint r, uint c, typename T> CINO_INLINE void mat_divide   (const T * m1, const T & s,  T * m2);
template<uint r, uint c, typename T> CINO_INLINE void mat_plus     (      T * m1, const T * m2);
template<uint r, uint c, typename T> CINO_INLINE void mat_minus    (      T * m1, const T * m2);
template<uint r, uint c, typename T> CINO_INLINE void mat_multiply (      T * m1, const T & s);
template<uint r, uint c, typename T> CINO_INLINE void mat_divide   (      T * m1, const T & s);
template<uint r, uint c, typename T> CINO_INLINE void mat_flip_sign(      T * m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r1, uint c1, uint r2, uint c2, typename T> CINO_INLINE void mat_multiply(const T * a, const T * b, T * c);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint r, uint c, typename T> CINO_INLINE bool mat_equals(const T * a, const T * b);
//template<uint r, uint c, typename T> CINO_INLINE bool mat_less  (const T * a, const T * b);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE void set_ZERO(T *a);
//template<uint R, uint C, typename T> CINO_INLINE void set_MIN (T *a);
//template<uint R, uint C, typename T> CINO_INLINE void set_MAX (T *a);
//template<uint R, uint C, typename T> CINO_INLINE void set_INF (T *a);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE void set_IDENTITY(T *a);
//template<uint R, uint C, typename T> CINO_INLINE void set_SCALING (T *a, const T  s);
//template<uint R, uint C, typename T> CINO_INLINE void set_SCALING (T *a, const T *s);
//template<uint R, uint C, typename T> CINO_INLINE void set_ROTATION(T *a, const T *axis, const double angle_rad);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE T    min_entry(const T *a);
//template<uint R, uint C, typename T> CINO_INLINE T    max_entry(const T *a);
//template<uint R, uint C, typename T> CINO_INLINE void min      (const T *a, const T *b, T *c);
//template<uint R, uint C, typename T> CINO_INLINE void max      (const T *a, const T *b, T *c);
//template<uint R, uint C, typename T> CINO_INLINE void clamp    (const T *a, const T min, const T max);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE bool is_null      (const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool is_nan      (const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool has_inf      (const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool is_degenerate(const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool is_full_rank (const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool is_symmetric (const T *a);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE bool svd (const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool ssvd(const T *a);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE bool norm_Frobenius(const T *a);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE bool eigenvalues(const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool eigenvectors(const T *a);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE bool det  (const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool trace(const T *a);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE bool transposed(const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool inverse   (const T *a);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE bool det_Cramer(const T *a);
//template<uint R, uint C, typename T> CINO_INLINE bool solve_w_Cramer(const T *a);
////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE void col (const T *a, T *c);
//template<uint R, uint C, typename T> CINO_INLINE void row (const T *a, T *r);
//template<uint R, uint C, typename T> CINO_INLINE void diag(const T *a, T *d);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE void col_ptr (const T *a, const uint i, T **c);
//template<uint R, uint C, typename T> CINO_INLINE void row_ptr (T a[][C], const uint i, T* r[]);
//template<uint R, uint C, typename T> CINO_INLINE void diag_ptr(const T *a, const uint i, T **d);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T> CINO_INLINE void copy(const T *a, T *b);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_print(const T * a);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}

#ifndef  CINO_STATIC_LIB
#include "mat_utils.cpp"
#endif

#endif // CINO_MAT_UTILS_H
