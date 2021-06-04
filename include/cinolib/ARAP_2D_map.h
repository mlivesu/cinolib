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
#ifndef CINO_ARAP_2D_MAP_H
#define CINO_ARAP_2D_MAP_H

#include <cinolib/meshes/trimesh.h>
#include <cinolib/linear_solvers.h>

namespace cinolib
{

/* Implementation of the ARAP UV mapping algorithm as described in:
 *
 *   A Local/Global Approach to Mesh Parameterization
 *   Ligang Liu, Lei Zhang, Yin Xu, Craig Gotsman and Steven J. Gortler
 *   Eurographics Symposium on Geometry Processing 2008
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct ARAP_2D_map_data
{
    uint n_iters = 4;
    bool init    = true; // initialize just once (useful for multiple calls, e.g. to make more iterations)

    std::vector<vec2d>  uv;     // output uv coords
    std::vector<vec2d>  uv_ref; // per triangle reference uv coords (w.r.t. a local frame)
    std::vector<vec2d>  uv_loc; // per triangle uv targets (100% rigid)
    std::vector<double> w;      // edge weights (cotangent)

    Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> cache; // factorized matrix
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void ARAP_2D_mapping(const Trimesh<M,V,E,P> & m, ARAP_2D_map_data & data);

}

#ifndef  CINO_STATIC_LIB
#include "ARAP_2D_map.cpp"
#endif

#endif // CINO_ARAP_2D_MAP_H
