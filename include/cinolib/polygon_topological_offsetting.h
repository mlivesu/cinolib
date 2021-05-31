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
#ifndef CINO_POLYGON_TOPOLOGICAL_OFFSETTING_H
#define CINO_POLYGON_TOPOLOGICAL_OFFSETTING_H

#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

/* Topological offsetting reads a simple polygon in the form of a circular chain
 * of vertices, and outputs a thickened polygon made of two nested simple curves
 * (the outer one being the input chain), with the space in between tessellated
 * with triangles.
 *
 * The number of vertices in the inner chain is typically bigger than the number
 * of vertices in the outer, and both chains are ordered starting from the same
 * position. The output vector vmap stores how many vertices in the inner chain
 * correspond to each vertex in the input outer chain.
 *
 * The algorithm works as follows:
 *
 *  - triangulate the input polygon
 *  - refine the so generated mesh such that each boundary vertex has at least three incident edges
 *  - split such edges to poisition vertices in the nested offsetted curve
 *
 * This guarantees that the nested chain of vertices is completely inside the input
 * shape, and that it still forms a simple polygon homotopic to the input one.
 *
 * TODO: the current version of the algorithm reads a 2D polygon made of 3D points,
 * The z coordinate is not used but it is assumed to exist just because I internally
 * construct a triangle mesh to easily make the refiement I need. This is all not
 * necessary, this whole part could be written (bit a bit more pain) without ever
 * constructing a mesh or asking input 3d points.
*/

CINO_INLINE
void polygon_topological_offsetting(std::vector<vec3d> & poly,
                                    std::vector<uint>  & tris,
                                    std::vector<uint>  & vmap);

}

#ifndef  CINO_STATIC_LIB
#include "polygon_topological_offsetting.cpp"
#endif

#endif // CINO_POLYGON_TOPOLOGICAL_OFFSETTING_H
