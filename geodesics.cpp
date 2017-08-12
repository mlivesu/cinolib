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
#include <cinolib/geodesics.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
ScalarField compute_geodesics(      AbstractPolygonMesh<M,V,E,P> & m,
                              const std::vector<uint>            & heat_charges,
                              const int                            laplacian_mode,
                              const float                          time_scalar)
{
    // optimize position and scale to get better numerical precision
    double d = m.bbox().diag();
    vec3d  c = m.bbox().center();
    m.translate(-c);
    m.scale(1.0/d);

    // use the squared avg edge length as time step, as suggested in the original paper
    double time = m.edge_avg_length();
    time *= time;
    time *= time_scalar;

    Eigen::SparseMatrix<double> L   = laplacian(m, laplacian_mode);
    Eigen::SparseMatrix<double> MM  = mass_matrix(m);
    Eigen::SparseMatrix<double> G   = gradient_matrix_srf(m);
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m.num_verts());

    std::map<uint,double> bc;
    for(uint vid : heat_charges) bc[vid] = 1.0;

    ScalarField heat(m.num_verts());
    solve_square_system_with_bc(MM - time * L, rhs, heat, bc);

    VectorField grad = G * heat;
    grad.normalize();

    ScalarField geodesics(m.num_verts());
    solve_square_system_with_bc(-L, G.transpose() * grad, geodesics, bc);
    geodesics.normalize_in_01();

    // restore original scale and position
    m.scale(d);
    m.translate(c);

    return geodesics;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
ScalarField compute_geodesics(      AbstractPolyhedralMesh<M,V,E,F,P> & m,
                              const std::vector<uint>                 & heat_charges,
                              const int                                 laplacian_mode,
                              const float                               time_scalar)
{
    // optimize position and scale to get better numerical precision
    double d = m.bbox().diag();
    vec3d  c = m.bbox().center();
    m.translate(-c);
    m.scale(1.0/d);

    // use the squared avg edge length as time step, as suggested in the original paper
    double time = m.edge_avg_length();
    time *= time;
    time *= time_scalar;

    Eigen::SparseMatrix<double> L   = laplacian(m, laplacian_mode);
    Eigen::SparseMatrix<double> MM  = mass_matrix_vol(m);
    Eigen::SparseMatrix<double> G   = gradient_matrix(m);
    Eigen::VectorXd             rhs = Eigen::VectorXd::Zero(m.num_verts());

    std::map<uint,double> bc;
    for(uint vid : heat_charges) bc[vid] = 1.0;

    ScalarField heat(m.num_verts());
    solve_square_system_with_bc(MM - time * L, rhs, heat, bc);

    VectorField grad = G * heat;
    grad.normalize();

    ScalarField geodesics(m.num_verts());
    solve_square_system_with_bc(-L, G.transpose() * grad, geodesics, bc);
    geodesics.normalize_in_01();

    // restore original scale and position
    m.scale(d);
    m.translate(c);

    return geodesics;
}

}
