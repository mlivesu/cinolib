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
#include <cinolib/deg_rad.h>

namespace cinolib
{

/* This class computes per face AO values by projecting the mesh on the framebuffer and
 * using the depth buffer to test visiblity.
*/

template<class Mesh>
class AO : public QGLPixelBuffer
{
    public:

        ScalarField ao_poly_values;

        AO(const Mesh & m,
           const int    buffer_size = 500,
           const int    n_dirs      = 256) : QGLPixelBuffer(buffer_size,buffer_size)
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
            sphere_coverage(n_dirs, dirs);

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
                int viewport[4] = { 0, 0, buffer_size, buffer_size };
                glViewport(0, 0, buffer_size, buffer_size);

                m.draw();
                float depth_buffer[buffer_size*buffer_size];
                glReadPixels(0, 0, buffer_size, buffer_size, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);

                // accumulate AO values, weighting views with the dot between
                // local surface normal and ray direction
                for(uint pid=0; pid<m.num_polys(); ++pid)
                {
                    vec3d  p = m.poly_centroid(pid);
                    double x, y, depth;
                    gluProject(p.x(), p.y(), p.z(), modelview, projection, viewport, &x, &y, &depth);

                    if(depth_buffer[buffer_size*int(y)+int(x)]+0.0025 > depth)
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
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // transfer the per face AO value on the mesh. To provide smoother shades
        // AO values can be smoothed on the dual mesh, using an angle barrier to
        // avoid averaging AO values between faces with a dihedral angle higher than
        // a given threshold
        //
        void copy_to_mesh(Mesh & m, const uint n_smooth_iters = 1, const float angle_thresh = 60.0)
        {
            for(uint i=0; i<n_smooth_iters; ++i)
            {
                for(uint pid=0; pid<m.num_polys(); ++pid)
                {
                    double val   = ao_poly_values[pid];
                    double count = 1.0;
                    for(uint nbr : m.adj_p2p(pid))
                    {
                        uint eid = m.edge_shared(pid, nbr);
                        if(to_deg(m.edge_crease_angle(eid)) < angle_thresh)
                        {
                            val   += ao_poly_values[nbr];
                            count += 1.0;
                        }
                    }
                    val/=count;
                    m.poly_data(pid).AO = val;
                }
            }
        }
};



template<class Mesh>
class AO_vol : public QGLPixelBuffer
{
    public:

        ScalarField ao_face_values;

        AO_vol(const Mesh & m,
           const int    buffer_size = 500,
           const int    n_dirs      = 256) : QGLPixelBuffer(buffer_size,buffer_size)
        {
            ao_face_values = ScalarField(m.num_faces());

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
            sphere_coverage(n_dirs, dirs);

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
                int viewport[4] = { 0, 0, buffer_size, buffer_size };
                glViewport(0, 0, buffer_size, buffer_size);

                m.draw();
                float depth_buffer[buffer_size*buffer_size];
                glReadPixels(0, 0, buffer_size, buffer_size, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);

                // accumulate AO values, weighting views with the dot between
                // local surface normal and ray direction
                for(uint pid=0; pid<m.num_polys(); ++pid)
                {
                    if (!m.poly_data(pid).visible) continue;

                    for(uint fid : m.adj_p2f(pid))
                    {
                        std::vector<uint> visible_polys;
                        for(uint nbr : m.adj_f2p(fid))
                        {
                            if (m.poly_data(nbr).visible) visible_polys.push_back(nbr);
                        }
                        if (visible_polys.size()!=1) continue;

                        vec3d  p = m.face_centroid(fid);
                        double x, y, depth;
                        gluProject(p.x(), p.y(), p.z(), modelview, projection, viewport, &x, &y, &depth);

                        if(depth_buffer[buffer_size*int(y)+int(x)]+0.0025 > depth)
                        {
                            double diff = std::max(-u.dot(m.poly_face_normal(pid,fid)), 0.0);
                            ao_face_values[fid] += diff;
                        }
                    }
                }
            }
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

            ao_face_values.normalize_in_01();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // transfer the per face AO value on the mesh. To provide smoother shades
        // AO values can be smoothed on the dual mesh, using an angle barrier to
        // avoid averaging AO values between faces with a dihedral angle higher than
        // a given threshold
        //
        void copy_to_mesh(Mesh & m, const uint n_smooth_iters = 1, const float angle_thresh = 60.0)
        {
            for(uint i=0; i<n_smooth_iters; ++i)
            {
                for(uint fid=0; fid<m.num_faces(); ++fid)
                {
                    double val   = ao_face_values[fid];
                    double count = 1.0;
                    for(uint nbr : m.adj_f2f(fid))
                    {
                        std::vector<uint> visible_polys;
                        for(uint pid : m.adj_f2p(nbr))
                        {
                            if (m.poly_data(pid).visible) visible_polys.push_back(pid);
                        }
                        if (visible_polys.size()!=1) continue;

                        uint eid = m.edge_shared(fid, nbr);
                        if(to_deg(m.edge_crease_angle(eid)) < angle_thresh)
                        {
                            val   += ao_face_values[nbr];
                            count += 1.0;
                        }
                    }
                    val/=count;
                    m.face_data(fid).AO = val;
                }
            }
        }
};


}

#endif // #ifdef CINOLIB_USES_QT
#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_AMBIENT_OCCLUSION_H
