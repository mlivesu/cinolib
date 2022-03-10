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
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/gl/load_texture.h>
#include <cinolib/textures/texture_hsv.h>
#include <cinolib/textures/texture_hsv_w_isolines.h>
#include <cinolib/textures/texture_parula.h>
#include <cinolib/textures/texture_parula_w_isolines.h>
#include <cinolib/serialize_index.h>
#include <stdint.h>
#include <iterator>

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"

// this is the only symbol not in OpenGL 1.1 in the whole library.
// useful to define it when compiling CinoLib in Windows without
// Glad, Glew or similar tools
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_isolines2D(      Texture & texture,
                             const Color   & u_isolines,
                             const Color   & v_isolines,
                             const Color   & background)
{
    if(texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    const uint size = 8;
    uint8_t * data = new uint8_t[size*size*4];
    for(uint r=0; r<size; ++r)
    for(uint c=0; c<size; ++c)
    {
        uint i = 4 * serialize_2D_index(r,c,size);
        data[i  ] = background.r_uchar();
        data[i+1] = background.g_uchar();
        data[i+2] = background.b_uchar();
        data[i+3] = background.a_uchar();
    }
    for(uint c=0; c<size; ++c)
    {
        uint i = 4 * serialize_2D_index(0,c,size);
        data[i  ] = u_isolines.r_uchar();
        data[i+1] = u_isolines.g_uchar();
        data[i+2] = u_isolines.b_uchar();
        data[i+3] = u_isolines.a_uchar();
    }
    for(uint r=0; r<size; ++r)
    {
        uint i = 4 * serialize_2D_index(r,0,size);
        data[i  ] = v_isolines.r_uchar();
        data[i+1] = v_isolines.g_uchar();
        data[i+2] = v_isolines.b_uchar();
        data[i+3] = v_isolines.a_uchar();
    }

    load_texture_2D(texture.id, data, size, size, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
    delete[] data;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_checkerboard(      Texture & texture,
                               const Color   & c0,
                               const Color   & c1)
{
    if(texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    const uint size = 8;
    uint8_t * data = new uint8_t[size*size*4];
    for(uint r=0; r<size; ++r)
    for(uint c=0; c<size; ++c)
    {
        uint i = 4 * serialize_2D_index(r,c,size);
        if(r%2 == c%2)
        {
            data[i  ] = c0.r_uchar();
            data[i+1] = c0.g_uchar();
            data[i+2] = c0.b_uchar();
            data[i+3] = c0.a_uchar();
        }
        else
        {
            data[i  ] = c1.r_uchar();
            data[i+1] = c1.g_uchar();
            data[i+2] = c1.b_uchar();
            data[i+3] = c1.a_uchar();
        }
    }

    load_texture_2D(texture.id, data, size, size, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
    delete[] data;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_isolines1D(      Texture & texture,
                             const Color   & c0,
                             const Color   & c1,
                             const int       n_bands)
{
    if(texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    uint8_t * data = new uint8_t[3*n_bands];
    for(int i=0; i<n_bands; ++i)
    {
        if(i%2)
        {
            data[3*i  ] = c0.r_uchar();
            data[3*i+1] = c0.g_uchar();
            data[3*i+2] = c0.b_uchar();
        }
        else
        {
            data[3*i  ] = c1.r_uchar();
            data[3*i+1] = c1.g_uchar();
            data[3*i+2] = c1.b_uchar();
        }
    }

    load_texture_1D(texture.id, data, n_bands, GL_RGB, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
    delete[] data;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_HSV(Texture & texture)
{
    if(texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);
    load_texture_1D(texture.id, hsv_texture1D, 256, GL_RGB, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_HSV_with_isolines(Texture & texture)
{
    if(texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);
    load_texture_1D(texture.id, hsv_texture1D_with_isolines, 256, GL_RGB, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_parula(Texture & texture)
{
    if(texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);
    load_texture_1D(texture.id, parula_texture1D, 64, GL_RGB, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_parula_with_isolines(Texture & texture)
{
    if(texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);
    load_texture_1D(texture.id, parula_w_isolines_texture1D, 80, GL_RGB, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_bitmap(Texture & texture, const char *bitmap)
{
    if(texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);
    int w,h,n;
    stbi_set_flip_vertically_on_load(true); // avoids vertical mirroring of the texture
    uint8_t * data = stbi_load(bitmap, &w, &h, &n, STBI_rgb_alpha); // it always returns an RGBA image, regardless of the actual file encoding
    if(data==nullptr)
    {
        std::cerr << "ERROR: could not load texture " << bitmap << std::endl;
    }
    else
    {
        load_texture_2D(texture.id, data, w, h, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
        stbi_image_free(data);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_1D(const GLuint    id,
                     const uint8_t * data,
                     const GLsizei   size,
                     const GLint     format,     // GL_RGB,     GL_RGBA, ...
                     const GLint     wrap,       // GL_REPEAT,  GL_CLAMP
                     const GLint     mag_filter, // GL_NEAREST, GL_LINEAR, ...
                     const GLint     min_filter) // GL_NEAREST, GL_LINEAR, ...
{
    glBindTexture(GL_TEXTURE_1D, id);
    glTexImage1D(GL_TEXTURE_1D, 0, format, size, 0, (GLenum)format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S    , wrap);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glBindTexture(GL_TEXTURE_1D, 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture_2D(const GLuint    id,
                     const uint8_t * data,
                     const GLsizei   width,
                     const GLsizei   height,
                     const GLint     format,     // GL_RGB,     GL_RGBA, ...
                     const GLint     wrap_s,     // GL_REPEAT,  GL_CLAMP
                     const GLint     wrap_t,     // GL_REPEAT,  GL_CLAMP
                     const GLint     mag_filter, // GL_NEAREST, GL_LINEAR, ...
                     const GLint     min_filter) // GL_NEAREST, GL_LINEAR, ...
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, (GLenum)format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glBindTexture(GL_TEXTURE_2D, 0);
}

}
