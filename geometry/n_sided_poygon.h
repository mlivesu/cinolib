/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
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
#ifndef CINO_N_SIDED_POLYGON_H
#define CINO_N_SIDED_POLYGON_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/trimesh/trimesh.h>

namespace cinolib
{

CINO_INLINE
Trimesh n_sided_polygon(const vec3d  & center,
                        const int      n_sides,
                        const double   radius,
                        const vec3d  & n = vec3d(0,1,0));

}

#ifndef  CINO_STATIC_LIB
#include "n_sided_poygon.cpp"
#endif

#endif // CINO_N_SIDED_POLYGON_H
