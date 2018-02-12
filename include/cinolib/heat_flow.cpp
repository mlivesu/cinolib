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
