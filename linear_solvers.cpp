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
#include <cinolib/linear_solvers.h>
#include <cinolib/timer.h>

namespace cinolib
{

CINO_INLINE
void solve_square_system(const Eigen::SparseMatrix<double> & A,
                         const Eigen::VectorXd             & b,
                               Eigen::VectorXd             & x,
                         int   solver)
{
    assert(A.rows() == A.cols());

    char msg[1024];
    sprintf(msg, "Linear solve - %d x %d Matrix - %s", A.rows(), A.cols(), txt[solver].c_str());

    timer_start(msg);

    switch (solver)
    {
        case SIMPLICIAL_LLT:
        {
            Eigen::SimplicialLLT< Eigen::SparseMatrix<double> > solver(A);
            assert(solver.info() == Eigen::Success);
            x = solver.solve(b).eval();
            break;
        }

        case SIMPLICIAL_LDLT:
        {
            Eigen::SimplicialLDLT< Eigen::SparseMatrix<double> > solver(A);
            assert(solver.info() == Eigen::Success);
            x = solver.solve(b).eval();
            break;
        }

        case BiCGSTAB:
        {
            Eigen::BiCGSTAB< Eigen::SparseMatrix<double> , Eigen::IncompleteLUT<double> > solver;
            //solver.setMaxIterations(100);
            solver.setTolerance(1e-5);
            solver.compute(A);
            assert(solver.info() == Eigen::Success);
            x = solver.solve(b).eval();
            break;
        }

        case SparseLU:
        {
            Eigen::SparseMatrix<double> Ac = A;
            Ac.makeCompressed();
            Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int> > solver;
            solver.analyzePattern(Ac);
            solver.factorize(Ac);
            x = solver.solve(b);
            break;
        }

        default: assert(false && "Unknown Solver");
    }

    timer_stop(msg);
}


CINO_INLINE
void solve_square_system_with_bc(const Eigen::SparseMatrix<double> & A,
                                 const Eigen::VectorXd             & b,
                                       Eigen::VectorXd             & x,
                                       std::map<int,double>        & bc, // Dirichlet boundary conditions
                                 int   solver)
{
    char msg[1024];
    sprintf(msg, "Linear solve with %d BCs - precomputation", (int)bc.size());

    timer_start(msg);

    std::vector<int> col_map(A.rows(), -1);
    int fresh_id = 0;
    for(int col=0; col<A.cols(); ++col)
    {
        if (DOES_NOT_CONTAIN(bc, col))
        {
            col_map[col] = fresh_id++;
        }
    }

    int size = A.rows() - bc.size();

    Eigen::SparseMatrix<double> Aprime(size,size);
    Eigen::VectorXd bprime(size);

    for(int row=0; row<A.rows(); ++row)
    {
        if (col_map[row] >= 0)
        {
            bprime[ col_map[row] ] = b[row];
        }
    }

    //
    // iterate over the non-zero entries
    // of sparse matrix A
    //
    for (int i=0; i<A.outerSize(); ++i)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(A,i); it; ++it)
        {
            int row = it.row();
            int col = it.col();

            if (col_map[row] < 0) continue;

            if (col_map[col] < 0)
            {
                //assert(CONTAINS(bc, col));
                bprime[ col_map[row] ] -= bc[col] * it.value();
            }
            else
            {
                Aprime.coeffRef( col_map[row], col_map[col] ) = it.value();
            }
        }
    }

    timer_stop(msg);

    Eigen::VectorXd tmp_x(size);

    solve_square_system(Aprime, bprime, tmp_x, solver);

    x.resize(A.cols());
    for(int col=0; col<A.cols(); ++col)
    {
        if (col_map[col] >= 0)
        {
            x[col] = tmp_x[ col_map[col] ];
        }
        else
        {
            //assert(CONTAINS(bc, col));
            x[col] = bc[col];
        }
    }
}


CINO_INLINE
void solve_least_squares(const Eigen::SparseMatrix<double> & A,
                         const Eigen::VectorXd             & b,
                               Eigen::VectorXd             & x,
                         int   solver)
{
    char msg[1024];
    sprintf(msg, "Least Squares Solve - %d x %d Matrix - %s", A.rows(), A.cols(), txt[solver].c_str());

    timer_start(msg);

    Eigen::SparseMatrix<double> At  = A.transpose();
    Eigen::SparseMatrix<double> AtA = At * A;
    Eigen::VectorXd             Atb = At * b;

    solve_square_system(AtA, Atb, x, solver);

    timer_stop(msg);
}

}
