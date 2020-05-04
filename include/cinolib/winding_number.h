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
#ifndef CINO_VERTEX_MASS_H
#define CINO_VERTEX_MASS_H

#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

/* Computation of winding numbers for general polygonal meshes.
 * https://en.wikipedia.org/wiki/Winding_number
 *
 * NOTE: for non simplicial meshes (e.g. quads, exagons). The
 * interior triangulation of each facet will be used for the
 * computation. If polygons are non planar, then the result
 * becomes tessellation dependent (e.g. what diagonal in a non
 * planar quad?)
 *
 * WARNING: input meshes are assumed to be watertight 2 manifolds.
 * No explicit checks are performed.
*/

CINO_INLINE
int winding_number(const std::vector<vec3d> & verts,
                   const std::vector<uint>  & tris,
                   const vec3d              & p);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int winding_number(const AbstractPolygonMesh<M,V,E,P> & m,
                   const vec3d                        & p);
}

#ifndef  CINO_STATIC_LIB
#include "winding_number.cpp"
#endif

#endif // CINO_VERTEX_MASS_H
