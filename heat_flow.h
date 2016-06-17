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
#ifndef HEAT_FLOW_H
#define HEAT_FLOW_H

#include "cinolib.h"
#include "scalar_field.h"
#include "laplacian.h"
#include "vertex_mass.h"
#include "linear_solvers.h"
#include "timer.h"
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

#endif // HEAT_FLOW_H
