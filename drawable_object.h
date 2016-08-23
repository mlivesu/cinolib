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
#ifndef DRAWABLE_OBJECT_H
#define DRAWABLE_OBJECT_H

#include <cinolib/geometry/vec3.h>
#include <float.h>

namespace cinolib
{

typedef enum
{
    ABSTRACT      ,
    TRIMESH       ,
    TETMESH       ,
    QUADMESH      ,
    HEXMESH       ,
    SKELETON      ,
    CURVE         ,
    ISOSURFACE    ,
    VECTOR_FIELD
}
ObjectType;

class DrawableObject
{
    public :

        ObjectType type;

        DrawableObject() { type = ABSTRACT; }

        virtual void  draw(const float scene_size = 1) const = 0;  // do rendering
        virtual vec3d scene_center() const = 0;  // get position in space
        virtual float scene_radius() const = 0;  // get size (approx. radius of the bounding sphere)
};

}

#endif // DRAWABLE_OBJECT_H
