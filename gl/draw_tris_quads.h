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
#ifndef DRAW_TRIS_QUADS_H
#define DRAW_TRIS_QUADS_H

#include "../cinolib.h"

#ifdef __APPLE__
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <cmath>

namespace cinolib
{

typedef enum
{
    DRAW_MESH        = 0x00000001,
    DRAW_POINTS      = 0x00000002,
    DRAW_FLAT        = 0x00000004,
    DRAW_SMOOTH      = 0x00000008,
    DRAW_WIREFRAME   = 0x00000010,
    DRAW_FACECOLOR   = 0x00000020,
    DRAW_VERTEXCOLOR = 0x00000040,
    DRAW_TEXTURE1D   = 0x00000080,
    DRAW_BORDER      = 0x00000100,
}
DrawMode;

typedef struct
{
          int                  draw_mode;
          GLenum               face_type;
    const std::vector<double> *coords           = NULL;
    const std::vector<uint>   *faces            = NULL;
    const std::vector<double> *v_norms          = NULL;
    const std::vector<float>  *v_colors         = NULL;
    const std::vector<float>  *f_colors         = NULL;
    const std::vector<float>  *text1D           = NULL;
    const std::vector<double> *border_coords    = NULL;
    const std::vector<uint>   *border_segs      = NULL;
    const float               *wireframe_color  = NULL;
    const float               *border_color     = NULL;
          int                  wireframe_width;
          int                  border_width;
}
RenderFaceData;


CINO_INLINE
void render_faces(const RenderFaceData & data);
}

#ifndef  CINO_STATIC_LIB
#include "draw_tris_quads.cpp"
#endif

#endif // DRAW_TRIS_QUADS_H
