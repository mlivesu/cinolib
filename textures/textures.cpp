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
#include <cinolib/textures/isolines_texture.h>
#include <cinolib/textures/quality_ramp_texture.h>
#include <cinolib/textures/quality_ramp_texture_plus_isolines.h>
#include <cinolib/common.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture(GLuint & texture_id, const int & texture_type)
{
    if (texture_id > 0) glDeleteTextures(1, &texture_id);
    glGenTextures(1, &texture_id);

    switch (texture_type)
    {
        case TEXTURE_1D_ISOLINES :            texture_isolines1D(texture_id);             break;
        case TEXTURE_1D_HSV_RAMP :            texture_HSV_ramp(texture_id);               break;
        case TEXTURE_1D_HSV_RAMP_W_ISOLINES : texture_HSV_ramp_with_isolines(texture_id); break;
        case TEXTURE_2D_CHECKERBOARD :        texture_checkerboard(texture_id);           break;
        case TEXTURE_2D_ISOLINES:             texture_isolines2D(texture_id);             break;
        default: assert("Unknown Texture!" && false);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_isolines2D(const GLuint   texture_id,
                             const Color  & u_isolines,
                             const Color  & v_isolines,
                             const Color  & background)
{
    uint size = 8;
    u_char texels[3*size*size];
    for(uint r=0; r<size; ++r)
    for(uint c=0; c<size; ++c)
    {
        uint i = 3 * serialize_2D_index(r,c,size);
        texels[i  ] = background.r_uchar();
        texels[i+1] = background.g_uchar();
        texels[i+2] = background.b_uchar();
    }
    for(uint c=0; c<size; ++c)
    {
        uint i = 3 * serialize_2D_index(0,c,size);
        texels[i  ] = u_isolines.r_uchar();
        texels[i+1] = u_isolines.g_uchar();
        texels[i+2] = u_isolines.b_uchar();
    }
    for(uint r=0; r<size; ++r)
    {
        uint i = 3 * serialize_2D_index(r,0,size);
        texels[i  ] = v_isolines.r_uchar();
        texels[i+1] = v_isolines.g_uchar();
        texels[i+2] = v_isolines.b_uchar();
    }
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, texels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_checkerboard(const GLuint   texture_id,
                               const Color  & c0,
                               const Color  & c1)
{
    uint size = 8;
    u_char texels[size*size*3];
    for(uint r=0; r<size; ++r)
    for(uint c=0; c<size; ++c)
    {
        uint i = 3 * serialize_2D_index(r,c,size);
        if (r%2 == c%2)
        {
            texels[i  ] = c0.r_uchar();
            texels[i+1] = c0.g_uchar();
            texels[i+2] = c0.b_uchar();
        }
        else
        {
            texels[i  ] = c1.r_uchar();
            texels[i+1] = c1.g_uchar();
            texels[i+2] = c1.b_uchar();
        }
    }
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, texels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_isolines1D(const GLuint   texture_id,
                             const Color  & c0,
                             const Color  & c1,
                             const uint     n_bands)
{
    u_char texels[3*n_bands];
    for(uint i=0; i<n_bands; ++i)
    {
        if (i%2)
        {
            texels[3*i  ] = c0.r_uchar();
            texels[3*i+1] = c0.g_uchar();
            texels[3*i+2] = c0.b_uchar();
        }
        else
        {
            texels[3*i  ] = c1.r_uchar();
            texels[3*i+1] = c1.g_uchar();
            texels[3*i+2] = c1.b_uchar();
        }
    };
    glBindTexture(GL_TEXTURE_1D, texture_id);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, n_bands, 0, GL_RGB, GL_UNSIGNED_BYTE, texels);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R,     GL_REPEAT);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_HSV_ramp(const GLuint texture_id)
{
    glBindTexture(GL_TEXTURE_1D, texture_id);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, quality_ramp_texture1D);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R,     GL_REPEAT);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void texture_HSV_ramp_with_isolines(const GLuint texture_id)
{
    glBindTexture(GL_TEXTURE_1D, texture_id);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, quality_ramp_texture1D_with_isolines);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R,     GL_REPEAT);
}

}
