/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
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
#include <cinolib/geometry/vec_mat.h>
#include <cinolib/geometry/vec_mat_utils.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const mat<r,c,T> & op)
{
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        if(i>0 && c>1 && j%c==0) std::cout << "\n";
        std::cout << op(i,j) << " ";
    }
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T>::mat(const std::initializer_list<T> & il)
{
    mat_set<r,c,T>(_mat, il);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T>::mat(const T scalar)
{
    vec_set_dense<r*c,T>(_vec, scalar);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::ZERO()
{
    return mat<r,c,T>(0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::DIAG(const T scalar)
{
    assert(r==c);
    mat<r,c,T> m;
    mat_set_diag<r,T>(m._mat, scalar);
    return m;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::DIAG(const mat<r,1,T> & diag)
{
    assert(r==c);
    mat<r,c,T> m;
    mat_set_diag<r,c,T>(m._mat, diag._vec);
    return m;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::ROT_2D(const T angle_rad)
{
    assert(r==c);
    mat<r,c,T> m;
    mat_set_rot_2d<r,T>(m._mat, angle_rad);
    return m;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::ROT_3D(const mat<r,1,T> & axis, const T angle_rad)
{
    assert(r==c);
    mat<r,c,T> m;
    mat_set_rot_3d<r,T>(m._mat, angle_rad, axis._vec);
    return m;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::TRANS(const mat<r,1,T> & tx)
{
    assert(r==c);
    mat<r,c,T> m;
    mat_set_trans<r,T>(m._mat, tx._vec);
    return m;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
const T & mat<r,c,T>::operator()(const uint i, const uint j) const
{
    return _mat[i][j];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T & mat<r,c,T>::operator()(const uint i, const uint j)
{
    return _mat[i][j];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}
