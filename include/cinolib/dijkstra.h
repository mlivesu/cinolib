/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#ifndef CINO_DIJKSTRA_H
#define CINO_DIJKSTRA_H

#include <set>
#include <cinolib/cinolib.h>
#include <cinolib/meshes/abstract_mesh.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra(const AbstractMesh<M,V,E,P> & m,
              const uint                    source,
              const uint                    dest,
                    std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Shortest path (with barriers). The path cannot
// pass throuh vertices for which mask[v] = true
//
template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra(const AbstractMesh<M,V,E,P> & m,
              const uint                    source,
              const uint                    dest,
              const std::vector<bool>     & mask,
                    std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Shortest path (with barriers and multiple destinations). The path
// cannot pass throuh vertices for which mask[v] = true. The algorithm
// stops as soon as it reaches one of the destinations
//
template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra(const AbstractMesh<M,V,E,P> & m,
              const uint                    source,
              const std::set<uint>        & dest,
              const std::vector<bool>     & mask,
                    std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// works on dual mesh: polygons/polyhedra rather then vertices
//
template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                      const uint                    source,
                      const uint                    dest,
                            std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// works on dual mesh: polygons/polyhedra rather then vertices
//
template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                      const uint                    source,
                      const uint                    dest,
                      const std::vector<bool>     & mask,
                            std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// works on dual mesh: polygons/polyhedra rather then vertices
//
template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                      const uint                    source,
                      const std::set<uint>        & dest,
                      const std::vector<bool>     & mask,
                            std::vector<uint>     & path);

}

#ifndef  CINO_STATIC_LIB
#include "dijkstra.cpp"
#endif

#endif // CINO_DIJKSTRA_H
