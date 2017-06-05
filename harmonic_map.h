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
#ifndef CINO_HARMONIC_MAP_H
#define CINO_HARMONIC_MAP_H

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
ScalarField harmonic_map(const Mesh            & m,
                         std::map<uint,double> & bc,
                         const uint              n = 1,
                         const int               laplacian_mode = COTANGENT,
                         const int               solver = SIMPLICIAL_LLT)
{
    assert(n > 0);
    assert(bc.size() > 0);
    assert(laplacian_mode == COTANGENT || laplacian_mode == UNIFORM);
    assert(solver == SIMPLICIAL_LLT || solver == SIMPLICIAL_LDLT || solver == SparseLU || solver == BiCGSTAB);

    timer_start("Compute Harmonic Map");

    ScalarField f(m.num_verts());

    Eigen::SparseMatrix<double> L   = laplacian<Mesh>(m, laplacian_mode);
    Eigen::SparseMatrix<double> Ln = -L;
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m.num_verts());

    for(uint i=1; i<n; ++i) Ln  = Ln * L;

    solve_square_system_with_bc(Ln, rhs, f, bc, solver);

    timer_stop("Compute Harmonic Map");

    return f;
}

}

#endif // CINO_HARMONIC_MAP_H
