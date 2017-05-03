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
#ifndef CINO_MESH_STD_DATA_H
#define CINO_MESH_STD_DATA_H

#include <cinolib/geometry/vec3.h>

namespace cinolib
{

/* These are the MINIMAL attributes required by CinoLib.
 * If omitted, the library would not compile!!!!!!!!!!!
 *
 * NOTE: you do not need to specify them, they have been
 * set as default template arguments in any mesh available
 * within the library. Meshes can therefore be declared as
 * follows:
 *
 * Trimesh<>     my_trimesh;
 * Quadmesh<>    my_quadmesh;
 * Polygonmesh<> my_polymesh;
 * Tetmesh<>     my_tetmesh;
 * Hexmesh<>     my_hexmesh;
 *
 * Otherwise, if more attributes are necessary, you can always
 * extend the current structures, and use explicit templates:
 *
 * Trimesh<Vdata,Edata,Fdata>     my_trimesh;
 * Quadmesh<Vdata,Edata,Fdata>    my_quadmesh;
 * Polygonmesh<Vdata,Edata,Fdata> my_polymesh;
 * Tetmesh<Vdata,Edata,Cdata>     my_tetmesh;
 * Hexmesh<Vdata,Edata,Cdata>     my_hexmesh;
*/

typedef struct
{
    vec3d  normal   = vec3d(0,0,0);
    float  color[3] = { 1, 1, 1 };
    float  uvw[3]   = { 0, 0, 0 };
}
Vert_std_data;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{}
Edge_std_data;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    vec3d  normal   = vec3d(0,0,0);
    float  color[3] = { 1, 1, 1 };
    int    label    = 0;
}
Face_std_data;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    float  color[3] = { 1, 1, 1 };
    int    label    = 0;
}
Cell_std_data;

}

#endif // CINO_MESH_STD_DATA_H
