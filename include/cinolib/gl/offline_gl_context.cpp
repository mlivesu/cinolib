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
#include <cinolib/gl/offline_gl_context.h>
#include <assert.h>

namespace cinolib
{

CINO_INLINE
GLFWwindow * create_offline_GL_context(const int w,  const int h)
{
    // create an invisible window for offline rendering as suggested in
    // https://www.glfw.org/docs/latest/context.html#context_offscreen
    //
    // since the window may be associated to a retina display or similar,
    // get the monitor pixel scale factor and adjust the input width and
    // height to make sure that the size of OpenGL buffers matches them.
    // To avoid roundoff errors due to cast to int, w and h MUST BE EVEN
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    assert(monitor!=NULL);
    float w_scale, h_scale;
    glfwGetMonitorContentScale(monitor, &w_scale, &h_scale);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* context = glfwCreateWindow(w/w_scale, h/h_scale, "", NULL, NULL);
    glfwDefaultWindowHints(); // restore default hints
    int fb_w,fb_h;
    glfwGetFramebufferSize(context, &fb_w, &fb_h);
    assert(w%2==0 && h%2==0); // sanity checks
    assert(fb_w==w && fb_h==h);
    return context;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void destroy_offline_GL_context(GLFWwindow * context)
{
    glfwDestroyWindow(context);
}

}
