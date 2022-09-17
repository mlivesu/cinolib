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
#include <cinolib/serialize_index.h>
#include <utility>

namespace cinolib
{

CINO_INLINE
uint serialize_2D_index(const uint i, const uint j, const uint nj)
{
    return i*nj + j;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec2i deserialize_2D_index(const uint ij, const uint nj)
{
    return vec2i(ij/nj, ij%nj);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint serialize_3D_index(const uint i, const uint j, const uint k, const uint nj, const uint nk)
{
    return i*nk*nj + j*nk + k;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3u deserialize_3D_index(const uint ijk, const uint nj, const uint nk)
{
    uint njk = nj*nk;
    uint i   = ijk/njk;
    uint jk  = ijk%njk;
    uint j   = jk/nk;
    uint k   = jk%nk;
    return vec3u(i,j,k);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint N, class GridPoint>
CINO_INLINE
uint serialize_nD_index(const std::array<uint,N> & dim_extent, const GridPoint & p)
{
    uint index=0;
    if(p[N-1]>=0)
    {
       index=(uint)p[N-1];
       if(index>=dim_extent[N-1]) index=dim_extent[N-1]-1;
    }
    for(uint i=N-1; i>0; --i)
    {
       index*=dim_extent[i-1];
       if(p[i-1]>=0)
       {
          uint j= (uint)p[i-1];
          if(j>=dim_extent[i-1]) j=dim_extent[i-1]-1;
          index+=j;
       }
    }
    return index;
}

}
