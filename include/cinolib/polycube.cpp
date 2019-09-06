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
#include <cinolib/polycube.h>
#include <cinolib/serialize_index.h>
#include <cinolib/extrude_mesh.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void polycube(Trimesh<M,V,E,P> & pc, const uint genus)
{
    pc = Trimesh<M,V,E,P>();

    uint nrows = 3;
    uint ncols = 1 + 2*genus;

    for(uint r=0; r<=nrows; ++r)
    for(uint c=0; c<=ncols; ++c)
    {
        pc.vert_add(vec3d(c,r,0));
    }

    for(uint r=0; r<=nrows; ++r)
    for(uint c=0; c<=ncols; ++c)
    {
        if(r<nrows && c<ncols)
        {
            if(r==1 && c%2!=0) continue; // handle

            pc.poly_add({serialize_2D_index(r  , c,   ncols+1),
                         serialize_2D_index(r  , c+1, ncols+1),
                         serialize_2D_index(r+1, c+1, ncols+1)});

            pc.poly_add({serialize_2D_index(r  , c,   ncols+1),
                         serialize_2D_index(r+1, c+1, ncols+1),
                         serialize_2D_index(r+1, c,   ncols+1)});
        }
    }

    extrude_mesh(pc, vec3d(0,0,1));
}

}
