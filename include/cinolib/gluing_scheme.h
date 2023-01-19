/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
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
#ifndef CINO_GLUING_SCHEME_NORMAL_FORM_H
#define CINO_GLUING_SCHEME_NORMAL_FORM_H

#include <cinolib/homotopy_basis.h>


namespace cinolib
{

/* This function takes in input a mesh and a homotopy basis that yields a
 * canonical polygional schema, and modifies the loops to make sure that
 * the scheme is in normal form, that is, for a mesh with genus g loops
 * appear in the polygon sides in the following order
 *
 *     l_0, l_1, l_0*, l_1*, ..., l_2g-1, l_2g, l_2g-1*, l_2g*
 *
 * This is obtained by applying a sequence of cut and stich operations that
 * were first described in Section 17b of the book
 *
 *    Algebraic topology: a first course.
 *    W. Fulton, 1997
 *
 * This particular implementation is based on the algorithm described in the
 * paper
 *
 *    Towards a Robust and Portable Pipeline for Quad Meshing:
 *    Topological Initialization of Injective Integer Grid Maps
 *    Computers & Graphics, 2023
 *    M. Livesu
 *
 *  NOTE: the input homotopy basis is expected to be encoded in the mesh edges
 *  and vertices. Specifically, mesh edges and vertices are expected to be marked
 *  if they belong to a loop, and their label is expected to encode the loop unique
 *  id. This encoding is guaranteed correct if the homotopy basis was computed with
 *  the algorithm contained in cinolib and the flag "detach_loops" in the HomotopyBasisData
 *  was set to true.
*/
template<class M, class V, class E, class P>
CINO_INLINE
void gluing_scheme_force_normal_form(Trimesh<M,V,E,P>  & m,
                                     HomotopyBasisData & data);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// given a gluing scheme and a loop's index, states whether such loop
// belongs to string in normal form (A-B-A-B)
bool is_normal_form(const std::vector<int> & gs, const uint pos);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// radial loop sorting around the origin of the homotopy basis
// the full version lists ALL incident labels, interleaved with vertex ids
// the other version lists only loop labels
//
template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> radial_sorting(Trimesh<M,V,E,P>  & m,
                                HomotopyBasisData & data,
                                const bool full);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// gluing scheme (derived from radial sorting around the origin of the homotopy basis)
// the full version lists loops in the polygonal schema interleaved with free vertex ids (for tracing)
// the other version lists only loops in the polygonal schema
//
template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> gluing_scheme(Trimesh<M,V,E,P>  & m,
                               HomotopyBasisData & data,
                               const bool full);
}

#ifndef  CINO_STATIC_LIB
#include "gluing_scheme.cpp"
#endif

#endif // CINO_GLUING_SCHEME_NORMAL_FORM_H
