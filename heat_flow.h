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
#ifndef CINO_HEAT_FLOW_H
#define CINO_HEAT_FLOW_H

#include <cinolib/cinolib.h>
#include <cinolib/scalar_field.h>
#include <cinolib/laplacian.h>
#include <cinolib/vertex_mass.h>
#include <cinolib/linear_solvers.h>
#include <cinolib/timer.h>
#include <eigen3/Eigen/Sparse>

namespace cinolib
{

/* Solve the heat flow problem  (M - t * L) u = u0,
 * subject to certain Dirichlet boundary conditions
*/

template <class Mesh>
CINO_INLINE
ScalarField heat_flow(const Mesh             & m,
                      const std::vector<int> & heat_charges,
                      const double             time = 100.0,
                      const int                laplacian_mode = COTANGENT)
{
    assert(heat_charges.size() > 0);

    timer_start("Compute Heat Flow");

    ScalarField heat(m.num_vertices());

    Eigen::SparseMatrix<double> L   = laplacian<Mesh>(m, laplacian_mode);
    Eigen::SparseMatrix<double> M   = mass_matrix<Mesh>(m);
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m.num_vertices());

    std::map<int,double> bc;
    for(int i=0; i<(int)heat_charges.size(); ++i)
    {
        bc[heat_charges[i]] = 1.0;
    }

    solve_square_system_with_bc(M - time * L, rhs, heat, bc);

    timer_stop("Compute Heat Flow");

    return heat;
}

}

#endif // CINO_HEAT_FLOW_H
