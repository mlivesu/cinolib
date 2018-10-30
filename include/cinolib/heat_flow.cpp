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
#include <cinolib/heat_flow.h>
#include <cinolib/laplacian.h>
#include <cinolib/vertex_mass.h>
#include <cinolib/linear_solvers.h>
#include <Eigen/Sparse>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
ScalarField heat_flow(const AbstractMesh<M,V,E,P> & m,
                      const std::vector<uint>     & heat_charges,
                      const double                  time,
                      const int                     laplacian_mode,
                      const bool                    hard_contraint_bcs)
{
    assert(heat_charges.size() > 0);

    ScalarField heat(m.num_verts());

    Eigen::SparseMatrix<double> L   = laplacian(m, laplacian_mode);
    Eigen::SparseMatrix<double> MM  = mass_matrix(m);
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m.num_verts());

    if (hard_contraint_bcs) // heat flow as a boundary problem (charges do not lose heat)
    {
        std::map<uint,double> bcs;
        for(uint vid: heat_charges) bcs[vid] = 1.0;
        solve_square_system_with_bc(MM - time * L, rhs, heat, bcs);
    }
    else // heat flow as a diffusion problem (charges lose heat)
    {
        for(uint vid : heat_charges) rhs[vid] = 1.0;
        solve_square_system(MM - time * L, rhs, heat);
    }


    return heat;
}

}
