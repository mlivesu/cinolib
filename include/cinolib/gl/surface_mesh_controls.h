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
#ifndef CINO_SURFACE_MESH_CONTROLS_H
#define CINO_SURFACE_MESH_CONTROLS_H

#include <cinolib/cino_inline.h>
#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/visual_control.h>
#include <cinolib/meshes/meshes.h>

namespace cinolib
{

template<class Mesh>
class SurfaceMeshControls : public VisualControl
{
    Mesh     * m   = nullptr;
    GLcanvas * gui = nullptr;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    typedef typename Mesh::M_type M;
    typedef typename Mesh::V_type V;
    typedef typename Mesh::E_type E;
    typedef typename Mesh::P_type P;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    bool  show_mesh       = true;
    bool  show_wireframe  = true;
    int   shading         = 2; // 0 = point, 1 = flat, 2 = smooth
    int   wireframe_width = 1;
    float wireframe_alpha = 1;
    int   color           = 1; // 0 = vert, 1 = poly, 2 = text 1D, 3 = text 2D

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    public:

        SurfaceMeshControls(Mesh *m, GLcanvas *gui, const std::string & name);
       ~SurfaceMeshControls() override {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void draw() override;
};

}

#ifndef  CINO_STATIC_LIB
#include "surface_mesh_controls.cpp"
#endif

#endif // CINO_SURFACE_MESH_CONTROLS_H
