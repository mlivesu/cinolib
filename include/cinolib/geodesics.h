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
#ifndef CINO_GEODESICS_H
#define CINO_GEODESICS_H

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/scalar_field.h>
#include <cinolib/symbols.h>
#include <Eigen/Sparse>

namespace cinolib
{

/* Compute approximated geodesics as explained in
 *
 * Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow
 * KEENAN CRANE, CLARISSE WEISCHEDEL and MAX WARDETZKY
 * ACM Transactions on Graphics, 2013
 *
 * First step: solve the heat flow problem with some boundary condition u0
 * (the point(s) from which you want to compute the geodesic distances)
 *
 *                  (M - t * L) u = u0
 *
 * Second step: solve a Poisson problem to determine the function phy whose
 * divergence coincides with the normalized gradient of the heat flow
 *
 *              L phy = div( grad(u)/|grad(u)| )
 *
 * which can also be written as
 *
 *              L phy = grad^T * ( grad(u)/|grad(u)| )
 *
 * phy is the scalar field encoding the geodesic distances.
*/

template<class Mesh>
CINO_INLINE
ScalarField compute_geodesics(      Mesh              & m,
                              const std::vector<uint> & heat_charges,
                              const int                 laplacian_mode = COTANGENT,
                              const float               time_scalar = 1.0,
                              const bool                hard_constrain_charges = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    Eigen::SimplicialLLT<Eigen::SparseMatrix<double>>  *heat_flow_cache   = NULL;
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> *integration_cache = NULL;
    Eigen::SparseMatrix<double>                         gradient_matrix;
}
GeodesicsCache;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
ScalarField compute_geodesics_amortized(      Mesh              & m,
                                              GeodesicsCache    & cache,
                                        const std::vector<uint> & heat_charges,
                                        const int                 laplacian_mode = COTANGENT,
                                        const float               time_scalar = 1.0);
}

#ifndef  CINO_STATIC_LIB
#include "geodesics.cpp"
#endif

#endif // CINO_GEODESICS_H
