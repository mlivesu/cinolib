/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include "mean_curv_flow.h"
#include "laplacian.h"
#include "linear_solvers.h"
#include "vertex_mass.h"
#include "timer.h"

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

        std::cout << "MCF iter: " << i << " residual: " << residual << std::endl;

        if (i<n_iters) // update matrices for the next iteration
        {
            M = mass_matrix<Trimesh>(m);
            if (!conformalized) L = laplacian<Trimesh>(m, COTANGENT);
        }
    }

    timer_stop(msg);
}

}
