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
#include <cinolib/cast_shadow.h>
#include <cinolib/gl/offline_gl_context.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
void cast_shadow(const Mesh    & m,    // mesh to be rendered
                 const vec3d   & dir,  // light direction
                 const uint      w,    // width  (must be an EVEN number)
                 const uint      h,    // height (must be an EVEN number)
                       uint8_t * data) // w x h buffer, 8 bits per pixel
{
    GLFWwindow* GL_context = create_offline_GL_context(w,h);
    if(GL_context==NULL)
    {
        std::cerr << "Impossible to create a GL context. Make sure GLFW has been initialized" << std::endl;
        return;
    }
    cast_shadow(m, dir, w, h, data, GL_context);
    destroy_offline_GL_context(GL_context);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void cast_shadow(const Mesh       & m,           // mesh to be rendered
                 const vec3d      & dir,         // light direction
                 const uint         w,           // width  (must be an EVEN number)
                 const uint         h,           // height (must be an EVEN number)
                       uint8_t    * data,        // w x h buffer, 8 bits per pixel
                       GLFWwindow * GL_context)  // cached GL context (for amortized calls)
{
    // set the model-view-projection-viewport
    glfwMakeContextCurrent(GL_context);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    vec3d  Z(0,0,1);
    vec3d  a = dir.cross(Z); a.normalize();
    vec3d  c = m.centroid();
    double s = 2.0/m.bbox().diag();
    glPushMatrix();
    glLoadIdentity();
    glRotatef(Z.angle_deg(dir), a.x(), a.y(), a.z());
    glScaled(s,s,s);
    glTranslated(-c.x(), -c.y(), -c.z());
    glViewport(0,0,w,h);

    // use the stencil buffer to produce a monocrome image (8 bits) with:
    // 0x00: background
    // 0xff: foreground
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    // render
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    m.draw();
    // dump the stencil buffer and destroy the GL context
    glReadPixels(0, 0, w, h, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, data);
}

}

