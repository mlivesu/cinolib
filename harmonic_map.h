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
#ifndef HARMONIC_MAP_H
#define HARMONIC_MAP_H

#include <cinolib/scalar_field.h>
#include <cinolib/laplacian.h>
#include <cinolib/linear_solvers.h>
#include <cinolib/timer.h>
#include <eigen3/Eigen/Sparse>


namespace cinolib
{

/*
 * Solve the (n)-harmonic problem  L phi = 0,
 * subject to certain Dirichlet boundary conditions
 *
 * The parameter n control the smoothness of phi. That is:
 *
 * n = 1  | harmonic    | C^0 at boundary conditions, C^1 everywhere else
 * n = 2  | biharmonic  | C^1 at boundary conditions, C^2 everywhere else
 * n = 3  | triharmonic | C^2 at boundary conditions, C^3 everywhere else
 * ...
 *
 * WARNING: the matrix is guaranteed Positive Semi Definite only for n=1. If
 * higher values of n are chosen I suggest avoiding the SIMPLICIAL_LLT solver
 * and using something like BiCGSTAB instead.
 *
 * WARNING: COTANGENT weights usually provide better results but may fail if
 * the mesh contains degenerate elements. In such cases I suggest using UNIFORM
 * weights instead.
*/

template <class Mesh>
CINO_INLINE
ScalarField harmonic_map(const Mesh           & m,
                         std::map<int,double> & bc,
                         const int              n = 1,
                         const int              laplacian_mode = COTANGENT,
                         const int              solver = SIMPLICIAL_LLT)
{
    assert(n > 0);
    assert(bc.size() > 0);

    timer_start("Compute Harmonic Map");

    ScalarField f(m.num_vertices());

    Eigen::SparseMatrix<double> L   = laplacian<Mesh>(m, laplacian_mode);
    Eigen::SparseMatrix<double> Ln = -L;
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m.num_vertices());

    for(int i=1; i<n; ++i) Ln  = Ln * L;

    solve_square_system_with_bc(Ln, rhs, f, bc, solver);

    timer_stop("Compute Harmonic Map");

    return f;
}

}

#endif // HARMONIC_MAP_H
