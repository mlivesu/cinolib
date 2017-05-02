/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#ifndef CINO_SSVD_H
#define CINO_SSVD_H

#include <cinolib/cinolib.h>

#include <eigen3/Eigen/Dense>

namespace cinolib
{

// Performs the Signed Singular Value Decomposition (SSVD) of a 3x3 matrix
//
//                      SSVD(A) = U * S * Vt
//
// where U and V' are GUARANTEED to be pure rotation matrices. They cannot
// be reflections, as in the standard SVD decomposition.
//
// NOTE: V is returned not transposed, to make Vt you'll need
//       to call the .transpose() method
//
CINO_INLINE
void ssvd(const Eigen::Matrix3d                 & m,
                Eigen::Matrix3d                 & u,
                Eigen::DiagonalMatrix<double,3> & s,
                Eigen::Matrix3d                 & v);


CINO_INLINE
void closest_pos_det_matrix(double T[3][3]);

}

#ifndef  CINO_STATIC_LIB
#include "ssvd.cpp"
#endif

#endif // CINO_SSVD_H
