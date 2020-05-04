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
#include <cinolib/winding_number.h>
#include <cinolib/solid_angle.h>

namespace cinolib
{

CINO_INLINE
int winding_number(const std::vector<vec3d> & verts,
                   const std::vector<uint>  & tris,
                   const vec3d              & p)
{
    double w = 0;
    for(uint i=0; i<tris.size(); i+=3)
    {
        w += solid_angle(verts.at(tris.at( i )),
                         verts.at(tris.at(i+1)),
                         verts.at(tris.at(i+2)),
                         p);
    }
    return static_cast<int>(round(w));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int winding_number(const AbstractPolygonMesh<M,V,E,P> & m,
                   const vec3d                        & p)
{
    double w = 0;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        const auto & tris = m.poly_tessellation(pid);
        for(uint i=0; i<tris.size(); i+=3)
        {
            w += solid_angle(m.vert(tris.at( i )),
                             m.vert(tris.at(i+1)),
                             m.vert(tris.at(i+2)),
                             p);
        }
    }
    return static_cast<int>(round(w));
}

}

