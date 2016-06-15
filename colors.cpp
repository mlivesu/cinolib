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
#include "colors.h"
#include "textures/quality_ramp_texture.h"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>

namespace cinolib
{

CINO_INLINE void scattered_color(int n_colors, int pos, float * rgb)
{
    float fpos  = float(pos)/float(n_colors - 1) * 255.0;
    float alpha = fpos - floor(fpos);
    int   prev  = (int)floor(fpos);
    int   next  = std::min(prev + 1, 255); // just to avoid "index out of bounds" errors...

    int r = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 0] + alpha * quality_ramp_texture1D[next*3 + 0];
    int g = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 1] + alpha * quality_ramp_texture1D[next*3 + 1];
    int b = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 2] + alpha * quality_ramp_texture1D[next*3 + 2];

    rgb[0] = float(r)/255.0;
    rgb[1] = float(g)/255.0;
    rgb[2] = float(b)/255.0;
}

}
