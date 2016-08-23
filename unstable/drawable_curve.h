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
#ifndef DRAWABLE_CURVE_H
#define DRAWABLE_CURVE_H

#include <cinolib/cinolib.h>
#include <cinolib/drawable_object.h>
#include <cinolib/unstable/curve.h>
#include <cinolib/gl/draw_cylinder.h>
#include <cinolib/colors.h>

namespace cinolib
{

class DrawableCurve : public Curve, public DrawableObject
{
    public:

        DrawableCurve();
        DrawableCurve(const std::vector<vec3d> & samples);

        void  draw(const float scene_size=1) const;
        vec3d scene_center() const;
        float scene_radius() const;
};

}

#ifndef  CINO_STATIC_LIB
#include "drawable_curve.cpp"
#endif

#endif // DRAWABLE_CURVE_H
