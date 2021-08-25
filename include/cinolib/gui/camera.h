/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
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
#ifndef CAMERA_H
#define CAMERA_H

#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* This class implements a virtual camera that observes a 3D scene. It is
 * internally represented following the standard OpenGL's MODEL-VIEW-PROJECTION
 * paradigm, which allows to move back and forth from objet, to world, to eye
 * coordinates with a stack of 4x4 hmogeneous matrices. Differently from OpenGL,
 * where model and view matrices are condensed into a single "modelview" matrix,
 * here the two matrices are kept separated for ease of coding and understanding.
 *
 * This class does not contain a single line of OpenGL code. The idea is that here
 * we provide code to setup the matrices you need, and then these matrices can be
 * passed to your OpenGL context through the following code (which will likely be
 * included into you GLcanvas class, such as our cinolib::GLcanvas)
 *
 *   glMatrixMode(GL_MODELVIEW);
 *   glLoadMatrixd((camera.view * camera.model).transpose().ptr());
 *   glMatrixMode(GL_PROJECTION);
 *   glLoadMatrixd(camera.projection.transpose().ptr());
 *
 * NOTE: matrices are transposed because OpenGL uses a column-major ordering for
 * its matrices, whereas cinolib uses the standard row-major ordering.
 *
 * Here is a great reference on GL transformations:
 * http://www.songho.ca/opengl/gl_transform.html
*/

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
class Camera
{
    public:

        int        width, height; // sensor size
        mat<3,1,T> scene_center;  // center of the scene
        double     scene_radius;  // radius of the smallest ball containing the whole scene
        double     zoom_factor;   // works for both perspective and orthographic modes
        mat<4,4,T> model;         // from OBJECT space to WORLD  space
        mat<4,4,T> view;          // from WORLD  space to CAMERA space
        mat<4,4,T> projection;    // defines the view frustum

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Camera(const int width = 640, const int height = 480);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void reset();
        void reset_matrices();
        void reset_modelview();
        void reset_projection();
        void reset_projection_persp();
        void reset_projection_ortho();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<4,4,T> MV()  const; // Model-View matrix
        mat<4,4,T> MVP() const; // Model-View-Projecetion matrix

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void toggle_persp_ortho();
        bool is_ortho() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_rotation_pivot(const mat<3,1,T> & p);
        void set_focus_point   (const mat<3,1,T> & p);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void zoom       (const T & delta);
        void rotate     (const T & deg, const mat<3,1,T> & axis);
        void rotate_x   (const T & deg);
        void rotate_y   (const T & deg);
        void rotate_z   (const T & deg);
        void translate  (const mat<3,1,T> & delta);
        void translate_x(const T & delta);
        void translate_y(const T & delta);
        void translate_z(const T & delta);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::string serialize() const;
        void        deserialize(const std::string & s);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void print() const;

        // STATIC FACILITIES FOR MATRIX SYNTHESIS ::::::::::::::::::::::::::::::::

        static mat<4,4,T> look_at(const mat<3,1,T> & eye,    // camera pos
                                  const mat<3,1,T> & center, // center of scene (rotation pivot)
                                  const mat<3,1,T> & up);    // upwards direction

        static mat<4,4,T> frustum_ortho(const T & l,   // left
                                        const T & r,   // right
                                        const T & b,   // bottom
                                        const T & t,   // top
                                        const T & n,   // near
                                        const T & f);  // far

        static mat<4,4,T> frustum_persp(const T & l,   // left
                                        const T & r,   // right
                                        const T & b,   // bottom
                                        const T & t,   // top
                                        const T & n,   // near
                                        const T & f);  // far

        static mat<4,4,T> frustum_persp(const T & fov, // vertical field of view (degrees)
                                        const T & ar,  // aspect ratio
                                        const T & n,   // near
                                        const T & f);  // far

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
};

}

#ifndef  CINO_STATIC_LIB
#include "camera.cpp"
#endif

#endif // CAMERA_H
