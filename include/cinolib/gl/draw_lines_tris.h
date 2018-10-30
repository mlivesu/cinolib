/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_DRAW_LINES_TRIS_H
#define CINO_DRAW_LINES_TRIS_H

#ifdef CINOLIB_USES_OPENGL

#ifdef __APPLE__
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <vector>
#include <cmath>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/color.h>
#include <cinolib/textures/textures.h>

namespace cinolib
{

enum
{
    DRAW_TRIS                 = 0x00000001,
    DRAW_TRI_POINTS           = 0x00000002,
    DRAW_TRI_FLAT             = 0x00000004,
    DRAW_TRI_SMOOTH           = 0x00000008,
    DRAW_TRI_FACECOLOR        = 0x00000010,
    DRAW_TRI_VERTCOLOR        = 0x00000020,
    DRAW_TRI_QUALITY          = 0x00000040,
    DRAW_TRI_TEXTURE1D        = 0x00000080,
    DRAW_TRI_TEXTURE2D        = 0x00000100,
    DRAW_SEGS                 = 0x00000200,
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    int                draw_mode;
    //
    std::vector<uint>  tris;
    std::vector<float> tri_coords;
    std::vector<float> tri_v_norms;
    std::vector<float> tri_v_colors; // rgba
    std::vector<float> tri_text;
    Texture            texture;
    //
    std::vector<uint>  segs;
    std::vector<float> seg_coords;
    std::vector<float> seg_colors; // rgba
    GLfloat            seg_width = 1;
    //
}
RenderData;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void render(const RenderData & data);

}

#ifndef  CINO_STATIC_LIB
#include "draw_lines_tris.cpp"
#endif

#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_DRAW_LINES_TRIS_H
