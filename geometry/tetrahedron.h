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
#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include "../cinolib.h"
#include "../geometry/vec3.h"

namespace cinolib
{

static int TET_FACES[4][3] =
{
    { 0, 2, 1 } ,
    { 0, 1, 3 } ,
    { 0, 3, 2 } ,
    { 1, 2, 3 }
};

static int TET_EDGES[6][2] =
{
    { 0, 2 }, // 0
    { 2, 1 }, // 1
    { 1, 0 }, // 2
    { 1, 3 }, // 3
    { 3, 0 }, // 4
    { 3, 2 }  // 5
};


CINO_INLINE
void tet_closest_vertex(const vec3d  & A,
                        const vec3d  & B,
                        const vec3d  & C,
                        const vec3d  & D,
                        const vec3d  & query,
                              int    & id,
                              double & dist);


CINO_INLINE
void tet_closest_edge(const vec3d  & A,
                      const vec3d  & B,
                      const vec3d  & C,
                      const vec3d  & D,
                      const vec3d  & query,
                            int    & id,
                            double & dist);
}

#ifndef  CINO_STATIC_LIB
#include "tetrahedron.cpp"
#endif

#endif // TETRAHEDRON_H
