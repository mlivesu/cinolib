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

namespace cinolib
{

CINO_INLINE
void load_texture(GLuint & texture_id, const TextureType & type)
{
    if (texture_id > 0) glDeleteTextures(1, &texture_id);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    float pixels[3*8*8];
    for(int r=0; r<8; ++r)
    for(int c=0; c<8; ++c)
    {
        pixels[3*8*r+3*c  ] = 1.0;
        pixels[3*8*r+3*c+1] = 1.0;
        pixels[3*8*r+3*c+2] = 1.0;
    }
    for(int c=0; c<8; ++c)
    {
        pixels[3*c  ] = 1.0;
        pixels[3*c+1] = 0.0;
        pixels[3*c+2] = 0.0;

        pixels[3*8*7+3*c  ] = 1.0;
        pixels[3*8*7+3*c+1] = 0.0;
        pixels[3*8*7+3*c+2] = 0.0;
    }
    for(int r=0; r<8; ++r)
    {
        pixels[3*8*r+0] = 0.0;
        pixels[3*8*r+1] = 0.0;
        pixels[3*8*r+2] = 1.0;

        pixels[3*8*r+3*7  ] = 0.0;
        pixels[3*8*r+3*7+1] = 0.0;
        pixels[3*8*r+3*7+2] = 1.0;
    }

//    float pixels[grid_size*grid_size*3];
//    for(int r=0; r<grid_size; ++r)
//    for(int c=0; c<grid_size; ++c)
//    {
//        if (r%2 == c%2)
//        {
//            pixels[3*grid_size*r+3*c  ] = c0.r;
//            pixels[3*grid_size*r+3*c+1] = c0.g;
//            pixels[3*grid_size*r+3*c+2] = c0.b;
//        }
//        else
//        {
//            pixels[3*grid_size*r+3*c  ] = c1.r;
//            pixels[3*grid_size*r+3*c+1] = c1.g;
//            pixels[3*grid_size*r+3*c+2] = c1.b;
//        }
//    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_FLOAT, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}


}
