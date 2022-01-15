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
#include <cinolib/icosphere.h>
#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

template<class T>
CINO_INLINE
void icosphere(const T             radius, // sphere radius
               const uint          n_subd, // number of subdivisions of the icosahedron
               std::vector<T>    & verts,  // output vertices
               std::vector<uint> & tris)   // output triangles
{
    T A = 0.5257310271 * radius;
    T B = 0.8506510258 * radius;

    verts =
    {
        -A,  0, -B,
         A,  0, -B,
         A,  0,  B,
        -A,  0,  B,
        -B, -A,  0,
        -B,  A,  0,
         B,  A,  0,
         B, -A,  0,
         0, -B,  A,
         0, -B, -A,
         0,  B, -A,
         0,  B,  A
    };

    // triangulation of the Icosahedron
    tris =
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

    for(uint i=0; i<n_subd; ++i)
    {
        std::vector<uint> tmp_triangles;
        for(auto it=tris.begin(); it!=tris.end(); ++it)
        {
            uint vid0 = *it; ++it;
            uint vid1 = *it; ++it;
            uint vid2 = *it;

            mat<3,1,T> v0(&(verts[vid0*3]));
            mat<3,1,T> v1(&(verts[vid1*3]));
            mat<3,1,T> v2(&(verts[vid2*3]));

            // split edges and project midpoints to the sphere
            mat<3,1,T> v01 = v0 + v1;
            mat<3,1,T> v12 = v1 + v2;
            mat<3,1,T> v20 = v2 + v0;
            v01 *= radius / v01.norm();
            v12 *= radius / v12.norm();
            v20 *= radius / v20.norm();

            // add three new vertices to the list
            uint vid01 = verts.size()/3.0;
            uint vid12 = vid01 + 1;
            uint vid20 = vid01 + 2;
            verts.push_back(v01.x()); // v01
            verts.push_back(v01.y());
            verts.push_back(v01.z());
            verts.push_back(v12.x()); // v12
            verts.push_back(v12.y());
            verts.push_back(v12.z());
            verts.push_back(v20.x()); // v20
            verts.push_back(v20.y());
            verts.push_back(v20.z());

            // add four new triangles to the list
            tmp_triangles.push_back(vid0 ); // t0
            tmp_triangles.push_back(vid01);
            tmp_triangles.push_back(vid20);
            tmp_triangles.push_back(vid01); // t1
            tmp_triangles.push_back(vid1 );
            tmp_triangles.push_back(vid12);
            tmp_triangles.push_back(vid01); // t2
            tmp_triangles.push_back(vid12);
            tmp_triangles.push_back(vid20);
            tmp_triangles.push_back(vid20); // t3
            tmp_triangles.push_back(vid12);
            tmp_triangles.push_back(vid2 );
        }
        tris = tmp_triangles;
    }
}

}
