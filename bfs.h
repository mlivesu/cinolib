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
#ifndef CINO_BFS_H
#define CINO_BFS_H

#include <cinolib/cinolib.h>
#include <cinolib/common.h>
#include <set>

namespace cinolib
{

// floodfill (for general graphs - i.e. not meshes)
//
CINO_INLINE
void bfs_exahustive(const std::vector<std::vector<uint>> & nodes_adjacency,
                    const uint                             source,
                          std::set<uint>                 & visited);


// floodfill
//
template<class Mesh>
CINO_INLINE
void bfs_exahustive(const Mesh          & m,
                    const int             source,
                          std::set<int> & visited);


// floodfill (with barriers) on the dual mesh (faces instead of vertices)
// The path cannot pass through faces for which mask[f] = true
//
template<class Mesh>
CINO_INLINE
void bfs_exahustive_on_dual(const Mesh              & m,
                            const int                 source,
                            const std::vector<bool> & mask,
                                  std::set<int>     & visited);


// shortest path on unweighted graph, essentially dijkstra with constaint weights.
//
template<class Mesh>
CINO_INLINE
void bfs(const Mesh             & m,
         const int                source,
         const int                dest,
               std::vector<int> & path);


// shortest path (with barriers) on unweighted graph.
// The path cannot pass throuh vertices for which mask[v] = true
//
template<class Mesh>
CINO_INLINE
void bfs(const Mesh              & m,
         const int                 source,
         const int                 dest,
         const std::vector<bool> & mask,
               std::vector<int>  & path);


// shortest path (with barriers and multiple destinatins) on unweighted graph.
// The path cannot pass throuh vertices for which mask[v] = true
// The algorithm stops as soon as it reaches of of the destinations
//
template<class Mesh>
CINO_INLINE
void bfs(const Mesh              & m,
         const int                 source,
         const std::set<int>     & dest,
         const std::vector<bool> & mask,
               std::vector<int>  & path);
}


#ifndef  CINO_STATIC_LIB
#include "bfs.cpp"
#endif

#endif // CINO_BFS_H
