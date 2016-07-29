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
#ifndef DRAWABLE_ISOCONTOUR_H
#define DRAWABLE_ISOCONTOUR_H

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/isocontour.h>

namespace cinolib
{

class DrawableIsocontour : public Isocontour, public DrawableObject
{
    public:

        DrawableIsocontour();
        DrawableIsocontour(Trimesh & m_ptr, float iso_value);

        // Implement DrawableObject interface
        //
        void  draw()         const;
        vec3d scene_center() const { return vec3d(); }
        float scene_radius() const { return 0.0;     }

    private:

        float sample_rad;
        float cylind_rad;

        float sample_rgb[3];
        float centre_rgb[3];
        float cylind_rgb[3];
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_isocontour.cpp"
#endif

#endif // DRAWABLE_ISOCONTOUR_H
