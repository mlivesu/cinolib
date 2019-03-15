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
#ifndef CINO_VECTOR_AREA_MATRIX
#define CINO_VECTOR_AREA_MATRIX

#include <cinolib/meshes/abstract_polygonmesh.h>
#include <Eigen/Sparse>

namespace cinolib
{

/* Compute a matrix A such that the quadratic form u^t*A*u gives the area of all
 * boundary triangles of mesh m. This matrix is used for the computation of LSCM.
 * For further details refer to Section 2.2 in:
 *
 * Spectral Conformal Parameterization
 * Patrick Mullen, Yiying Tong, Pierre Alliez, Mathieu Desbrun
 * Symposium on Geometry Processing, 2008
*/

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::SparseMatrix<double> vector_area_matrix(const AbstractPolygonMesh<M,V,E,P> & m);

}

#ifndef  CINO_STATIC_LIB
#include "vector_area_matrix.cpp"
#endif

#endif // CINO_VECTOR_AREA_MATRIX
