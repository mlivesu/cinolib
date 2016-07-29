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
#ifndef DRAW_ARROW_H
#define DRAW_ARROW_H

#include <cinolib/cinolib.h>
#include <cinolib/gl/draw_cylinder.h>

namespace cinolib
{

template <typename vec3>
CINO_INLINE
void arrow(const vec3  & base,
           const vec3  & tip,
           float         radius,
           const float * color)
{
    vec3 tip_base = 0.3 * tip + 0.7 * base;

    cylinder<vec3>(tip_base, tip, radius, 0.0, color);
    cylinder<vec3>(base, tip_base, radius * 0.2,  radius * 0.2, color);
}

}

#endif // DRAW_ARROW_H
