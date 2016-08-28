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
#include "drawable_curve.h"

namespace cinolib
{

CINO_INLINE
DrawableCurve::DrawableCurve() : Curve()
{}

CINO_INLINE
DrawableCurve::DrawableCurve(const std::vector<vec3d> & samples) : Curve(samples)
{}


CINO_INLINE
void DrawableCurve::draw(const float scene_size) const
{
    double cylind_rad = scene_size*0.001;

    for(size_t i=1; i<samples().size(); ++i)
    {
        cylinder<vec3d>(samples()[i-1].pos, samples()[i].pos, cylind_rad, cylind_rad, RED);
    }
}


CINO_INLINE
vec3d DrawableCurve::scene_center() const
{
    return bbox().center();
}


CINO_INLINE
float DrawableCurve::scene_radius() const
{
    return bbox().diag();
}



}
