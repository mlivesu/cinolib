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
#include <cinolib/pca.h>
#include <cinolib/covmat.h>
#include <cinolib/matrix.h>

namespace cinolib
{

/* Computes the Principal Component Analysis of a list of points in 3D space.
 * It returns the three mutually orthogonal eigenvectors of the covariance matrix,
 * each one scaled by its corresponding eigenvalue.
*/

CINO_INLINE
void PCA(const std::vector<vec3d> & points, vec3d & e_min, vec3d & e_mid, vec3d & e_max)
{
    double cov[3][3];
    covariance_matrix(points, cov);

    double min, mid, max;
    eigen_decomposition_3x3(cov, e_min, e_mid, e_max, min, mid, max);

    // rescale vectors according to eigenvalues
    e_min *= min;
    e_mid *= mid;
    e_max *= max;
}

}
