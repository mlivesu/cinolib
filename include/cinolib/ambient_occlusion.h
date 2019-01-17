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

#ifdef CINOLIB_USES_OPENGL
#ifdef CINOLIB_USES_QT

#include <QGLPixelBuffer>
#include <cinolib/sphere_coverage.h>
#include <cinolib/scalar_field.h>

namespace cinolib
{

template<class Mesh>
class AO : public QGLPixelBuffer
{
    public:

        ScalarField ao_poly_values;

        AO(Mesh & m, const int size = 500, const int ndirs = 256) : QGLPixelBuffer(size,size)
        {
            ao_poly_values = ScalarField(m.num_polys());

            makeCurrent();
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();

            // choose a set of points of view that evenly cover the unit sphere
            std::vector<vec3d> dirs;
            sphere_coverage(ndirs, dirs);

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
                double modelview[16];
                glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
                double projection[16] =
                {
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
                };
                int viewport[4] = { 0, 0, size, size };
                glViewport(0, 0, size, size);

                m.draw();
                float depth_buffer[size*size];
                glReadPixels(0, 0, size, size, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);

                // accumulate AO values, weighting views with the dot between
                // local surface normal and ray direction
                for(uint pid=0; pid<m.num_polys(); ++pid)
                {
                    vec3d  p = m.poly_centroid(pid);
                    double x, y, depth;
                    gluProject(p.x(), p.y(), p.z(), modelview, projection, viewport, &x, &y, &depth);

                    if(depth_buffer[size*int(y)+int(x)]+0.0025 > depth)
                    {
                        double diff = std::max(-u.dot(m.poly_data(pid).normal), 0.0);
                        ao_poly_values[pid] += diff;
                    }
                }
            }
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

            ao_poly_values.normalize_in_01();
            for(uint pid=0; pid<m.num_polys(); ++pid)
            {
                //std::cout << ao[vid] << std::endl;
                m.poly_data(pid).color *= ao_poly_values[pid];
            }
            m.updateGL();
        }
};

}

#endif // #ifdef CINOLIB_USES_QT
#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_AMBIENT_OCCLUSION_H
