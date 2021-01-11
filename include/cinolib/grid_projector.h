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
#ifndef CINO_GRID_PROJECTOR_H
#define CINO_GRID_PROJECTOR_H

#include <cinolib/meshes/meshes.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    double conv_thresh = 1e-4;  // convergence threshold (either H or mean distance from target)
    uint   max_iter    = 10;    // force convergence after a maximum number of iterations
    bool   use_H_dist  = false; // uses Hausdorff distance if true. Average distance otherwise
    double SJ_thresh   = 0;     // minimum threshold for SJ (elements must be strictly above the thresh...)
}
GridProjectorOptions;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M1, class V1, class E1, class F1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
double grid_projector(      Hexmesh<M1,V1,E1,F1,P1> & m,
                      const Trimesh<M2,V2,E2,P2>    & srf,
                      const GridProjectorOptions    & opt = GridProjectorOptions());

}

#ifndef  CINO_STATIC_LIB
#include "grid_projector.cpp"
#endif

#endif // CINO_GRID_PROJECTOR_H
