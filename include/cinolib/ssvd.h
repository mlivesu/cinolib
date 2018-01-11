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
#ifndef CINO_SSVD_H
#define CINO_SSVD_H

#include <cinolib/cino_inline.h>
#include <Eigen/Dense>

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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void closest_pos_det_matrix(double T[3][3]);

}

#ifndef  CINO_STATIC_LIB
#include "ssvd.cpp"
#endif

#endif // CINO_SSVD_H
