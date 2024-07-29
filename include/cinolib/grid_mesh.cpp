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
#include <cinolib/grid_mesh.h>
#include <cinolib/serialize_index.h>
#include <vector>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void grid_mesh(const uint nx, const uint ny, Quadmesh<M,V,E,P> & m)
{
    std::vector<vec3d> verts;
    std::vector<uint>  polys;
    for(uint x=0; x<=nx; ++x)
    for(uint y=0; y<=ny; ++y)
    {
        verts.push_back(vec3d(x,y,0));

        if(x<nx && y<ny)
        {
            polys.push_back(serialize_2D_index(x  , y,   ny+1));
            polys.push_back(serialize_2D_index(x  , y+1, ny+1));
            polys.push_back(serialize_2D_index(x+1, y+1, ny+1));
            polys.push_back(serialize_2D_index(x+1, y,   ny+1));
        }
    }
    m = Quadmesh<M,V,E,P>(verts, polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void grid_mesh(const uint nx, const uint ny, const uint nz, Hexmesh<M,V,E,F,P> & m)
{
    std::vector<vec3d> verts;
    std::vector<uint>  polys;
    for(uint x=0; x<=nx; ++x)
    for(uint y=0; y<=ny; ++y)
    for(uint z=0; z<=nz; ++z)
    {
        verts.push_back(vec3d(x,y,z));

        if(x<nx && y<ny && z<nz)
        {
            polys.push_back(serialize_3D_index(x  , y,   z  , ny+1, nz+1));
            polys.push_back(serialize_3D_index(x  , y+1, z  , ny+1, nz+1));
            polys.push_back(serialize_3D_index(x+1, y+1, z  , ny+1, nz+1));
            polys.push_back(serialize_3D_index(x+1, y,   z  , ny+1, nz+1));
            polys.push_back(serialize_3D_index(x  , y,   z+1, ny+1, nz+1));
            polys.push_back(serialize_3D_index(x  , y+1, z+1, ny+1, nz+1));
            polys.push_back(serialize_3D_index(x+1, y+1, z+1, ny+1, nz+1));
            polys.push_back(serialize_3D_index(x+1, y,   z+1, ny+1, nz+1));
        }
    }
    m = Hexmesh<M,V,E,F,P>(verts, polys);
}

}
