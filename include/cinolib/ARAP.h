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

#include <cinolib/meshes/abstract_mesh.h>
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

    std::vector<mat3d> R;       // local (per vertex) rotation matrices
    std::vector<vec3d> xyz_ref; // reference (original) vertex positions

    // edge weights
    std::vector<double> w;
    int w_type = UNIFORM; // { UNIFORM, COTANGENT }

    Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> cache; // factorized matrix
    Eigen::SparseMatrix<double> A; // a copy of the matrix (to be pre-multiplied to the rhs to form the normal equations)

    // deformation handles, separated for x,y,z coords to make solver call easier
    std::vector<uint>     handles;
    std::map<uint,double> handles_x;
    std::map<uint,double> handles_y;
    std::map<uint,double> handles_z;

    bool hard_constrain_handles = false;
    std::vector<int> col_map; // column map (to handle hard constrainted handles)
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
