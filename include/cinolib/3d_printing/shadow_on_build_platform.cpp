/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#include <cinolib/3d_printing/shadow_on_build_platform.h>
#include <cinolib/cast_shadow.h>
#include <cinolib/gl/offline_gl_context.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
float shadow_on_build_platform(const DrawableTrimesh<M,V,E,P> & m,         //
                               const vec3d                    & build_dir, //
                               const uint                       img_size,  // frame buffer will be img_size x img_size
                                     u_int8_t                 * data)
{
    GLFWwindow* GL_context = create_offline_GL_context(img_size,img_size);
    if(GL_context==NULL)
    {
        std::cerr << "Impossible to create a GL context. Make sure GLFW has been initialized" << std::endl;
        return -1;
    }
    float res = shadow_on_build_platform(m, build_dir, img_size, data, GL_context);
    destroy_offline_GL_context(GL_context);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
float shadow_on_build_platform(const DrawableTrimesh<M,V,E,P> & m,          //
                               const vec3d                    & build_dir,  //
                               const uint                       img_size,   // frame buffer will be img_size x img_size
                                     u_int8_t                 * data,       //
                                     GLFWwindow               * GL_context) // cached for amortized computation
{
    cast_shadow(m, build_dir, img_size, img_size, data, GL_context);
    uint shadow_pixels = 0;
    for(uint i=0; i<img_size*img_size; ++i)
    {
        if(data[i]==0xFF) ++shadow_pixels;
    }
    return (float)shadow_pixels/(img_size*img_size);
}

}
