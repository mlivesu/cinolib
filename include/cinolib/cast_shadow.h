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
#ifndef CINO_CAST_SHADOW_H
#define CINO_CAST_SHADOW_H

#include <cinolib/geometry/vec_mat.h>
#include <cinolib/gl/gl_glfw.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
void cast_shadow(const Mesh    & m,     // mesh to be rendered
                 const vec3d   & dir,   // light direction
                 const uint      w,     // width  (must be an EVEN number)
                 const uint      h,     // height (must be an EVEN number)
                       uint8_t * data); // w x h buffer, 8 bits per pixel

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void cast_shadow(const Mesh       & m,           // mesh to be rendered
                 const vec3d      & dir,         // light direction
                 const uint         w,           // width  (must be an EVEN number)
                 const uint         h,           // height (must be an EVEN number)
                       uint8_t    * data,        // w x h buffer, 8 bits per pixel
                       GLFWwindow * GL_context); // cached GL context (for amortized calls)
}

#ifndef  CINO_STATIC_LIB
#include "cast_shadow.cpp"
#endif

#endif // CINO_CAST_SHADOW_H
