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
#ifndef CINO_IRLS_H
#define CINO_IRLS_H

#include <Eigen/Sparse>
#include <cinolib/linear_solvers.h>

namespace cinolib
{

/* Implementation of Iteratively Reweighted Least Squares (IRLS in short)
 * to minimize energies subject to a p-norm
 *
 * arg min E = || Ax - b ||p
 *      x
 *
 * for further details, see:
 *
 * Iterative Reweighted Least Squares
 * C. Sidney Burrus
 * and
 * https://en.wikipedia.org/wiki/Iteratively_reweighted_least_squares
*/

CINO_INLINE
void solve_IRLS(const Eigen::SparseMatrix<double> & A,
                const Eigen::VectorXd             & b,
                      Eigen::VectorXd             & x,
                const double p,
                const int    max_iter = 100,     // -1 to have unlimited iterations
                const double conv_thresh = 1e-6, // res difference across subsequent iterations
                const int    solver = SIMPLICIAL_LLT);
}

#ifndef  CINO_STATIC_LIB
#include "IRLS.cpp"
#endif

#endif // CINO_IRLS_H
