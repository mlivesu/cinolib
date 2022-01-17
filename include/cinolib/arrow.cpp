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
#include <cinolib/arrow.h>
#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

template<class T>
CINO_INLINE
void arrow(const T            height,
           const T            radius,
           const T            base_rel_height, // percentage of the height (a good value is 0.7)
           const T            base_rel_radius, // percentage of the radius (a good value is 0.2)
           const uint         n_sides,         // cross section
           std::vector<T>    & verts,          // output vertices
           std::vector<uint> & tris,           // output triangles
           std::vector<T>    & normals)        // output normals (per vertex)
{
    T b_height = height * base_rel_height;
    T b_radius = radius * base_rel_radius;

    // add first three points
    verts   = {        0, 0,   height,
                b_radius, 0,        0,
                b_radius, 0, b_height,
                  radius, 0, b_height};
    normals = { 0, 0, 1,
                1, 0, 0,
                1, 0, 0,
                1, 0, 0 };
    tris    = {};

    // tessellate the base
    mat<2,1,T> b(b_radius,0);
    mat<2,1,T> t(radius,0);
    mat<2,2,T> R = mat<2,2,T>::ROT_2D(2*M_PI/n_sides);
    uint off = 0;
    for(uint i=1; i<n_sides; ++i)
    {
        // base verts
        b = R*b;
        t = R*t;
        verts.push_back(b.x());
        verts.push_back(b.y());
        verts.push_back(0);
        verts.push_back(b.x());
        verts.push_back(b.y());
        verts.push_back(b_height);
        verts.push_back(t.x());
        verts.push_back(t.y());
        verts.push_back(b_height);

        tris.push_back(off+1);
        tris.push_back(off+4);
        tris.push_back(off+2);
        tris.push_back(off+2);
        tris.push_back(off+4);
        tris.push_back(off+5);
        tris.push_back(off+3);
        tris.push_back(off+6);
        tris.push_back(0);
        off+=3;

        normals.push_back(b.x());
        normals.push_back(b.y());
        normals.push_back(0);
        normals.push_back(b.x());
        normals.push_back(b.y());
        normals.push_back(0);
        normals.push_back(t.x());
        normals.push_back(t.y());
        normals.push_back(0);
    }
    tris.push_back(off+1);
    tris.push_back(1);
    tris.push_back(off+2);
    tris.push_back(off+2);
    tris.push_back(1);
    tris.push_back(2);
    tris.push_back(off+3);
    tris.push_back(3);
    tris.push_back(0);
}

}
