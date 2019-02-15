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
#include <cinolib/IRLS.h>
#include <cinolib/p_norm.h>

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
                const int    max_iter,
                const double conv_thresh,
                const int    solver)
{
    Eigen::VectorXd w(A.rows());
    for(int i=0; i<A.rows(); ++i) w[i] = 1.0;

    double res      = 0;
    double prev_res = 0;
    int    iter     = 0;
    do
    {
        prev_res = res;

        // minimize L2
        solve_weighted_least_squares(A, w, b, x, solver);

        // update weights
        for(int i=0; i<A.rows(); ++i)
        {
            double e = std::fabs(b[i] - A.row(i)*x);
            w[i] = std::min(std::pow(e, p-2.0), 1000.0);
        }
        w.normalize();

        if(max_iter>0 && ++iter>max_iter) return;

        res = p_norm(b - A*x, A.rows(), p);
        //std::cout << "res: " << res << "\tdelta: " << std::fabs(res-prev_res) << std::endl;
    }
    while(std::fabs(res-prev_res)>conv_thresh);
}

}
