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
#ifndef CINO_MEAN_CURV_FLOW_H
#define CINO_MEAN_CURV_FLOW_H

#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

/* For the differences between cassical mean curvature flow (MCF) and
 * conformalized mean curvature flow (cMCF), please refer to:
 *
 * Can Mean-Curvature Flow be Modified to be Non-singular?
 * Michael Kazhdan, Jake Solomon and Mirela Ben-Chen
 * Computer Graphics Forum, 31(5), 2012.
*/

template<class M, class V, class E, class P>
CINO_INLINE
void MCF(AbstractPolygonMesh<M,V,E,P> & m,
         const uint                     n_iters,
         const double                   time_scalar = 0.01, // I suggest very small steps for the conformalized version
         const bool                     conformalized = true);

}

#ifndef  CINO_STATIC_LIB
#include "mean_curv_flow.cpp"
#endif

#endif // CINO_MEAN_CURV_FLOW_H
