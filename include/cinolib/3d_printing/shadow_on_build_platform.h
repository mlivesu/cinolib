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
#ifndef CINO_SHADOW_ON_BUILD_PLATFORM_H
#define CINO_SHADOW_ON_BUILD_PLATFORM_H

#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/gl/gl_glfw.h>

namespace cinolib
{

/* projects the mesh m onto the building platform using rasterization
 * on an offline GL buffer of size img_size x img_size. This can be
 * useful to aid packing methods that aim to optimally fill the building
 * platform with multiple pieces.
 *
 * The method returns the ratio between shadow pixel and total amount of
 * pixel in the image.
 *
 * In case the method is called multiple times it is conveniente to pass
 * a GL context so as to amortize the cost of initialization.
*/

template<class M, class V, class E, class P>
CINO_INLINE
float shadow_on_build_platform(const DrawableTrimesh<M,V,E,P> & m,         //
                               const vec3d                    & build_dir, //
                               const uint                       img_size,  // frame buffer will be img_size x img_size
                                     u_int8_t                 * data);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
float shadow_on_build_platform(const DrawableTrimesh<M,V,E,P> & m,           //
                               const vec3d                    & build_dir,   //
                               const uint                       img_size,    // frame buffer will be img_size x img_size
                                     u_int8_t                 * data,        //
                                     GLFWwindow               * GL_context); // cached for amortized computation


}

#ifndef  CINO_STATIC_LIB
#include "shadow_on_build_platform.cpp"
#endif

#endif // CINO_SHADOW_ON_BUILD_PLATFORM_H
