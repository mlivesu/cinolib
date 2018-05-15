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
#include <cinolib/linear_solvers.h>
#include <cinolib/stl_container_utilities.h>

namespace cinolib
{

typedef Eigen::Triplet<double> Entry;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_square_system(const Eigen::SparseMatrix<double> & A,
                         const Eigen::VectorXd             & b,
                               Eigen::VectorXd             & x,
                         int   solver)
{
    assert(A.rows() == A.cols());

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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_square_system_with_bc(const Eigen::SparseMatrix<double> & A,
                                 const Eigen::VectorXd             & b,
                                       Eigen::VectorXd             & x,
                                 const std::map<uint,double>       & bc, // Dirichlet boundary conditions
                                 int   solver)
{
    std::vector<int> col_map(A.rows(), -1);
    uint fresh_id = 0;
    for(uint col=0; col<A.cols(); ++col)
    {
        if (DOES_NOT_CONTAIN(bc, col))
        {
            col_map[col] = fresh_id++;
        }
    }

    uint size = A.rows() - bc.size();

    std::vector<Entry> Aprime_entries;
    Eigen::VectorXd    bprime(size);

    for(uint row=0; row<A.rows(); ++row)
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
    for (uint i=0; i<A.outerSize(); ++i)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(A,i); it; ++it)
        {
            uint    row = it.row();
            uint    col = it.col();
            double val = it.value();

            if (col_map[row] < 0) continue;

            if (col_map[col] < 0)
            {
                bprime[ col_map[row] ] -= bc.at(col) * val;
            }
            else
            {
                Aprime_entries.push_back(Entry(col_map[row], col_map[col], val));
            }
        }
    }

    Eigen::SparseMatrix<double> Aprime(size, size);
    Aprime.setFromTriplets(Aprime_entries.begin(), Aprime_entries.end());

    Eigen::VectorXd tmp_x(size);

    solve_square_system(Aprime, bprime, tmp_x, solver);

    x.resize(A.cols());
    for(uint col=0; col<A.cols(); ++col)
    {
        if (col_map[col] >= 0)
        {
            x[col] = tmp_x[ col_map[col] ];
        }
        else
        {
            x[col] = bc.at(col);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_least_squares(const Eigen::SparseMatrix<double> & A,
                         const Eigen::VectorXd             & b,
                               Eigen::VectorXd             & x,
                         int   solver)
{
    Eigen::SparseMatrix<double> At  = A.transpose();
    Eigen::SparseMatrix<double> AtA = At * A;
    Eigen::VectorXd             Atb = At * b;

    solve_square_system(AtA, Atb, x, solver);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_least_squares_with_bc(const Eigen::SparseMatrix<double> & A,
                                 const Eigen::VectorXd             & b,
                                       Eigen::VectorXd             & x,
                                 const std::map<uint,double>       & bc, // Dirichlet boundary conditions
                                 int   solver)
{
    Eigen::SparseMatrix<double> At  = A.transpose();
    Eigen::SparseMatrix<double> AtA = At * A;
    Eigen::VectorXd             Atb = At * b;

    solve_square_system_with_bc(AtA, Atb, x, bc, solver);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_weighted_least_squares(const Eigen::SparseMatrix<double> & A,
                                  const Eigen::VectorXd             & w,
                                  const Eigen::VectorXd             & b,
                                        Eigen::VectorXd             & x,
                                  int   solver)
{
    Eigen::SparseMatrix<double> At   = A.transpose();
    Eigen::SparseMatrix<double> AtWA = At * w.asDiagonal() * A;
    Eigen::VectorXd             AtWb = At * w.asDiagonal() * b;

    solve_square_system(AtWA, AtWb, x, solver);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_weighted_least_squares_with_bc(const Eigen::SparseMatrix<double> & A,
                                          const Eigen::VectorXd             & w,
                                          const Eigen::VectorXd             & b,
                                                Eigen::VectorXd             & x,
                                          const std::map<uint,double>       & bc, // Dirichlet boundary conditions
                                          int   solver)
{
    Eigen::SparseMatrix<double> At   = A.transpose();
    Eigen::SparseMatrix<double> AtWA = At * w.asDiagonal() * A;
    Eigen::VectorXd             AtWb = At * w.asDiagonal() * b;

    solve_square_system_with_bc(AtWA, AtWb, x, bc, solver);
}



}
