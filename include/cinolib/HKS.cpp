/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2025: Marco Livesu                                              *
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
#include <cinolib/HKS.h>
#include <cinolib/laplacian.h>
#include <cinolib/linear_solvers.h>
#include <cinolib/scalar_field.h>
#include <cinolib/vertex_mass.h>
#include <cinolib/sampling.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::MatrixXd HKS(      AbstractMesh<M,V,E,P> & m,
                    const std::vector<uint>     & landmarks,
                    const uint                    n_timesteps,
                    const bool                    normalize_mesh,
                    const bool                    normalize_columns,
                    const bool                    verbose)
{
    if(normalize_mesh) m.normalize_bbox();

    uint n_rows = m.num_verts();
    uint n_cols = landmarks.size()*n_timesteps;
    Eigen::MatrixXd A(n_rows,n_cols);

    Eigen::SparseMatrix<double> L  = laplacian(m, COTANGENT);
    Eigen::SparseMatrix<double> MM = mass_matrix(m);

    // This is what Dorian Nogneng and Maks Ovsjanikov do in their reference code for the paper:
    //
    //      Informative Descriptor Preservation via Commutativity for Shape Matching
    //      Eurographics 2017
    //
    std::vector<double> timesteps = sample_within_interval(log(0.005), log(0.2), n_timesteps);
    for(uint i=0; i<n_timesteps; ++i) timesteps[i] = exp(timesteps[i]);

    uint col = 0;
    for(auto t : timesteps)
    {
        Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver(MM - t*L);
        assert(solver.info() == Eigen::Success);

        for(auto v : landmarks)
        {
            if(verbose) std::cout << "column " << col << ": time step " << t << ", landmark " << v << std::endl;

            Eigen::VectorXd rhs = Eigen::VectorXd::Zero(m.num_verts());
            rhs[v] = 1.0;
            ScalarField f = solver.solve(rhs).eval();
            if(normalize_columns) f.normalize_in_01(); // Useful for visualization but "physically wrong"...
            for(uint vid=0; vid<m.num_verts(); ++vid) A.coeffRef(vid,col) = f[vid];
            ++col;
        }
    }
    return A;
}

}
