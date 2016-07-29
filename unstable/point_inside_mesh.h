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
#ifndef POINT_INSIDE_MESH_H
#define POINT_INSIDE_MESH_H

#include <cinolib/cinolib.h>
#include <cinolib/octree.h>

namespace cinolib
{

template<class Mesh>
class PointInsideMeshCache
{
    private:

        Mesh        m;
        Octree<int> octree;

    public:

        PointInsideMeshCache(const Mesh & m);

        void  locate(const vec3d p, int & tid, double wgts[4]) const;
        vec3d locate(const vec3d p, const Mesh & obj) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "point_inside_mesh.cpp"
#endif

#endif // POINT_INSIDE_MESH_H
