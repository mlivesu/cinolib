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
#include <cinolib/geometry/n_sided_poygon.h>
#include <cinolib/pi.h>

namespace cinolib
{

CINO_INLINE
std::vector<vec3d> n_sided_polygon(const uint n,
                                   const uint type)
{
    std::vector<vec3d> verts(n);

    switch(type)
    {
        case CIRCLE:
        {
            assert(n>=3);
            verts[0] = vec3d(1,0,0);
            for(uint i=1; i<n; ++i)
            {
                verts[i] = verts[i-1];
                verts[i].rotate(vec3d(0,0,1), 2.0*M_PI/double(n));
            }
            break;
        }

        case SQUARE:
        {
            assert(n>=4);
            uint a = 0;
            uint b = n/4;   assert(a<b);
            uint c = 2*n/4; assert(b<c);
            uint d = 3*n/4; assert(c<d);
            verts.at(a) = vec3d( 1, 1, 0);
            verts.at(b) = vec3d(-1, 1, 0);
            verts.at(c) = vec3d(-1,-1, 0);
            verts.at(d) = vec3d( 1,-1, 0);
            for(uint i=a+1; i<b; ++i)
            {
                double t = (double)(i-a)/(double)(b-a);
                verts.at(i) = (1-t)*verts.at(a) + t*verts.at(b);
            }
            for(uint i=b+1; i<c; ++i)
            {
                double t = (double)(i-b)/(double)(c-b);
                verts.at(i) = (1-t)*verts.at(b) + t*verts.at(c);
            }
            for(uint i=c+1; i<d; ++i)
            {
                double t = (double)(i-c)/(double)(d-c);
                verts.at(i) = (1-t)*verts.at(c) + t*verts.at(d);
            }
            for(uint i=d+1; i<n; ++i)
            {
                double t = (double)(i-d)/(double)(n-d);
                verts.at(i) = (1-t)*verts.at(d) + t*verts.at(a);
            }
            break;
        }

        case STAR:
        {
            assert(n>=4);// && n%2==0);
            verts[0] = vec3d(1,0,0);
            for(uint i=1; i<n; ++i)
            {
                verts[i] = verts[i-1];
                verts[i].rotate(vec3d(0,0,1), 2.0*M_PI/double(n));
            }
            for(uint i=1; i<n; i+=2) verts[i] -= 0.5*verts[i];
            break;
        }

        default: assert(false && "Unknown polygon type");
    }

    return verts;
}

}
