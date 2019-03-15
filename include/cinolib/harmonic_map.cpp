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

    Eigen::SparseMatrix<double> L   = laplacian(m, laplacian_mode, 3);
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
