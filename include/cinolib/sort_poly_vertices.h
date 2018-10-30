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
#ifndef CINO_SORT_POLY_VERTICES_H
#define CINO_SORT_POLY_VERTICES_H

#include <cinolib/cino_inline.h>
#include <sys/types.h>
#include <cinolib/standard_elements_tables.h>
#include <vector>

namespace cinolib
{

/* These methods serve to move from a "general" polyhedral representation (i.e.
 * a list of faces plus the winding order associated to each face), to a more
 * "structured" representation often used to represent standard elements such
 * as hexahedra and tetrahedra, where polyhedra are represented as ordered lists
 * of vertices, and the order automatically defines the connectivity:
 *
 *               Reference               Reference
 *               Hexahedron:             Tetrahedron:
 *
 *                 v7------v6              v3
 *                / |     / |             /| \
 *              v4------v5  |           /  |   \
 *              |   |    |  |         v0---|----v2
 *              |  v3----|--v2         \   |   /
 *              | /      | /             \ | /
 *              v0------v1                 v1
 *
*/


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sort_poly_vertices_as_hexa(const std::vector<std::vector<uint>> & poly_faces,
                                const std::vector<bool>              & poly_winding,
                                      std::vector<uint>              & hexa);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sort_poly_vertices_as_hexa(const std::vector<std::vector<uint>> & faces,
                                const std::vector<std::vector<uint>> & polys,
                                const std::vector<std::vector<bool>> & winding,
                                      std::vector<std::vector<uint>> & hexas);
}

#ifndef  CINO_STATIC_LIB
#include "sort_poly_vertices.cpp"
#endif

#endif // CINO_SORT_POLY_VERTICES_H
