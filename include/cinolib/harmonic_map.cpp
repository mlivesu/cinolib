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
#include <cinolib/laplacian.h>
#include <Eigen/Sparse>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
ScalarField harmonic_map(const AbstractMesh<M,V,E,P> & m,
                         const std::map<uint,double> & bc,
                         const uint                    n,
                         const int                     laplacian_mode,
                         const int                     solver)
{
    assert(n > 0);
    assert(bc.size() > 0);
    assert(laplacian_mode == COTANGENT || laplacian_mode == UNIFORM);
    assert(solver == SIMPLICIAL_LLT || solver == SIMPLICIAL_LDLT || solver == SparseLU || solver == BiCGSTAB);

    ScalarField f(m.num_verts());

    Eigen::SparseMatrix<double> L   = laplacian(m, laplacian_mode);
    Eigen::SparseMatrix<double> Ln = -L;
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m.num_verts());

    for(uint i=1; i<n; ++i) Ln  = Ln * (-L); // keep it PSD

    solve_square_system_with_bc(Ln, rhs, f, bc, solver);

    return f;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<vec3d> harmonic_map_3d(const AbstractMesh<M,V,E,P> & m,
                                   const std::map<uint,vec3d>  & bc,
                                   const uint                    n,
                                   const int                     laplacian_mode,
                                   const int                     solver)
{
    assert(n > 0);
    assert(bc.size() > 0);
    assert(laplacian_mode == COTANGENT || laplacian_mode == UNIFORM);
    assert(solver == SIMPLICIAL_LLT || solver == SIMPLICIAL_LDLT || solver == SparseLU || solver == BiCGSTAB);

    ScalarField f(3*m.num_verts());

    Eigen::SparseMatrix<double> L   = laplacian_3d(m, laplacian_mode);
    Eigen::SparseMatrix<double> Ln = -L;
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(3*m.num_verts());

    for(uint i=1; i<n; ++i) Ln  = Ln * (-L); // keep it PSD

    uint y_off = m.num_verts();
    uint z_off = m.num_verts() + y_off;
    std::map<uint,double> bc_1d;
    for(auto obj : bc)
    {
        uint  vid = obj.first;
        vec3d pos = obj.second;
        bc_1d[      vid] = pos.x();
        bc_1d[y_off+vid] = pos.y();
        bc_1d[z_off+vid] = pos.z();
    }

    solve_square_system_with_bc(Ln, rhs, f, bc_1d, solver);

    std::vector<vec3d> res(m.num_verts());
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        res.at(vid) = vec3d(f[vid], f[y_off+vid], f[z_off+vid]);
    }

    return res;
}

}
