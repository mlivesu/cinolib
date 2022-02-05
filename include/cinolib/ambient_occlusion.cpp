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
#include <cinolib/ambient_occlusion.h>
#include <cinolib/gl/gl_glfw.h>
#include <cinolib/gl/glproject.h>
#include <cinolib/gl/glunproject.h>
#include <cinolib/sphere_coverage.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/parallel_for.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
void ambient_occlusion_srf_meshes(      Mesh & m,
                                  const int    buffer_size,
                                  const uint   sample_dirs,
                                  const bool   init_glfw)
{
    std::vector<float> ao(m.num_polys(),0);
    std::vector<vec3d> dirs;
    sphere_coverage(sample_dirs, dirs);

    if(init_glfw) glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // https://www.glfw.org/docs/latest/context.html#context_offscreen
    GLFWwindow* GL_context = glfwCreateWindow(buffer_size, buffer_size, "", NULL, NULL);
    glfwDefaultWindowHints(); // restore default hints

    glfwMakeContextCurrent(GL_context);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    float* z_buffer = new float[buffer_size*buffer_size];

    for(vec3d u : dirs)
    {
        // for each POV render on a buffer, and do a visibility check
        // by reading values from the Z buffer and comparing with the actual depth

        glClear(GL_DEPTH_BUFFER_BIT);
        vec3d w(1,0,0), v;
        v = u.cross(w); v.normalize();
        w = v.cross(u); w.normalize();
        double mat[16]=
        {
            w[0], v[0], u[0], 0,
            w[1], v[1], u[1], 0,
            w[2], v[2], u[2], 0,
            0,    0,    0,    1
        };
        glLoadIdentity();
        glMultMatrixd(mat);
        vec3d c = -m.bbox().center();
        vec3d s =  m.bbox().delta();
        glScaled(1.0/s.x(), 1.0/s.y(), 1.0/s.z());
        glTranslated(c.x(), c.y(), c.z());

        m.draw();
        glReadPixels(0, 0, buffer_size, buffer_size, GL_DEPTH_COMPONENT, GL_FLOAT, z_buffer);

        mat4d modelview;
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview.ptr());
        modelview = modelview.transpose(); // from column-major to row-major
        mat4d projection = mat4d::DIAG(1);
        mat2i viewport({ 0, 0, buffer_size, buffer_size });
        glViewport(0, 0, buffer_size, buffer_size);

        // accumulate AO values, weighting views with the dot between
        // local surface normal and ray direction
        PARALLEL_FOR(0, m.num_polys(), 1000, [&](const uint pid)
        {
            if(!m.poly_data(pid).flags[HIDDEN])
            {
                vec2d  pp;
                double depth;
                gl_project(m.poly_centroid(pid), modelview, projection, viewport, pp, depth);

                if(z_buffer[buffer_size*int(pp.y())+int(pp.x())]+0.0025 > depth)
                {
                    double diff = std::max(-u.dot(m.poly_data(pid).normal), 0.0);
                    ao[pid] += diff;
                }
            }
        });
    }
    delete[] z_buffer;
    glfwDestroyWindow(GL_context);
    if(init_glfw) glfwTerminate();

    // apply AO
    auto min_max = std::minmax_element(ao.begin(), ao.end());
    auto min     = *min_max.first;
    auto max     = *min_max.second;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        m.poly_data(pid).AO = (m.poly_data(pid).flags[HIDDEN]) ? 1.0 : (ao[pid]-min)/max;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
CINO_INLINE
void ambient_occlusion_vol_meshes(      Mesh & m,
                                  const int    buffer_size,
                                  const uint   sample_dirs,
                                  const bool   init_glfw)
{
    std::vector<float> ao(m.num_faces(),0);
    std::vector<bool>  face_visible(m.num_faces(),false);
    std::vector<vec3d> dirs;
    sphere_coverage(sample_dirs, dirs);

    if(init_glfw) glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // https://www.glfw.org/docs/latest/context.html#context_offscreen
    GLFWwindow* GL_context = glfwCreateWindow(buffer_size, buffer_size, "", NULL, NULL);
    glfwDefaultWindowHints(); // restore default hints

    glfwMakeContextCurrent(GL_context);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    float* z_buffer = new float[buffer_size*buffer_size];

    for(vec3d u : dirs)
    {
        // for each POV render on a buffer, and do a visibility check
        // by reading values from the Z buffer and comparing with the actual depth

        glClear(GL_DEPTH_BUFFER_BIT);
        vec3d w(1,0,0), v;
        v = u.cross(w); v.normalize();
        w = v.cross(u); w.normalize();
        double mat[16]=
        {
            w[0], v[0], u[0], 0,
            w[1], v[1], u[1], 0,
            w[2], v[2], u[2], 0,
            0,    0,    0,    1
        };
        glLoadIdentity();
        glMultMatrixd(mat);
        vec3d c = -m.bbox().center();
        vec3d s =  m.bbox().delta();
        glScaled(1.0/s.x(), 1.0/s.y(), 1.0/s.z());
        glTranslated(c.x(), c.y(), c.z());

        m.draw();
        glReadPixels(0, 0, buffer_size, buffer_size, GL_DEPTH_COMPONENT, GL_FLOAT, z_buffer);

        mat4d modelview;
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview.ptr());
        modelview = modelview.transpose(); // from column-major to row-major
        mat4d projection = mat4d::DIAG(1);
        mat2i viewport({ 0, 0, buffer_size, buffer_size });
        glViewport(0, 0, buffer_size, buffer_size);

        // accumulate AO values, weighting views with the dot between
        // local surface normal and ray direction
        PARALLEL_FOR(0, m.num_faces(), 1000, [&](const uint fid)
        {
            uint pid_beneath;
            if(m.face_is_visible(fid, pid_beneath))
            {
                face_visible.at(fid) = true;
                vec2d  pp;
                double depth;
                gl_project(m.face_centroid(fid), modelview, projection, viewport, pp, depth);

                if(z_buffer[buffer_size*int(pp.y())+int(pp.x())]+0.0025 > depth)
                {
                    double diff = std::max(-u.dot(m.poly_face_normal(pid_beneath,fid)), 0.0);
                    ao[fid] += diff;
                }
            }
        });
    }
    delete[] z_buffer;
    glfwDestroyWindow(GL_context);
    if(init_glfw) glfwTerminate();

    // apply AO
    auto min_max = std::minmax_element(ao.begin(), ao.end());
    auto min     = *min_max.first;
    auto max     = *min_max.second;
    for(uint fid=0; fid<m.num_faces(); ++fid)
    {
        m.face_data(fid).AO = (face_visible.at(fid)) ? (ao[fid]-min)/max : 1.0;
    }
}

}
