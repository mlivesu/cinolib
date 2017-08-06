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

#include <cinolib/cinolib.h>
#include <cinolib/scalar_field.h>
#include <cinolib/vector_field.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/meshes/tetmesh.h>
#include <eigen3/Eigen/Sparse>

namespace cinolib
{

/* Compute the gradient of a mesh as a 3M x N matrix, where M is the
 * number of elements and N the number of vertices.
 *
 *
 * TRIANGLE MESHES
 *
 * Given a function f() defined on the vertices of the mesh, the gradient
 * at the triangle Tijk is defined as
 *
 *  grad(Tijk) = [ f(j) - f(i) ] * R90_(vi - vk) / 2A +
 *               [ f(k) - f(i) ] * R90_(vj - vi) / 2A
 *
 * with vi being the 3D position of vertex i and A the area of Tijk. R90 represents a
 * rotation of 90 degrees around the surface normal at Tijk.
 *
 *
 *
 * TETRAHEDRAL MESHES
 *
 * Given a function f() defined on the vertices of the mesh, the gradient
 * at the tetrahedron Tijkl is defined according to the Green-Gauss method:
 *
 * grad(Tijkl) = 1/V(T) * sum_{t=1}^{4} ( avg_f(t_i) * n_{t_i} )
 *
 * where t_i is the i-th triangular facet of Tijkl, n_{t_i} its normal
 * orientation and avg_f(t_i) the average f() at the vertices of t_i.
 *
 * NOTE: avg_f() can be measured in many different ways, and the accuracy
 * of the gradient estimation heavily depends on the chosen averaging system.
 * For ease of implementation we use the arithmetic average.
 *
 *
 * References:
 *
 * Gradient Calculation Methods on Arbitrary Polyhedral Unstructured Meshes for Cell-Centered CFD Solvers
 * Emre Sozer, Christoph Brehm and Cetin C. Kiris
 * American Institute of Aeronautics and Astronautics (2014)
 *
 * A Comparison of Gradient Estimation Methods for Volume Rendering on Unstructured Meshes
 * Carlos D. Correa, Robert Hero and Kwan-Liu Ma
 * IEEE Transactions on Visualization and Computer Graphics (2011)
*/


template<class Mesh>
CINO_INLINE
Eigen::SparseMatrix<double> gradient(const Mesh &)
{
    std::cerr << "WARNING! - Gradient is not available for this mesh type yet!" << endl;
    assert(false);
}

template <>
CINO_INLINE
Eigen::SparseMatrix<double> gradient<Trimesh<>>(const Trimesh<> & m);


template <>
CINO_INLINE
Eigen::SparseMatrix<double> gradient<Tetmesh<>>(const Tetmesh<> & m);

}

#ifndef  CINO_STATIC_LIB
#include "gradient.cpp"
#endif

#endif // CINO_GRADIENT_H
