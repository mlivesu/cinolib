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
