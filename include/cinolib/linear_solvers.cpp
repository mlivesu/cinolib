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
