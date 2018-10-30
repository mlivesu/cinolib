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
#ifndef CINO_COARSE_LAYOUT_H
#define CINO_COARSE_LAYOUT_H

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/quadmesh.h>
#include <cinolib/meshes/hexmesh.h>

/* Compute coarse layouts for structured surface (quad)
 * and volume (hexa) meshes. The layout will be encoded
 * in the labels of each element.
 *
 * One label per domain will be generated, and all the
 * mesh elements will receive a label corresponding to
 * the domain they blong to.
 *
 * Interfaces between adjacent domains will also be marked.
 * For surfaces, boundary edges and vertices will be marked.
 * Similarly, for volumes boundary edges, vertices and faces
 * will be marked.
*/

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void compute_coarse_quad_layout(Quadmesh<M,V,E,P> & m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void compute_coarse_hex_layout(Hexmesh<M,V,E,P> & m);

}

#ifndef  CINO_STATIC_LIB
#include "coarse_layout.cpp"
#endif

#endif // CINO_COARSE_LAYOUT_H
