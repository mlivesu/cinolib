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
#ifndef CINO_TETRAHEDRALIZATION_H
#define CINO_TETRAHEDRALIZATION_H

#include <cinolib/meshes/meshes.h>

/* The split schemes implemented here are largely based on:
 *
 * How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra
 * J.Dompierre, P.Labbé, M.G. Vallet, R.Camarero
 * International Meshing Roundtable, pp.195-204, 1999
*/

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Transforms a hexahedral mesh into a conforming tetrahedral
// mesh, splitting all hexahedra into five or six tetrahedra.
template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_to_tets(const Hexmesh<M,V,E,F,P> & hm,
                       Tetmesh<M,V,E,F,P> & tm);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Subdivides a hexahedron either into 5 tets or into 6 tets
// (according to four poossible schemes). Split schemes are
// chosen in order to grant a global conforming tetrahedral
// mesh, where diagonals of quad faces are compatible across
// face-adjacent hexahedra.
CINO_INLINE
void hex_to_tets(const std::vector<uint> & hex,
                       std::vector<uint> & tets);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Subdivides a prism with triangular base into 3 tets.
// Prism vertices are assumed in the following order:
//
//      v2             v5
//    /    \         /    \
//  v0 --- v1      v3 --- v4
//
//   bot base       top base
//
CINO_INLINE
void prism_to_tets(const std::vector<uint> & prism,
                         std::vector<uint> & tets);
}

#ifndef  CINO_STATIC_LIB
#include "tetrahedralization.cpp"
#endif

#endif // CINO_TETRAHEDRALIZATION_H
