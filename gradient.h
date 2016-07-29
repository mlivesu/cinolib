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
#ifndef GRADIENT_H
#define GRADIENT_H

#include <cinolib/cinolib.h>
#include <cinolib/scalar_field.h>
#include <cinolib/vector_field.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/meshes/tetmesh/tetmesh.h>
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
Eigen::SparseMatrix<double> gradient<Trimesh>(const Trimesh & m);


template <>
CINO_INLINE
Eigen::SparseMatrix<double> gradient<Tetmesh>(const Tetmesh & m);

}

#ifndef  CINO_STATIC_LIB
#include "gradient.cpp"
#endif

#endif // GRADIENT_H
