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
#ifndef CINO_GEODESICS_H
#define CINO_GEODESICS_H

#include <vector>

#include <cinolib/cinolib.h>
#include <cinolib/scalar_field.h>
#include <cinolib/vector_field.h>
#include <cinolib/gradient.h>
#include <cinolib/laplacian.h>
#include <cinolib/vertex_mass.h>
#include <cinolib/linear_solvers.h>

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

template<class M, class V, class E, class P>
CINO_INLINE
ScalarField compute_geodesics(      AbstractPolygonMesh<M,V,E,P> & m,
                              const std::vector<uint>            & heat_charges,
                              const int                            laplacian_mode = COTANGENT,
                              const float                          time_scalar = 1.0);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
ScalarField compute_geodesics(      AbstractPolyhedralMesh<M,V,E,F,P> & m,
                              const std::vector<uint>                 & heat_charges,
                              const int                                 laplacian_mode = COTANGENT,
                              const float                               time_scalar = 1.0);
}

#ifndef  CINO_STATIC_LIB
#include "geodesics.cpp"
#endif

#endif // CINO_GEODESICS_H
