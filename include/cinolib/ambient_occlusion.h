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
#include <cinolib/scalar_field.h>

namespace cinolib
{

/* These classes compute per face AO values by projecting the mesh on the framebuffer and
 * using the depth buffer to test visiblity. AO values are stored on mesh faces, and averaged
 * at mesh vertices only if smooth shading is enabled, as happens for surface normals. AO_srf
 * is dedicated to triangle, hexahedral and polygonal meshes. AO_vol is for tetraheral, hexahedral
 * and general polyhedral meshes
*/

template<class Mesh>
class AO_srf : public QGLPixelBuffer
{
    ScalarField ao;

    public:

        AO_srf(const Mesh & m, const int buffer_size = 500, const int n_dirs = 256);
        void copy_to_mesh(Mesh & m);
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class Mesh>
class AO_vol : public QGLPixelBuffer
{
    ScalarField ao;
    std::vector<bool> visible;

    public:

        AO_vol(const Mesh & m, const int buffer_size = 500, const int n_dirs = 256);
        void copy_to_mesh(Mesh & m);
};

}

#ifndef  CINO_STATIC_LIB
#include "ambient_occlusion.cpp"
#endif

#endif // #ifdef CINOLIB_USES_QT
#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_AMBIENT_OCCLUSION_H
