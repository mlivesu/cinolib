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
#include <cinolib/timer.h>

namespace cinolib
{

CINO_INLINE
void MCF(Trimesh      & m,
         const int      n_iters,
         const double   time,
         const bool     conformalized)
{
    char msg[1024];
    sprintf(msg, "Mean Curvature Flow - time step: %f - n_iters: %d - conformalized: %d", time, n_iters, conformalized);

    timer_start(msg);

    Eigen::SparseMatrix<double> L = laplacian<Trimesh>(m, COTANGENT);
    Eigen::SparseMatrix<double> M = mass_matrix<Trimesh>(m);

    for(int i=1; i<=n_iters; ++i)
    {
        // this is for numerical precision: try to stay within
        // the denser part of the machine float representation
        //
        m.normalize_area();
        m.center_bbox();

        // backward euler time integration of heat flow equation
        //
        Eigen::SimplicialLLT< Eigen::SparseMatrix<double> > solver(M - time * L);

        int nv = m.num_vertices();
        Eigen::VectorXd x(nv);
        Eigen::VectorXd y(nv);
        Eigen::VectorXd z(nv);

        for(int vid=0; vid<nv; ++vid)
        {
            vec3d pos = m.vertex(vid);
            x[vid] = pos.x();
            y[vid] = pos.y();
            z[vid] = pos.z();
        }

        x = solver.solve(M * x);
        y = solver.solve(M * y);
        z = solver.solve(M * z);

        double residual = 0.0;
        for(int vid=0; vid<m.num_vertices(); ++vid)
        {
            vec3d new_pos(x[vid], y[vid], z[vid]);
            residual += (m.vertex(vid) - new_pos).length();
            m.set_vertex(vid, new_pos);
        }

        logger << "MCF iter: " << i << " residual: " << residual << endl;

        if (i<n_iters) // update matrices for the next iteration
        {
            M = mass_matrix<Trimesh>(m);
            if (!conformalized) L = laplacian<Trimesh>(m, COTANGENT);
        }
    }

    timer_stop(msg);
}

}
