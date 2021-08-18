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

        mat<4,4,T> model;      // from MODEL space to WORLD  space
        mat<4,4,T> view;       // from WORLD space to CAMERA space
        mat<4,4,T> projection; // viewing frustum

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Camera();

        // METHODS TO SET THE MODELVIEW MATRICES :::::::::::::::::::::::::::::::::

        void set_MV(const mat<3,1,T> & eye,
                    const mat<3,1,T> & center,
                    const mat<3,1,T> & up);

        void set_MV(const mat<3,1,T> & scene_center,
                    const T          & scene_radius);

        // METHODS TO SET THE PROJECTION MATRIX ::::::::::::::::::::::::::::::::::

        void set_PR_ortho(const T & l,    const T & r,  const T & b, const T & t, const T & n, const T & f);
        void set_PR_persp(const T & l,    const T & r,  const T & b, const T & t, const T & n, const T & f);
        void set_PR_persp(const T & fovY, const T & ar, const T & n, const T & f);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void zoom     (const T & factor);
        void rotate   (const mat<3,1,T> & axis, const T & deg);
        void translate(const mat<3,1,T> & delta);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::string serialize() const;
        void        deserialize(const std::string & s);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void print() const;
};

}

#ifndef  CINO_STATIC_LIB
#include "camera.cpp"
#endif

#endif // CAMERA_H
