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
#ifndef CINO_DRAW_SPHERE_H
#define CINO_DRAW_SPHERE_H

#ifdef CINOLIB_USES_OPENGL_GLFW_IMGUI

#include <cinolib/gl/gl_glu_glfw.h>
#include <cinolib/geometry/vec_mat.h>
#include <vector>

namespace cinolib
{

CINO_INLINE
static void sphere(const double * center,
                   const float    radius,
                   const float  * color,
                   const uint     subdiv = 1) // number of subdivisions of the regular icosahedron
{
    // vertices of an Icosahedron centered at the origin and having radius 1
    std::vector<double> verts =
    {
        -0.5257310271,  0.0000000000, -0.8506510258,
         0.5257310271,  0.0000000000, -0.8506510258,
         0.5257310271,  0.0000000000,  0.8506510258,
        -0.5257310271,  0.0000000000,  0.8506510258,
        -0.8506510258, -0.5257310271,  0.0000000000,
        -0.8506510258,  0.5257310271,  0.0000000000,
         0.8506510258,  0.5257310271,  0.0000000000,
         0.8506510258, -0.5257310271,  0.0000000000,
         0.0000000000, -0.8506510258,  0.5257310271,
         0.0000000000, -0.8506510258, -0.5257310271,
         0.0000000000,  0.8506510258, -0.5257310271,
         0.0000000000,  0.8506510258,  0.5257310271
    };

    // triangulation of the Icosahedron
    std::vector<uint> polys =
    {
         1,  9,  0,
        10,  1,  0,
         5, 10,  0,
         4,  5,  0,
         9,  4,  0,
         8,  2,  3,
         4,  8,  3,
         5,  4,  3,
        11,  5,  3,
         2, 11,  3,
        11,  2,  6,
        10, 11,  6,
         1, 10,  6,
         7,  1,  6,
         2,  7,  6,
        11, 10,  5,
         9,  8,  4,
         7,  2,  8,
         9,  7,  8,
         1,  7,  9
    };

    // subdivide the regular icosahedron to obtain the wanted smoothness
    for(uint i=0; i<subdiv; ++i)
    {
        std::vector<uint> tmp_polys;
        for(auto it=polys.begin(); it!=polys.end(); ++it)
        {
            uint vid0 = *it; ++it;
            uint vid1 = *it; ++it;
            uint vid2 = *it;

            vec3d v0(&(verts[vid0*3]));
            vec3d v1(&(verts[vid1*3]));
            vec3d v2(&(verts[vid2*3]));

            // split edges and project midpoints to the sphere
            vec3d v01 = v0 + v1;
            vec3d v12 = v1 + v2;
            vec3d v20 = v2 + v0;
            v01 /= v01.norm();
            v12 /= v12.norm();
            v20 /= v20.norm();

            // add three new vertices to the list
            uint vid01 = verts.size()/3.0;
            uint vid12 = vid01 + 1;
            uint vid20 = vid01 + 2;
            verts.push_back(v01.x());
            verts.push_back(v01.y());
            verts.push_back(v01.z());
            verts.push_back(v12.x());
            verts.push_back(v12.y());
            verts.push_back(v12.z());
            verts.push_back(v20.x());
            verts.push_back(v20.y());
            verts.push_back(v20.z());

            // add four new triangles to the list
            tmp_polys.push_back(vid0 );
            tmp_polys.push_back(vid01);
            tmp_polys.push_back(vid20);
            tmp_polys.push_back(vid01);
            tmp_polys.push_back(vid1 );
            tmp_polys.push_back(vid12);
            tmp_polys.push_back(vid01);
            tmp_polys.push_back(vid12);
            tmp_polys.push_back(vid20);
            tmp_polys.push_back(vid20);
            tmp_polys.push_back(vid12);
            tmp_polys.push_back(vid2 );
        }
        polys = tmp_polys;
    }

    // true only for the unit sphere: vertex coordinates coincide with surface normals
    std::vector<double> normals = verts;

    // scale and translate vertices
    for(uint i=0; i<verts.size(); i+=3)
    {
        vec_times<3,double>(&(verts[i]), radius, &(verts[i]));
        vec_plus<3,double> (&(verts[i]), center, &(verts[i]));
    }

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColor3fv(color);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, verts.data());
    glNormalPointer(GL_DOUBLE, 0, normals.data());
    glDrawElements(GL_TRIANGLES, (GLsizei)polys.size(), GL_UNSIGNED_INT, polys.data());
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glColor3f(1.f,1.f,1.f);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
}

}

#endif // #ifdef CINOLIB_USES_OPENGL_GLFW_IMGUI

#endif // CINO_DRAW_SPHERE_H
