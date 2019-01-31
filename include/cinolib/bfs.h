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

// floodfill only the surface of a polyhedral mesh (with vertex barriers).
//
template<class M, class V, class E, class F, class P>
CINO_INLINE
void bfs_srf_only(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                  const uint                                source,
                  const std::vector<bool>                 & mask, // if mask[vid] = true, path cannot pass through vertex vid
                  std::unordered_set<uint>                & visited);

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
                                 const std::vector<bool>            & mask_edges, // if mask[e] = true, bfs cannot expand through edge e
                                 std::unordered_set<uint>           & visited);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// floodfill (with barriers on faces)
//
template<class M, class V, class E, class F, class P>
CINO_INLINE
void bfs_on_dual_w_face_barriers(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                                 const uint                                source,
                                 const std::vector<bool>                 & mask_faces, // if mask[f] = true, bfs cannot expand through face f
                                 std::unordered_set<uint>                & visited);
}

#ifndef  CINO_STATIC_LIB
#include "bfs.cpp"
#endif

#endif // CINO_BFS_H
