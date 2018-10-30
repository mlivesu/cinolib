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
#ifndef CINO_LINEAR_SOLVERS_H
#define CINO_LINEAR_SOLVERS_H

#include <string>
#include <map>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <Eigen/Sparse>

namespace cinolib
{

/* http://eigen.tuxfamily.org/dox-devel/group__TutorialLinearAlgebra.html
 *
 * SOLVER       MATRIX REQUIREMENTS         SPEED       ACCURACY
 * --------------------------------------------------------------
 * LLT          positive definite           +++         +
 * --------------------------------------------------------------
 * LDLT         positive semi definite      +           ++
 *              negative semi definite
 * --------------------------------------------------------------
 * BiCGSTAB     none
 * (iterative)
 */

enum
{
    SIMPLICIAL_LLT, // default
    SIMPLICIAL_LDLT,
    SparseLU,
    BiCGSTAB,
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

static const std::string txt[4] =
{
    "SIMPLICIAL_LLT"  ,
    "SIMPLICIAL_LDLT" ,
    "SparseLU",
    "BiCGSTAB",
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_square_system(const Eigen::SparseMatrix<double> & A,
                         const Eigen::VectorXd             & b,
                               Eigen::VectorXd             & x,
                         int   solver = SIMPLICIAL_LLT);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_square_system_with_bc(const Eigen::SparseMatrix<double> & A,
                                 const Eigen::VectorXd             & b,
                                       Eigen::VectorXd             & x,
                                 const std::map<uint,double>       & bc, // Dirichlet boundary conditions
                                 int   solver = SIMPLICIAL_LLT);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_least_squares(const Eigen::SparseMatrix<double> & A,
                         const Eigen::VectorXd             & b,
                               Eigen::VectorXd             & x,
                         int   solver = SIMPLICIAL_LLT);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_least_squares_with_bc(const Eigen::SparseMatrix<double> & A,
                                 const Eigen::VectorXd             & b,
                                       Eigen::VectorXd             & x,
                                 const std::map<uint,double>       & bc, // Dirichlet boundary conditions
                                 int   solver = SIMPLICIAL_LLT);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_weighted_least_squares(const Eigen::SparseMatrix<double> & A,
                                  const Eigen::VectorXd             & w,
                                  const Eigen::VectorXd             & b,
                                        Eigen::VectorXd             & x,
                                  int   solver = SIMPLICIAL_LLT);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void solve_weighted_least_squares_with_bc(const Eigen::SparseMatrix<double> & A,
                                          const Eigen::VectorXd             & w,
                                          const Eigen::VectorXd             & b,
                                                Eigen::VectorXd             & x,
                                          const std::map<uint,double>       & bc, // Dirichlet boundary conditions
                                          int   solver = SIMPLICIAL_LLT);

}

#ifndef  CINO_STATIC_LIB
#include "linear_solvers.cpp"
#endif

#endif // CINO_LINEAR_SOLVERS_H
