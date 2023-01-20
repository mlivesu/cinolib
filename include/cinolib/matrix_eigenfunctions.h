/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
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
#ifndef CINO_MATRIX_EIGENFUNCTIONS_H
#define CINO_MATRIX_EIGENFUNCTIONS_H

#ifdef CINOLIB_USES_SPECTRA
#include <cinolib/cino_inline.h>
#include <Eigen/Sparse>

namespace cinolib
{
/* This function computes the nf smallest eigenfunctions of a sparse real matrix.
 * It relies on the shift-and-invert solver of Spectra (https://spectralib.org).
 *
 *       m: input (sparse) matrix
 *     sym: true if the matrix is symmetric, false otherwise
 *      nf: number of functions to extract
 *       f: serialized list of coefficients for all the nf requested functions. Each function
 *          is represented as a list of #C real coefficients, with #C being the number of columns
 *   f_min: min coefficients for each one of the nf requested functions (useful for visual plots)
 *   f_max: max coefficients for each one of the nf requested functions (useful for visual plots)
 *  return: true if the computation of all eigenfunctions converged and all coefficients are real.
 *          false in case the solver did not converge or the eigendecomposition contains complex
 *          coefficients (it may happen only for non symmetric matrices)
*/
CINO_INLINE
bool matrix_eigenfunctions(const Eigen::SparseMatrix<double> & m,
                           const bool                          sym,
                           const int                           nf,
                                 std::vector<double>         & f,
                                 std::vector<double>         & f_min,
                                 std::vector<double>         & f_max);
}

#ifndef  CINO_STATIC_LIB
#include "matrix_eigenfunctions.cpp"
#endif

#endif // CINOLIB_USES_SPECTRA
#endif // CINO_MATRIX_EIGENFUNCTIONS_H
