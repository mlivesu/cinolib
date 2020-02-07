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
#ifndef CINO_DIJKSTRA_H
#define CINO_DIJKSTRA_H

#include <set>
#include <sys/types.h>
#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/abstract_mesh.h>
#include <cinolib/meshes/abstract_polyhedralmesh.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::: DIJKSTRAs ON PRIMAL GRAPH (VERTICES) ::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive(const AbstractMesh<M,V,E,P> & m,
                         const uint                    source,
                               std::vector<double>   & dist);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive(const AbstractMesh<M,V,E,P> & m,
                         const std::vector<uint>     & sources,
                               std::vector<double>   & dist);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void dijkstra_exhaustive_srf_only(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                                  const std::vector<uint>                 & sources,
                                        std::vector<double>               & dist); // unreached verts will have inf_double distance

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra(const AbstractMesh<M,V,E,P> & m,
                const uint                    source,
                const uint                    dest,
                      std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra(const AbstractMesh<M,V,E,P> & m,
                const uint                    source,
                const uint                    dest,
                const std::vector<bool>     & mask, // if mask[v] = true, path cannot pass through it
                      std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra(const AbstractMesh<M,V,E,P> & m,
                const uint                    source,
                const std::set<uint>        & dest,
                const std::vector<bool>     & mask, // if mask[v] = true, path cannot pass through it
                      std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_mask_on_edges(const AbstractMesh<M,V,E,P> & m,
                              const uint                    source,
                              const uint                    dest,
                              const std::vector<bool>     & mask, // if mask[e] = true, path cannot pass through edge e
                                    std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::: DIJKSTRAs ON DUAL GRAPH (POLYGONS/POLYHEDRA) :::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive_on_dual(const AbstractMesh<M,V,E,P> & m,
                                 const uint                    source,
                                       std::vector<double>   & dist);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive_on_dual(const AbstractMesh<M,V,E,P> & m,
                                 const std::vector<uint>     & sources,
                                       std::vector<double>   & dist);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                        const uint                    source,
                        const uint                    dest,
                              std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                        const uint                    source,
                        const uint                    dest,
                        const std::vector<bool>     & mask, // if mask[p] = true, path cannot pass through it
                              std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                        const uint                    source,
                        const std::set<uint>        & dest,
                        const std::vector<bool>     & mask, // if mask[p] = true, path cannot pass through it
                              std::vector<uint>     & path);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                        const uint                    source,
                        const std::set<uint>        & dest,
                              std::vector<uint>     & path);

}

#ifndef  CINO_STATIC_LIB
#include "dijkstra.cpp"
#endif

#endif // CINO_DIJKSTRA_H
