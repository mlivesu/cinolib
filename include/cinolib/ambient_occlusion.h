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
*********************************************************************************/
#ifndef CINO_AMBIENT_OCCLUSION_H
#define CINO_AMBIENT_OCCLUSION_H

#ifdef CINOLIB_USES_OPENGL_GLFW_IMGUI

#include <cinolib/cino_inline.h>
#include <sys/types.h>

namespace cinolib
{

/* updates the ambient occlusion for the (visible portion of) an input mesh.
 * All surface and volumetric meshes are supported. AO values are approximated
 * with a dirty trick: the mesh is rendered from a given number of viepoints,
 * and visibility is checked for each render using the Z-buffer
 *
 * WARNING: this method uses GLFW, but it cannot know whether it has already
 * be initialized or not (e.g. by creating a GLcanvas). It is therefore up to
 * the programmer to instruct this method to call (or not call) glfwInit() and
 * glfwTerminate(). In short: if this is the first call to a GLFW in your
 * application, set init_glfw to true. If it's not, set it to false. Since AO
 * is for rendering, I would expect this flag to be false almost always.
*/

template<class Mesh>
CINO_INLINE
void ambient_occlusion_srf_meshes(      Mesh & m,
                                  const int    buffer_size = 350,
                                  const uint   sample_dirs = 256,
                                  const bool   init_glfw   = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void ambient_occlusion_vol_meshes(      Mesh & m,
                                  const int    buffer_size = 350,
                                  const uint   sample_dirs = 256,
                                  const bool   init_glfw   = false);
}

#ifndef  CINO_STATIC_LIB
#include "ambient_occlusion.cpp"
#endif

#endif // CINOLIB_USES_OPENGL_GLFW_IMGUI
#endif // CINO_AMBIENT_OCCLUSION_H
