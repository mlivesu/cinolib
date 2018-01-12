/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
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
