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
