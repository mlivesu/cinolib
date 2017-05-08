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
#ifndef CINO_DRAW_LINES_TRIS_H
#define CINO_DRAW_LINES_TRIS_H

#include <cinolib/cinolib.h>

#ifdef __APPLE__
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <sys/types.h>
#include <cmath>

namespace cinolib
{

enum
{
    DRAW_TRIS            = 0x00000001,
    DRAW_TRI_POINTS      = 0x00000002,
    DRAW_TRI_FLAT        = 0x00000004,
    DRAW_TRI_SMOOTH      = 0x00000008,
    DRAW_TRI_FACECOLOR   = 0x00000020,
    DRAW_TRI_VERTEXCOLOR = 0x00000040,
    DRAW_TRI_TEXTURE1D   = 0x00000080,
    DRAW_SEGS            = 0x00000100,
    DRAW_SEG_SEGCOLOR    = 0x00000200,
    DRAW_SEG_VERTCOLOR   = 0x00000400,
};

typedef struct
{
    int                 draw_mode;
    //
    std::vector<uint>   tris;
    std::vector<double> tri_coords;
    std::vector<double> tri_v_norms;
    std::vector<float>  tri_v_colors; // rgba
    std::vector<float>  tri_text1D;
    GLuint              tri_text1D_id;
    //
    std::vector<uint>   segs;
    std::vector<double> seg_coords;
    float               seg_color[4];
    int                 seg_width;
}
RenderData;


CINO_INLINE
void render(const RenderData & data);
}

#ifndef  CINO_STATIC_LIB
#include "draw_lines_tris.cpp"
#endif

#endif // CINO_DRAW_LINES_TRIS_H
