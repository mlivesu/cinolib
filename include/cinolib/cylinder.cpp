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
#include <cinolib/cylinder.h>
#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

template<class T>
CINO_INLINE
void cylinder(const T             height,
              const T             bot_radius,
              const T             top_radius,
              const uint          n_sides,    // cross section
              std::vector<T>    & verts,      // output vertices
              std::vector<uint> & tris,       // output triangles
              std::vector<T>    & normals)    // output normals (per vertex)
{
    // add first two points
    verts   = { bot_radius, 0,      0,
                top_radius, 0, height };
    normals = { 1, 0, 0,
                1, 0, 0 };
    tris    = {};

    // add remaining points and triangles
    mat<2,1,T> b(bot_radius,0);
    mat<2,1,T> t(top_radius,0);
    mat<2,2,T> R = mat<2,2,T>::ROT_2D(2*M_PI/n_sides);
    uint off = 0;
    for(uint i=1; i<n_sides; ++i)
    {
        b = R*b;
        verts.push_back(b.x());
        verts.push_back(b.y());
        verts.push_back(0);
        t = R*t;
        verts.push_back(t.x());
        verts.push_back(t.y());
        verts.push_back(height);

        tris.push_back(off);
        tris.push_back(off+2);
        tris.push_back(off+1);
        tris.push_back(off+1);
        tris.push_back(off+2);
        tris.push_back(off+3);
        off+=2;

        normals.push_back(b.x());
        normals.push_back(b.y());
        normals.push_back(0);
        normals.push_back(t.x());
        normals.push_back(t.y());
        normals.push_back(0);
    }
    tris.push_back(off);
    tris.push_back(0);
    tris.push_back(off+1);
    tris.push_back(off+1);
    tris.push_back(0);
    tris.push_back(1);
}

}
