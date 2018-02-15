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
#include <cinolib/textures/textures.h>
#include <cinolib/textures/quality_ramp_texture.h>
#include <cinolib/textures/quality_ramp_texture_plus_isolines.h>
#include <cinolib/serialize_2D.h>
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
void texture_HSV_ramp(Texture & texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = 256;
    texture.data = new uint8_t[768];
    std::copy(std::begin(quality_ramp_texture1D), std::end(quality_ramp_texture1D), texture.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_HSV_ramp_with_isolines(Texture & texture)
{
    if (texture.id > 0) glDeleteTextures(1, &texture.id);
    glGenTextures(1, &texture.id);

    delete[] texture.data;
    texture.size = 256;
    texture.data = new uint8_t[768];
    std::copy(std::begin(quality_ramp_texture1D_with_isolines), std::end(quality_ramp_texture1D_with_isolines), texture.data);
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
