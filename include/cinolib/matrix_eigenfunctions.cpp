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
#include <cinolib/matrix_eigenfunctions.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/MatOp/SparseGenRealShiftSolve.h>
#include <Spectra/GenEigsRealShiftSolver.h>
#include <Spectra/SymEigsShiftSolver.h>
#include <cinolib/min_max_inf.h>

namespace cinolib
{

CINO_INLINE
bool matrix_eigenfunctions(const Eigen::SparseMatrix<double> & m,
                           const bool                          sym,
                           const int                           nf,
                                 std::vector<double>         & f,
                                 std::vector<double>         & f_min,
                                 std::vector<double>         & f_max)
{
    int nc = m.cols();
    f.resize(m.cols()*nf);
    f_min.resize(nf);
    f_max.resize(nf);

    if(sym) // symmetric real matrix
    {
        // https://github.com/yixuan/spectra/issues/149#issuecomment-1398594081
        Spectra::SparseSymShiftSolve<double> op(m);
        Spectra::SymEigsShiftSolver<Spectra::SparseSymShiftSolve<double>> eigs(op, nf, 2*nf+1, -0.001);
        eigs.init();
        eigs.compute(Spectra::SortRule::LargestMagn); // largest beacuse it's inverted
        if(eigs.info()!=Spectra::CompInfo::Successful) return false;
        // assemble output data
        Eigen::MatrixXd basis_func = eigs.eigenvectors();
        for(int fid=0; fid<basis_func.cols(); ++fid)
        {
            double min = max_double;
            double max = min_double;
            for(int i=0; i<nc; ++i)
            {
                auto coeff = basis_func(i,fid);
                f.at(fid*nc+i) = coeff;
                max = std::max(max, std::fabs(coeff));
                min = std::min(min, -max);
            }
            f_min.at(fid) = min;
            f_max.at(fid) = max;
        }
    }
    else // generic real matrix
    {
        // https://github.com/yixuan/spectra/issues/149#issuecomment-1398594081
        Spectra::SparseGenRealShiftSolve<double> op(m);
        Spectra::GenEigsRealShiftSolver<Spectra::SparseGenRealShiftSolve<double>> eigs(op, nf, 2*nf+1, -0.001);
        eigs.init();
        eigs.compute(Spectra::SortRule::LargestMagn); // largest beacuse it's inverted
        if(eigs.info()!=Spectra::CompInfo::Successful) return false;
        // assemble output data
        auto basis_func = eigs.eigenvectors();
        for(int fid=0; fid<basis_func.cols(); ++fid)
        {
            double min = max_double;
            double max = min_double;
            int    off = nf-1-fid; // reverse the order
            for(int i=0; i<nc; ++i)
            {
                auto coeff = basis_func(i,fid);
                if(coeff.imag()!=0) return false; // fail if there are complex coefficients
                f.at(off*nc+i) = coeff.real();
                max = std::max(max, std::fabs(coeff.real()));
                min = std::min(min, -max);
            }
            f_min.at(off) = min;
            f_max.at(off) = max;
        }
    }
    return true;
}

}
