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
#include <cinolib/harmonic_map.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
ScalarField harmonic_map(const AbstractMesh<M,V,E,P> & m,
                         std::map<uint,double>       & bc,
                         const uint                    n,
                         const int                     laplacian_mode,
                         const int                     solver)
{
    profiler.push("cinolib::harmonic_map");

    assert(n > 0);
    assert(bc.size() > 0);
    assert(laplacian_mode == COTANGENT || laplacian_mode == UNIFORM);
    assert(solver == SIMPLICIAL_LLT || solver == SIMPLICIAL_LDLT || solver == SparseLU || solver == BiCGSTAB);

    ScalarField f(m.num_verts());

    Eigen::SparseMatrix<double> L   = laplacian(m, laplacian_mode);
    Eigen::SparseMatrix<double> Ln = -L;
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m.num_verts());

    for(uint i=1; i<n; ++i) Ln  = Ln * L;

    solve_square_system_with_bc(Ln, rhs, f, bc, solver);

    profiler.pop();

    return f;
}

}
