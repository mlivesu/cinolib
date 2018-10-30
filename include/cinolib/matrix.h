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
#ifndef CINO_MATRIX_H
#define CINO_MATRIX_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec2.h>
#include <Eigen/Dense>

namespace cinolib
{

CINO_INLINE
void eigen_decomposition_2x2(const double   a00,
                             const double   a01,
                             const double   a10,
                             const double   a11,
                                   vec2d  & v_min, // eigenvectors
                                   vec2d  & v_max,
                                   double & min,   // eigenvalues
                                   double & max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigenvalues_2x2(const double   a00,
                     const double   a01,
                     const double   a10,
                     const double   a11,
                           double & min,
                           double & max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigenvectors_2x2(const double   a00,
                      const double   a01,
                      const double   a10,
                      const double   a11,
                            vec2d  & v_min,
                            vec2d  & v_max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double determinant_2x2(const double a00, const double a01,
                       const double a10, const double a11);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double determinant_2x2(const vec2d a0, const vec2d a1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigen_decomposition_3x3(const double   a[3][3],
                                   vec3d  & v_min, // eigenvectors
                                   vec3d  & v_mid,
                                   vec3d  & v_max,
                                   double & min,   // eigenvalues
                                   double & mid,
                                   double & max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigen_decomposition_3x3(const double   a00,
                             const double   a01,
                             const double   a02,
                             const double   a10,
                             const double   a11,
                             const double   a12,
                             const double   a20,
                             const double   a21,
                             const double   a22,
                                   vec3d  & v_min, // eigenvectors
                                   vec3d  & v_mid,
                                   vec3d  & v_max,
                                   double & min,   // eigenvalues
                                   double & mid,
                                   double & max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigenvalues_3x3(const double   a00,
                     const double   a01,
                     const double   a02,
                     const double   a10,
                     const double   a11,
                     const double   a12,
                     const double   a20,
                     const double   a21,
                     const double   a22,
                           double & min,
                           double & mid,
                           double & max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigenvectors_3x3(const double   a00,
                      const double   a01,
                      const double   a02,
                      const double   a10,
                      const double   a11,
                      const double   a12,
                      const double   a20,
                      const double   a21,
                      const double   a22,
                            vec3d  & v_min,
                            vec3d  & v_mid,
                            vec3d  & v_max);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double determinant_3x3(const double a00, const double a01, const double a02,
                       const double a10, const double a11, const double a12,
                       const double a20, const double a21, const double a22);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_std_3x3_to_Eigen_3x3(const double stdM[3][3], Eigen::Matrix3d & eigenM);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_eigen_3x3_to_std_3x3(const Eigen::Matrix3d & eigenM, double stdM[3][3]);

}

#ifndef  CINO_STATIC_LIB
#include "matrix.cpp"
#endif

#endif // CINO_MATRIX_H
