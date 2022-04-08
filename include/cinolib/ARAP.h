/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#ifndef CINO_ARAP_H
#define CINO_ARAP_H

#include <cinolib/meshes/trimesh.h>
#include <cinolib/linear_solvers.h>

namespace cinolib
{

/* Implementation of the ARAP surface modelling algorithm as described in:
 *
 *   As-Rigid-As-Possible Surface Modeling
 *   Olga Sorkine-Hornung, Marc Alexa
 *   Eurographics Symposium on Geometry Processing 2007
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct ARAP_data
{
    uint n_iters = 4;
    bool init = true; // initialize just once (useful for multiple calls, e.g. to make more iterations)

    std::map<uint,vec3d> bcs;     // boundary conditions
    std::vector<int>     col_map; // maps vertex ids to matrix columns

    std::vector<vec3d>  xyz_out; // current solution (will be the output, eventually)
    std::vector<vec3d>  xyz_loc; // per element targets (100% rigid)
    std::vector<double> w;       // edge weights { UNIFORM, COTANGENT }
    int w_type = UNIFORM;        // WARNING: cot weights seem rather unstable on volume meshes in interactive deformations

    Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> cache; // factorized matrix

    // In my experience replacing hard with soft constraints works
    // much better for interactive shape deformation (no artifacts
    // due to C-0 continuity around boundary conditions).
    // On the other hand, things like simplicial volume maps require
    // the use of hard constraints to ensure boundary conformity.
    bool   use_soft_constraints = true;
    double w_constr  = 100.0;      // weight for soft constraints
    double w_laplace = 1.0;        // weight for the laplacian component of the matrix
    Eigen::VectorXd W;             // diagonal matrix of weights
    Eigen::SparseMatrix<double> A; // a copy of the matrix (to be pre-multiplied to the rhs to form the normal equations)

    // if true (default), the warm start will be the minimizer of
    // | Lx - delta |^2
    // where delta are the differential mesh coordinates. If false,
    // the warm start will simply be a copy of the input mesh, with
    // constrained vertices moved onto their prescribed position
    bool warm_start_with_laplacian = true;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void ARAP(AbstractMesh<M,V,E,P> & m, ARAP_data & data);

}

#ifndef  CINO_STATIC_LIB
#include "ARAP.cpp"
#endif

#endif // CINO_ARAP_H
