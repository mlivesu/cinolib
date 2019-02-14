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
#ifndef CINO_SAMPLE_MESH_H
#define CINO_SAMPLE_MESH_H

#include <cinolib/meshes/meshes.h>

namespace cinolib
{

/* This is just an awful method that selects a subset of mesh vertices from an input mesh.
 * It works by starting from a random vertex, and iteratively picking as next vertex the one
 * that is furthest from all the previously selected vertices. Distance computation is cheap
 * because the amortized heat based geodesics are used.
 *
 * For a more serious resource to solve this problem, one may refer to:
 *
 * Efficient and Flexible Sampling with Blue Noise Properties of Triangular Meshes
 * IEEE Transactions on Visualization and Computer Graphics (2012)
 * M.Corsini, P.Cignoni, R.Scopigno
*/

template<class M, class V, class E, class P>
CINO_INLINE
void sample_mesh(AbstractPolygonMesh<M,V,E,P> & m, const uint n_samples, std::vector<uint> & samples);

}

#ifndef  CINO_STATIC_LIB
#include "sample_mesh.cpp"
#endif

#endif // CINO_SAMPLE_MESH
