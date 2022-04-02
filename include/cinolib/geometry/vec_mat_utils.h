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
#ifndef CINO_VEC_MAT_UTILS_H
#define CINO_VEC_MAT_UTILS_H

#include <cinolib/cino_inline.h>
#include <initializer_list>
#include <sys/types.h>

namespace cinolib
{

/* Exhaustive list of basic linear algebra operators for vector/matrix
 * entities represented as plain C-like arrays of some scalar type T
 *
 *      T my_vector[size];
 *      T my_matrix[rows][cols];
 *
 * Element-wise vector operators (e.g. plus, minus,...) can be used
 * for both 1D and 2D arrays.
 *
 * 2D Arrays are stored in row major order.
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T> CINO_INLINE void   vec_set      (      T * vec, const std::initializer_list<T> & il);
template<uint d, typename T> CINO_INLINE void   vec_set_dense(      T * vec, const T   val);
template<uint d, typename T> CINO_INLINE void   vec_plus     (const T * v_0, const T * v_1, T * v_2);
template<uint d, typename T> CINO_INLINE void   vec_minus    (const T * v_0, const T * v_1, T * v_2);
template<uint d, typename T> CINO_INLINE void   vec_minus    (const T * v_0,       T * v_1);
template<uint d, typename T> CINO_INLINE void   vec_times    (const T * v_0, const T   val, T * v_1);
template<uint d, typename T> CINO_INLINE void   vec_divide   (const T * v_0, const T   val, T * v_1);
template<uint d, typename T> CINO_INLINE bool   vec_equals   (const T * v_0, const T * v_1);
template<uint d, typename T> CINO_INLINE bool   vec_less     (const T * v_0, const T * v_1);
template<uint d, typename T> CINO_INLINE T      vec_dot      (const T * v_0, const T * v_1);
template<        typename T> CINO_INLINE void   vec_cross    (const T * v_0, const T * v_1, T *v_2);
template<uint d, typename T> CINO_INLINE T      vec_angle_deg(const T * v_0, const T * v_1, const bool normalize);
template<uint d, typename T> CINO_INLINE T      vec_angle_rad(const T * v_0, const T * v_1, const bool normalize);
template<uint d, typename T> CINO_INLINE double vec_dist     (const T * v_0, const T * v_1);
template<uint d, typename T> CINO_INLINE double vec_dist_sqrd(const T * v_0, const T * v_1);
template<uint d, typename T> CINO_INLINE double vec_norm     (const T * vec);
template<uint d, typename T> CINO_INLINE double vec_norm_sqrd(const T * vec);
template<uint d, typename T> CINO_INLINE double vec_norm_p   (const T * vec, const float p);
template<uint d, typename T> CINO_INLINE double vec_normalize(      T * vec);
template<uint d, typename T> CINO_INLINE void   vec_min      (const T * v_0, const T * v_1, T * c);
template<uint d, typename T> CINO_INLINE void   vec_max      (const T * v_0, const T * v_1, T * c);
template<uint d, typename T> CINO_INLINE T      vec_min_entry(const T * vec);
template<uint d, typename T> CINO_INLINE T      vec_max_entry(const T * vec);
template<uint d, typename T> CINO_INLINE void   vec_clamp    (      T * vec, const T min, const T max);
template<uint d, typename T> CINO_INLINE void   vec_swap     (      T * vec, const uint i, const uint j);
template<uint d, typename T> CINO_INLINE bool   vec_is_null  (const T * vec);
template<uint d, typename T> CINO_INLINE bool   vec_is_nan   (const T * vec);
template<uint d, typename T> CINO_INLINE bool   vec_is_inf   (const T * vec);
template<uint d, typename T> CINO_INLINE bool   vec_is_deg   (const T * vec);
template<uint d, typename T> CINO_INLINE void   vec_copy     (const T * v_0, T * v_1);
template<uint d, typename T> CINO_INLINE void   vec_print    (const T * vec);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T> CINO_INLINE void mat_set             (      T m[][c], const std::initializer_list<T> & il);
template<uint d,         typename T> CINO_INLINE void mat_set_diag        (      T m[][d], const T val);
template<uint d,         typename T> CINO_INLINE void mat_set_diag        (      T m[][d], const T diag[]);
template<uint r, uint c, typename T> CINO_INLINE void mat_set_col         (      T m[][c], const uint i, const T col []);
template<uint r, uint c, typename T> CINO_INLINE void mat_set_row         (      T m[][c], const uint i, const T row []);
template<uint d,         typename T> CINO_INLINE void mat_set_rot_2d      (      T m[][d], const T ang_rad);
template<uint d,         typename T> CINO_INLINE void mat_set_rot_3d      (      T m[][d], const T ang_rad, const T axis[]);
template<uint d,         typename T> CINO_INLINE void mat_set_trans       (      T m[][d], const T tx[]);
template<uint r, uint c, typename T> CINO_INLINE void mat_swap            (      T m[][c], const uint i, const uint j, const uint k, const uint l);
template<uint r, uint c, typename T> CINO_INLINE void mat_col             (const T m[][c], const uint i, T  col []);
template<uint r, uint c, typename T> CINO_INLINE void mat_row             (const T m[][c], const uint i, T  row []);
template<uint d,         typename T> CINO_INLINE void mat_diag            (const T m[][d], const         T  diag[]);
template<uint r, uint c, typename T> CINO_INLINE void mat_col_ptr         (const T m[][c], const uint i, T *col []);
template<uint r, uint c, typename T> CINO_INLINE void mat_row_ptr         (      T m[][c], const uint i, T *row []);
template<uint d,         typename T> CINO_INLINE void mat_diag_ptr        (const T m[][d],               T *diag[]);
template<uint d,         typename T> CINO_INLINE bool mat_is_symmetric    (const T m[][d]);
template<uint d,         typename T> CINO_INLINE T    mat_trace           (const T m[][d]);
template<uint d,         typename T> CINO_INLINE T    mat_det             (const T m[][d]);
template<                typename T> CINO_INLINE T    mat_det22           (const T m00, const T m01, const T m10, const T m11);
template<                typename T> CINO_INLINE T    mat_det33           (const T m00, const T m01, const T m02, const T m10, const T m11, const T m12, const T m20, const T m21, const T m22);
template<uint r, uint c, typename T> CINO_INLINE void mat_transpose       (const T m[][c], T tr[][r]);
template<uint d,         typename T> CINO_INLINE void mat_inverse         (const T m[][d], T in[][d]);
template<uint r, uint c, typename T> CINO_INLINE void mat_eigendec        (const T m[][c], T eval[], T evec[][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_eigenval        (const T m[][c], T eval[]);
template<uint r, uint c, typename T> CINO_INLINE void mat_eigenvec        (const T m[][c], T evec[][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_svd             (const T m[][c], T U[][r], T S[], T V[][c]);
template<uint d,         typename T> CINO_INLINE void mat_closest_orth_mat(const T m[][d], T n[][d], const bool force_pos_det);
template<uint d,         typename T> CINO_INLINE void mat_solve_Cramer    (const T m[][d], const T b[], T x[]);
template<uint r, uint c, typename T> CINO_INLINE void mat_copy            (const T m[][c], T n[][c]);
template<uint r, uint c, typename T> CINO_INLINE void mat_print           (const T m[][c]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r0, uint c0, uint c1, typename T>
CINO_INLINE
void mat_times(const T m0[][c0], const T m1[][c1], T m2[][c1]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}

#ifndef  CINO_STATIC_LIB
#include "vec_mat_utils.cpp"
#endif

#endif // CINO_VEC_MAT_UTILS_H
