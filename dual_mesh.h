/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#ifndef CINO_DUAL_MESH
#define CINO_DUAL_MESH

#include <cinolib/cinolib.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/meshes/polygonmesh/polygonmesh.h>

namespace cinolib
{

CINO_INLINE
void dual_mesh(const Trimesh                        & primal,
                     std::vector<vec3d>             & dual_verts,
                     std::vector<std::vector<uint>> & dual_faces,
               const bool                             with_clipped_cells); // consider/discard boundary vertices


CINO_INLINE
void dual_mesh(const Trimesh       & primal,
                     Polygonmesh<> & dual,
               const bool            with_clipped_cells); // consider/discard boundary vertices

}

#ifndef  CINO_STATIC_LIB
#include "dual_mesh.cpp"
#endif

#endif // CINO_DUAL_MESH
