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
#ifndef CINO_CANONICAL_POLYGONAL_SCHEMA_H
#define CINO_CANONICAL_POLYGONAL_SCHEMA_H

#include <cinolib/meshes/trimesh.h>
#include <cinolib/homotopy_basis.h>

namespace cinolib
{

/* Given a mesh with genus g and its homotopy basis, it realizes a
 * canonical polygonal schema, which consists in cutting the mesh
 * along the basis loops, and flatten it to the plane in a regular
 * polygon with 4g sides. Interior vertices are mapped with Tutte.
 *
 * For further details, refer to:
 *
 *    Obtaining a Canonical Polygonal Schema from a
 *    Greedy Homotopy Basis with Minimal Mesh Refinement
 *    Marco Livesu
 *    (submitted)
 *
 * and
 *
 *    Globally Optimal Surface Mapping for Surfaces with Arbitrary Topology
 *    Xin Li, Yunfan Bao, Xiaohu Guo, Miao Jin, Xianfeng Gu and Hong Qin
 *    IEEE Transactions on Visualization and Computer Graphics, 2008
*/

template<class M, class V, class E, class P>
CINO_INLINE
void canonical_polygonal_schema(      Trimesh<M,V,E,P>  & m_in,
                                const HomotopyBasisData & basis,
                                      Trimesh<M,V,E,P>  & m_out,
                                const int                 laplacian_mode = COTANGENT);
}

#ifndef  CINO_STATIC_LIB
#include "canonical_polygonal_schema.cpp"
#endif

#endif // CINO_CANONICAL_POLYGONAL_SCHEMA_H
