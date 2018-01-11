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
#include <cinolib/mean_curv_flow.h>
#include <cinolib/laplacian.h>
#include <cinolib/linear_solvers.h>
#include <cinolib/vertex_mass.h>
#include <cinolib/symbols.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void MCF(AbstractPolygonMesh<M,V,E,P> & m,
         const uint                     n_iters,
         const double                   time_scalar,
         const bool                     conformalized)
{
    // use the squared avg edge length as time step, as suggested in:
    // Geodesics in Heat: A New Approach to Computing Distance Based on Heat Flow
    // K.Crane, C.Weischedel, M.Wardetzky
    // SIGGRAPH 2013
    double time = m.edge_avg_length();
    time *= time;
    time *= time_scalar;

    Eigen::SparseMatrix<double> L  = laplacian(m, COTANGENT);
    Eigen::SparseMatrix<double> MM = mass_matrix(m);

    for(uint i=1; i<=n_iters; ++i)
    {
        // optimize position and scale to get better numerical precision
        m.normalize_bbox();
        m.center_bbox();        

        // backward euler time integration of heat flow equation
        Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> LLT(MM - time_scalar * L);

        uint nv = m.num_verts();
        Eigen::VectorXd x(nv);
        Eigen::VectorXd y(nv);
        Eigen::VectorXd z(nv);

        for(uint vid=0; vid<nv; ++vid)
        {
            vec3d pos = m.vert(vid);
            x[vid] = pos.x();
            y[vid] = pos.y();
            z[vid] = pos.z();
        }

        x = LLT.solve(MM * x);
        y = LLT.solve(MM * y);
        z = LLT.solve(MM * z);

        double residual = 0.0;
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            vec3d new_pos(x[vid], y[vid], z[vid]);
            residual += (m.vert(vid) - new_pos).length();
            m.vert(vid) = new_pos;
        }

        std::cout << "MCF iter: " << i << " residual: " << residual << std::endl;

        if (i<n_iters) // update matrices for the next iteration
        {
            MM = mass_matrix(m);
            if (!conformalized) L = laplacian(m, COTANGENT);
        }
    }

    m.update_bbox();
}

}
