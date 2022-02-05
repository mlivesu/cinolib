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
*                                                                               *
*     Fabrizio Corda (cordafab@gmail.com)                                       *
*     http://www.fabriziocorda.com                                              *
*                                                                               *
*     University of Cagliari                                                    *
*     Via Ospedale, 72                                                          *
*     09124 Cagliari,                                                           *
*     Italy                                                                     *                                                                               *
*********************************************************************************/
#ifndef CINO_LOAD_TEXTURE_H
#define CINO_LOAD_TEXTURE_H

#ifdef CINOLIB_USES_OPENGL_GLFW_IMGUI

#include <cinolib/gl/gl_glfw.h>
#include <cinolib/color.h>

namespace cinolib
{

enum // texture types currently supported
{
    TEXTURE_1D_ISOLINES            = 0,
    TEXTURE_1D_HSV                 = 1,
    TEXTURE_1D_HSV_W_ISOLINES      = 2,
    TEXTURE_1D_PARULA              = 3,
    TEXTURE_1D_PARULA_W_ISOLINES   = 4,
    TEXTURE_2D_ISOLINES            = 0,
    TEXTURE_2D_CHECKERBOARD        = 1,
    TEXTURE_2D_BITMAP              = 2
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct Texture
{
    int    type;
    GLuint id             = 0;
    float  scaling_factor = 1.0;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_1D(const GLuint    id,
                     const uint8_t * data,
                     const GLsizei   size,
                     const GLint     format,      // GL_RGB,     GL_RGBA, ...
                     const GLint     wrap,        // GL_REPEAT,  GL_CLAMP
                     const GLint     mag_filter,  // GL_NEAREST, GL_LINEAR, ...
                     const GLint     min_filter); // GL_NEAREST, GL_LINEAR, ...

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_2D(const GLuint    id,
                     const uint8_t * data,
                     const GLsizei   width,
                     const GLsizei   height,
                     const GLint     format,      // GL_RGB,     GL_RGBA, ...
                     const GLint     wrap_s,      // GL_REPEAT,  GL_CLAMP
                     const GLint     wrap_t,      // GL_REPEAT,  GL_CLAMP
                     const GLint     mag_filter,  // GL_NEAREST, GL_LINEAR, ...
                     const GLint     min_filter); // GL_NEAREST, GL_LINEAR, ...

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_HSV(Texture & texture);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_HSV_with_isolines(Texture & texture);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_parula(Texture & texture);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_parula_with_isolines(Texture & texture);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_isolines1D(      Texture & texture,
                             const Color   & c0 = Color::BLACK(),
                             const Color   & c1 = Color::WHITE(),
                             const int       n_bands = 32);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_checkerboard(      Texture & texture,
                               const Color   & c0 = Color(.5,.5,.5),
                               const Color   & c1 = Color::WHITE());

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_isolines2D(      Texture & texture,
                             const Color   & u_isolines = Color::BLUE(),
                             const Color   & v_isolines = Color::RED(),
                             const Color   & background = Color::WHITE());

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_bitmap(Texture & texture, const char *bitmap);

}

#ifndef  CINO_STATIC_LIB
#include "load_texture.cpp"
#endif

#endif // CINOLIB_USES_OPENGL_GLFW_IMGUI
#endif // CINO_LOAD_TEXTURE_H
