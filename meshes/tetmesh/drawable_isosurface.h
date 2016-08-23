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
#ifndef DRAWABLE_ISOSURFACE_H
#define DRAWABLE_ISOSURFACE_H

#ifdef __APPLE__
#include <gl.h>
#else
#include <GL/gl.h>
#endif

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/meshes/tetmesh/isosurface.h>

namespace cinolib
{

class DrawableIsosurface : public Isosurface, public DrawableObject
{
    public:

        DrawableIsosurface();
        DrawableIsosurface(const Tetmesh & m, const float iso_value);

        void   draw(const float scene_size=1) const;
        vec3d scene_center() const;
        float scene_radius() const;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_isosurface.cpp"
#endif

#endif // DRAWABLE_ISOSURFACE_H
