/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#ifndef CINO_SPLIT_SEPARATING_SIMPLICES_H
#define CINO_SPLIT_SEPARATING_SIMPLICES_H

#include <cinolib/meshes/trimesh.h>
#include <cinolib/meshes/tetmesh.h>

namespace cinolib
{

/* Separating simplices are non top simplicies that are internal but are entirely formed by
 * boundary vertices. This includes internal edges having both endpoints on the surface and
 * internal faces having all three corners on the surface.
 * These elements are called "separating" because break the connectedness of the mesh graph.
 * For example, an internal edge having both endpoints on the boundary is separating for a
 * triangle mesh because the removal of both vertices would inevitably split the mesh into two
 * connected components (hence the graph is not 2-connected). Similarly, an internal face with
 * all thee vertices on the surface makes the graph of a tetrahedral mesh non 3-connected because
 * the removal of all three vertices would split the mesh into two connected components.
 * These routines split all such elements in a simplicial complex of dimensions 2 or 3.
*/

template<class M, class V, class E, class P>
CINO_INLINE
uint split_separating_simplices(Trimesh<M,V,E,P> & m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint split_separating_simplices(Tetmesh<M,V,E,F,P> & m);
}

#ifndef  CINO_STATIC_LIB
#include "split_separating_simplices.cpp"
#endif

#endif // CINO_SPLIT_SEPARATING_SIMPLICES_H
