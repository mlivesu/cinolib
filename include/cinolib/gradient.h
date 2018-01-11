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
#ifndef CINO_GRADIENT_H
#define CINO_GRADIENT_H

#include <Eigen/Sparse>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/abstract_polyhedralmesh.h>
#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

/* Compute the gradient of a mesh as a 3M x N matrix, where M is the
 * number of elements and N the number of vertices. References:
 *
 * Gradient Calculation Methods on Arbitrary Polyhedral Unstructured Meshes for Cell-Centered CFD Solvers
 * Emre Sozer, Christoph Brehm and Cetin C. Kiris
 * American Institute of Aeronautics and Astronautics (2014)
 *
 * A Comparison of Gradient Estimation Methods for Volume Rendering on Unstructured Meshes
 * Carlos D. Correa, Robert Hero and Kwan-Liu Ma
 * IEEE Transactions on Visualization and Computer Graphics (2011)
*/

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix(const AbstractPolygonMesh<M,V,E,P> & m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix(const AbstractPolyhedralMesh<M,V,E,F,P> & m);

}

#ifndef  CINO_STATIC_LIB
#include "gradient.cpp"
#endif

#endif // CINO_GRADIENT_H
