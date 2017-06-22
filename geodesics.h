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
#include <cinolib/timer.h>

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
ScalarField compute_geodesics(const Mesh              & m,
                              const std::vector<uint> & heat_charges,
                              const int                 laplacian_mode = COTANGENT,
                              const float               time_scalar = 1.0)
{
    timer_start("Compute geodesics");

    // use h^2 as time step, as suggested in the original paper
    Mesh m_copy = m;
    m_copy.normalize_area();
    m_copy.center_bbox();
    double time = m_copy.edge_avg_length();
    time *= time;
    time *= time_scalar;

    Eigen::SparseMatrix<double> L   = laplacian<Mesh>(m_copy, laplacian_mode);
    Eigen::SparseMatrix<double> M   = mass_matrix<Mesh>(m_copy);
    Eigen::SparseMatrix<double> G   = gradient(m_copy);
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m_copy.num_verts());

    std::map<uint,double> bc;
    for(uint vid : heat_charges) bc[vid] = 1.0;

    ScalarField heat(m_copy.num_verts());
    solve_square_system_with_bc(M - time * L, rhs, heat, bc);

    VectorField grad = G * heat;
    grad.normalize();

    ScalarField geodesics;
    solve_square_system_with_bc(-L, G.transpose() * grad, geodesics, bc);

    timer_stop("Compute geodesics");

    geodesics.normalize_in_01();
    return geodesics;
}

}

#endif // CINO_GEODESICS_H
