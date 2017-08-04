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
#include <cinolib/common.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void load_texture(GLuint & texture_id, const TextureType & type)
{
    if (texture_id > 0) glDeleteTextures(1, &texture_id);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    switch (type)
    {
        case TEXTURE_1D_ISOLINES :            break;
        case TEXTURE_1D_HSV_RAMP :            break;
        case TEXTURE_1D_HSV_RAMP_W_ISOLINES : break;
        case TEXTURE_2D_CHECKERBOARD :        make_texture_checkerboard(); break;
        case TEXTURE_2D_ISOLINES:             make_texture_isolines2D();   break;
        default: assert("Unknown Texture!" && false);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void make_texture_isolines2D(const Color & u_isolines,
                             const Color & v_isolines,
                             const Color & background)
{
    uint size = 8;
    float pixels[3*size*size];
    for(uint r=0; r<size; ++r)
    for(uint c=0; c<size; ++c)
    {
        uint i = serialize_2D_index(r,c,size);
        pixels[3*i  ] = background.r;
        pixels[3*i+1] = background.g;
        pixels[3*i+2] = background.b;
    }
    for(uint c=0; c<size; ++c)
    {
        uint i = serialize_2D_index(0,c,size);
        pixels[3*i  ] = u_isolines.r;
        pixels[3*i+1] = u_isolines.g;
        pixels[3*i+2] = u_isolines.b;

        i = serialize_2D_index(size-1,c,size);
        pixels[3*i  ] = u_isolines.r;
        pixels[3*i+1] = u_isolines.g;
        pixels[3*i+2] = u_isolines.b;
    }
    for(uint r=0; r<size; ++r)
    {
        uint i = serialize_2D_index(r,0,size);
        pixels[3*i  ] = v_isolines.r;
        pixels[3*i+1] = v_isolines.g;
        pixels[3*i+2] = v_isolines.b;

        i = serialize_2D_index(r,size-1,size);
        pixels[3*i  ] = v_isolines.r;
        pixels[3*i+1] = v_isolines.g;
        pixels[3*i+2] = v_isolines.b;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_FLOAT, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void make_texture_checkerboard(const Color & c0, const Color & c1)
{
    uint size = 8;
    float pixels[size*size*3];
    for(uint r=0; r<size; ++r)
    for(uint c=0; c<size; ++c)
    {
        uint i = serialize_2D_index(r,c,size);
        if (r%2 == c%2)
        {
            pixels[3*i  ] = c0.r;
            pixels[3*i+1] = c0.g;
            pixels[3*i+2] = c0.b;
        }
        else
        {
            pixels[3*i  ] = c1.r;
            pixels[3*i+1] = c1.g;
            pixels[3*i+2] = c1.b;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_FLOAT, pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_FLOAT, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

}
