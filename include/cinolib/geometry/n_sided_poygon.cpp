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
std::vector<vec3d> n_sided_polygon(const vec3d & center,
                                   const uint    n_sides,
                                   const double  radius,
                                   const int     axis)
{
    std::vector<vec3d> verts(n_sides);

    switch (axis)
    {
        case X : verts[0] = vec3d(0,0,radius); break;
        case Y : verts[0] = vec3d(radius,0,0); break;
        case Z : verts[0] = vec3d(radius,0,0); break;
        default: assert(false);
    }

    for(uint i=1; i<n_sides; ++i)
    {
        verts[i] = verts[i-1];
        switch (axis)
        {
            case X : rotate(verts[i], vec3d(1,0,0), 2.0*M_PI/double(n_sides)); break;
            case Y : rotate(verts[i], vec3d(0,1,0), 2.0*M_PI/double(n_sides)); break;
            case Z : rotate(verts[i], vec3d(0,0,1), 2.0*M_PI/double(n_sides)); break;
            default: assert(false);
        }
    }
    for(vec3d & v : verts) v += center;
    return verts;
}

}
