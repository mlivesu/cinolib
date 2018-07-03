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

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/meshes.h>
#include <unordered_set>

namespace cinolib
{

// floodfill (for general graphs - i.e. not meshes)
//
CINO_INLINE
void bfs(const std::vector<std::vector<uint>> & nodes_adjacency,
         const uint                             source,
               std::unordered_set<uint>       & visited);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// floodfill
//
template<class M, class V, class E, class P>
CINO_INLINE
void bfs(const AbstractMesh<M,V,E,P>    & m,
         const uint                       source,
               std::unordered_set<uint> & visited);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// floodfill (with vertex barriers)
//
template<class M, class V, class E, class P>
CINO_INLINE
void bfs(const AbstractMesh<M,V,E,P>    & m,
         const uint                       source,
         const std::vector<bool>        & mask, // if mask[vid] = true, path cannot pass through vertex vid
               std::unordered_set<uint> & visited);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// floodfill (with barriers) on the dual mesh (polygons/polyhedra instead of vertices)
//
template<class M, class V, class E, class P>
CINO_INLINE
void bfs_on_dual(const AbstractMesh<M,V,E,P>    & m,
                 const uint                       source,
                 const std::vector<bool>        & mask, // if mask[p] = true, bfs cannot visit polygon/polyhedron p
                       std::unordered_set<uint> & visited);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// floodfill (with barriers on edges)
//
template<class M, class V, class E, class P>
CINO_INLINE
void bfs_on_dual_w_edge_barriers(const AbstractPolygonMesh<M,V,E,P> & m,
                                 const uint                           source,
                                 const std::vector<bool>            & mask_edges, // if mask[e] = true, bfs canno expand through edge e
                                 std::unordered_set<uint>           & visited);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// floodfill (with barriers on faces)
//
template<class M, class V, class E, class F, class P>
CINO_INLINE
void bfs_on_dual_w_face_barriers(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                                 const uint                                source,
                                 const std::vector<bool>                 & mask_faces, // if mask[f] = true, bfs canno expand through face f
                                 std::unordered_set<uint>                & visited);
}

#ifndef  CINO_STATIC_LIB
#include "bfs.cpp"
#endif

#endif // CINO_BFS_H
