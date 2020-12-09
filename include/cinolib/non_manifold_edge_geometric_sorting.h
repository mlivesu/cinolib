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
#ifndef CINO_NON_MANIFOLD_EDGE_GEOMETRIC_SORTING_H
#define CINO_NON_MANIFOLD_EDGE_GEOMETRIC_SORTING_H

#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

/* Given an edge (e0,e1) and an unordered list of vertices that form triangles
 * with it, this method returns a circular ordering of the triangles around the edge.
 *
 * The ordering is purely geometric, and is based on the orient3d predicates contained
 * in cinolib/predicates.h
*/

CINO_INLINE
void non_manifold_edge_geometric_sorting(const vec3d              & e0,
                                         const vec3d              & e1,
                                         const std::vector<vec3d> & tris,
                                               std::vector<uint>  & ordering,
                                         const uint                 first_elem = 0);
}

#ifndef  CINO_STATIC_LIB
#include "non_manifold_edge_geometric_sorting.cpp"
#endif

#endif // CINO_NON_MANIFOLD_EDGE_GEOMETRIC_SORTING_H
