/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/colors.h>
#include <cinolib/textures/quality_ramp_texture.h>

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
    int   prev  = std::max((int)floor(fpos),0); // just to avoid "index out of bounds" errors...
    int   next  = std::min(prev + 1, 255);      // just to avoid "index out of bounds" errors...

    int r = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 0] + alpha * quality_ramp_texture1D[next*3 + 0];
    int g = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 1] + alpha * quality_ramp_texture1D[next*3 + 1];
    int b = (1.0 - alpha) * quality_ramp_texture1D[prev*3 + 2] + alpha * quality_ramp_texture1D[next*3 + 2];

    rgb[0] = float(r)/255.0;
    rgb[1] = float(g)/255.0;
    rgb[2] = float(b)/255.0;
}

}
