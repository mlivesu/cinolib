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
*                                                                               *
*                                                                               *
*     Claudio Mancinelli (claudio.f.mancinelli@gmail.com)                       *
*                                                                               *
*     University of Genoa                                                       *
*     Via Dodecaneso, 35                                                        *
*     16146 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_GRADIENT_H
#define CINO_GRADIENT_H

#include <Eigen/Sparse>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/abstract_polyhedralmesh.h>
#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

/* Compute the gradient of a mesh as a 3M x N matrix, where M is the
 * number of elements and N the number of vertices. If per_poly is true,
 * the matrix size is 3N x N, and the gradient is computed per vertex,
 * as average between the elements incident to it.
 *
 * Gradients are always computed using the Green-Gauss method. For simplicial
 * meshes, this is equivalent to extending the function within each tri/tet
 * with linear interpolation using the barycentri coordinates, which uniquely
 * defines a piece-wise constant gradient field.
 *
 * References:
 *
 *   A Comparison of Methods for Gradient Field Estimation on Simplicial Meshes
 *   Claudio Mancinelli, Marco Livesu, Enrico Puppo
 *   Computers & Graphics (2019)
 *
 *   Gradient Calculation Methods on Arbitrary Polyhedral Unstructured Meshes for Cell-Centered CFD Solvers
 *   Emre Sozer, Christoph Brehm and Cetin C. Kiris
 *   American Institute of Aeronautics and Astronautics (2014)
 *
 *   A Comparison of Gradient Estimation Methods for Volume Rendering on Unstructured Meshes
 *   Carlos D. Correa, Robert Hero and Kwan-Liu Ma
 *   IEEE Transactions on Visualization and Computer Graphics (2011)
*/

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix(const AbstractPolygonMesh<M,V,E,P> & m, const bool per_poly = true);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix(const AbstractPolyhedralMesh<M,V,E,F,P> & m, const bool per_poly = true);

}

#ifndef  CINO_STATIC_LIB
#include "gradient.cpp"
#endif

#endif // CINO_GRADIENT_H
