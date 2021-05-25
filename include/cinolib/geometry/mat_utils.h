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

template<uint r, uint c, typename T> CINO_INLINE void mat_plus  (const T ma[r][c], const T mb[r][c], T mc[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_minus (const T ma[r][c], const T mb[r][c], T mc[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_minus (      T ma[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_times (const T ma[r][c], const T scalar,   T mb[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_divide(const T ma[r][c], const T scalar,   T mb[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint ra, uint ca, uint rb, uint cb, typename T>
CINO_INLINE
void mat_times(const T ma[ra][ca], const T mb[rb][cb], T mc[ra][cb]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE bool mat_equals(const T ma[r][c], const T mb[r][c]);
template<uint r, uint c, typename T> CINO_INLINE bool mat_less  (const T ma[r][c], const T mb[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_set_full(T m[r][c], const T scalar);
template<uint r, uint c, typename T> CINO_INLINE void mat_set_diag(T m[r][c], const T scalar);
template<uint r, uint c, typename T> CINO_INLINE void mat_set_diag(T m[r][c], const T diag[r]);
template<                typename T> CINO_INLINE void mat_set_rot (T m[2][2], const T ang_rad);
template<                typename T> CINO_INLINE void mat_set_rot (T m[3][3], const T ang_rad, const T axis[3]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_min      (const T m1[r][c], const T m2[r][c], T m3[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_max      (const T m1[r][c], const T m2[r][c], T m3[r][c]);
template<uint r, uint c, typename T> CINO_INLINE T    mat_min_entry(const T m1[r][c]);
template<uint r, uint c, typename T> CINO_INLINE T    mat_max_entry(const T m1[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_clamp    (const T m1[r][c], const T min, const T max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE bool mat_is_null      (const T m[r][c]);
template<uint r, uint c, typename T> CINO_INLINE bool mat_is_nan       (const T m[r][c]);
template<uint r, uint c, typename T> CINO_INLINE bool mat_is_inf       (const T m[r][c]);
template<uint r, uint c, typename T> CINO_INLINE bool mat_is_degenerate(const T m[r][c]);
template<uint r, uint c, typename T> CINO_INLINE bool mat_is_symmetric (const T m[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_svd (const T m[r][c], T S[r][c], T V[], T D[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_ssvd(const T m[r][c], T S[r][c], T V[], T D[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE double mat_norm_Frobenius(const T m[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_eigen_decomp(const T m[r][c], T eval[], T evec[][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_eigenvalues (const T m[r][c], T eval[]);
template<uint r, uint c, typename T> CINO_INLINE void mat_eigenvectors(const T m[r][c], T evec[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE T mat_det  (const T m[r][c]);
template<uint r, uint c, typename T> CINO_INLINE T mat_trace(const T m[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_transposed(const T m[r][c], T tr[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_inverse   (const T m[r][c], T in[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE T    mat_det_Cramer    (const T m[r][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_solve_w_Cramer(const T m[r][c], const T b[], T x[]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_col (const T m[r][c], T col []);
template<uint r, uint c, typename T> CINO_INLINE void mat_row (const T m[r][c], T row []);
template<uint r, uint c, typename T> CINO_INLINE void mat_diag(const T m[r][c], T diag[]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_col_ptr (const T m[r][c], const uint i, T *col []);
template<uint r, uint c, typename T> CINO_INLINE void mat_row_ptr (      T m[r][c], const uint i, T *row []);
template<uint r, uint c, typename T> CINO_INLINE void mat_diag_ptr(const T m[r][c], const uint i, T *diag[]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_copy(const T m1[r][c], T m2[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_print(const T m[r][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}

#ifndef  CINO_STATIC_LIB
#include "mat_utils.cpp"
#endif

#endif // CINO_MAT_UTILS_H
