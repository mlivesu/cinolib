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
#ifndef CINO_LINEAR_SOLVERS_H
#define CINO_LINEAR_SOLVERS_H

#include <cinolib/cinolib.h>
#include <eigen3/Eigen/Sparse>
#include <string>

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

static const std::string txt[4] =
{
    "SIMPLICIAL_LLT"  ,
    "SIMPLICIAL_LDLT" ,
    "SparseLU",
    "BiCGSTAB",
};


CINO_INLINE
void solve_square_system(const Eigen::SparseMatrix<double> & A,
                         const Eigen::VectorXd             & b,
                               Eigen::VectorXd             & x,
                         int   solver = SIMPLICIAL_LLT);

CINO_INLINE
void solve_square_system_with_bc(const Eigen::SparseMatrix<double> & A,
                                 const Eigen::VectorXd             & b,
                                       Eigen::VectorXd             & x,
                                       std::map<int,double>        & bc, // Dirichlet boundary conditions
                                 int   solver = SIMPLICIAL_LLT);


void solve_least_squares(const Eigen::SparseMatrix<double> & A,
                         const Eigen::VectorXd             & b,
                               Eigen::VectorXd             & x,
                         int   solver = SIMPLICIAL_LLT);


}

#ifndef  CINO_STATIC_LIB
#include "linear_solvers.cpp"
#endif

#endif // CINO_LINEAR_SOLVERS_H
