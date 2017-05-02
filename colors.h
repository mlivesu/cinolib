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
#ifndef CINO_COLORS_H
#define CINO_COLORS_H

#include <cinolib/cinolib.h>

namespace cinolib
{

static const float RED     [3] = { 1.0 , 0.0 , 0.0 };
static const float GREEN   [3] = { 0.0 , 1.0 , 0.0 };
static const float BLUE    [3] = { 0.0 , 0.0 , 1.0 };
static const float YELLOW  [3] = { 1.0 , 1.0 , 0.0 };
static const float MAGENTA [3] = { 1.0 , 0.0 , 1.0 };
static const float CYAN    [3] = { 0.0 , 1.0 , 1.0 };
static const float BLACK   [3] = { 0.0 , 0.0 , 0.0 };
static const float WHITE   [3] = { 1.0 , 1.0 , 1.0 };

CINO_INLINE void scattered_color(int n_colors, int pos, float * rgb);

}

#ifndef  CINO_STATIC_LIB
#include "colors.cpp"
#endif

#endif // CINO_COLORS_H
