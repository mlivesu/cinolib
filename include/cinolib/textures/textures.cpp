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
#include <cinolib/textures/textures.h>
#include <cinolib/textures/texture_hsv.h>
#include <cinolib/textures/texture_hsv_w_isolines.h>
#include <cinolib/textures/texture_parula.h>
#include <cinolib/textures/texture_parula_w_isolines.h>
#include <cinolib/serialize_index.h>
#include <stdint.h>
#include <iterator>

#ifdef CINOLIB_USES_QT
#include <QImage>
#include <QGLWidget>
#endif

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_isolines2D(      Texture & texture,
                        const Color   & u_isolines,
                        const Color   & v_isolines,
                        const Color   & background)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = 8;
    texture.data = new uint8_t[texture.size*texture.size*4];
    for(int32_t r=0; r<texture.size; ++r)
    for(int32_t c=0; c<texture.size; ++c)
    {
        uint i = 4 * serialize_2D_index(r,c,texture.size);
        texture.data[i  ] = background.r_uchar();
        texture.data[i+1] = background.g_uchar();
        texture.data[i+2] = background.b_uchar();
        texture.data[i+3] = background.a_uchar();
    }
    for(int32_t c=0; c<texture.size; ++c)
    {
        uint i = 4 * serialize_2D_index(0,c,texture.size);
        texture.data[i  ] = u_isolines.r_uchar();
        texture.data[i+1] = u_isolines.g_uchar();
        texture.data[i+2] = u_isolines.b_uchar();
        texture.data[i+3] = u_isolines.a_uchar();
    }
    for(int32_t r=0; r<texture.size; ++r)
    {
        uint i = 4 * serialize_2D_index(r,0,texture.size);
        texture.data[i  ] = v_isolines.r_uchar();
        texture.data[i+1] = v_isolines.g_uchar();
        texture.data[i+2] = v_isolines.b_uchar();
        texture.data[i+3] = v_isolines.a_uchar();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_checkerboard(      Texture & texture,
                          const Color   & c0,
                          const Color   & c1)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = 8;
    texture.data = new uint8_t[texture.size*texture.size*4];
    for(int32_t r=0; r<texture.size; ++r)
    for(int32_t c=0; c<texture.size; ++c)
    {
        uint i = 4 * serialize_2D_index(r,c,texture.size);
        if (r%2 == c%2)
        {
            texture.data[i  ] = c0.r_uchar();
            texture.data[i+1] = c0.g_uchar();
            texture.data[i+2] = c0.b_uchar();
            texture.data[i+3] = c0.a_uchar();
        }
        else
        {
            texture.data[i  ] = c1.r_uchar();
            texture.data[i+1] = c1.g_uchar();
            texture.data[i+2] = c1.b_uchar();
            texture.data[i+3] = c1.a_uchar();
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_isolines1D(      Texture & texture,
                        const Color   & c0,
                        const Color   & c1,
                        const uint     n_bands)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = n_bands;
    texture.data = new uint8_t[3*texture.size];
    for(int32_t i=0; i<texture.size; ++i)
    {
        if (i%2)
        {
            texture.data[3*i  ] = c0.r_uchar();
            texture.data[3*i+1] = c0.g_uchar();
            texture.data[3*i+2] = c0.b_uchar();
        }
        else
        {
            texture.data[3*i  ] = c1.r_uchar();
            texture.data[3*i+1] = c1.g_uchar();
            texture.data[3*i+2] = c1.b_uchar();
        }
    };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_HSV(Texture & texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = 256;
    texture.data = new uint8_t[768];
    std::copy(std::begin(hsv_texture1D), std::end(hsv_texture1D), texture.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_HSV_with_isolines(Texture & texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = 256;
    texture.data = new uint8_t[768];
    std::copy(std::begin(hsv_texture1D_with_isolines), std::end(hsv_texture1D_with_isolines), texture.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_parula(Texture & texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = 64;
    texture.data = new uint8_t[192];
    std::copy(std::begin(parula_texture1D), std::end(parula_texture1D), texture.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_parula_with_isolines(Texture & texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = 80;
    texture.data = new uint8_t[240];
    std::copy(std::begin(parula_w_isolines_texture1D), std::end(parula_w_isolines_texture1D), texture.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_bitmap(Texture & texture, const char *bitmap)
{
#ifdef CINOLIB_USES_QT
    // https://stackoverflow.com/questions/20245865/render-qimage-with-opengl
    QImage img = QGLWidget::convertToGLFormat(QImage(bitmap));
    if (img.height()!=img.width())
    {
        std::cerr << "CinoLib supports only textures with 1:1 ratio. Image discarded!" << std::endl;
        return;
    }
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = img.height();
    texture.data = new uint8_t[texture.size*texture.size*4];
    std::copy(img.bits(), img.bits()+(texture.size*texture.size*4), texture.data);
#else
    std::cerr << "ERROR : Qt missing. Install Qt and recompile defining symbol CINOLIB_USES_QT" << std::endl;
#endif
}

}
