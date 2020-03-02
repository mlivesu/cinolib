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
#include <cinolib/geometry/mat.h>
#include <iostream>
#include <assert.h>

namespace cinolib
{

template<class T, uint r, uint c>
CINO_INLINE
std::ostream & operator<<(std::ostream & in, const mat<T,r,c> & m)
{
    in << "[";
    for(uint i=0; i<r; ++i)
    {
        for(uint j=0; j<c; ++j)
        {
            in << m[i][j] << " ";
        }
        in << "\n";
    }
    in << "]";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c>::mat(const T s)
{
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        val[i][j] = s;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c>::mat(const std::initializer_list<T> & il)
{
    assert(il.size()==r*c);
    auto it = il.begin();
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        val[i][j] = *it;
        ++it;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
const T & mat<T,r,c>::at(const uint _r, const uint _c) const
{
    assert(_r<r && _c<c);
    return val[r][c];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
T & mat<T,r,c>::at(const uint _r, const uint _c)
{
    assert(_r<r && _c<c);
    return val[r][c];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
const T * mat<T,r,c>::ptr() const
{
    return val;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> mat<T,r,c>::operator+(const mat<T,r,c> & m) const
{
    mat<T,r,c> res;
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        res.val[i][j] = val[i][j] + m.val[i][j];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> mat<T,r,c>::operator-(const mat<T,r,c> & m) const
{
    mat<T,r,c> res;
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        res.val[i][j] = val[i][j] - m.val[i][j];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> mat<T,r,c>::operator-() const
{
    mat<T,r,c> res;
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        res.val[i][j] = -val[i][j];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> mat<T,r,c>::operator*(const T s) const
{
    mat<T,r,c> res;
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        res.val[i][j] = val[i][j] * s;
    }
    return res;
}











//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> mat<T,r,c>::operator/(const T s) const
{
    mat<T,r,c> res;
    for(uint i=0; i<d; ++i) res.val[i] = val[i] / s;
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> & mat<T,r,c>::operator+=(const mat<T,r,c> & m)
{
    for(uint i=0; i<d; ++i) val[i] += m.val[i];
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> & mat<T,r,c>::operator-=(const mat<T,r,c> & m)
{
    for(uint i=0; i<d; ++i) val[i] -= m.val[i];
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> & mat<T,r,c>::operator*=(const T s)
{
    for(uint i=0; i<d; ++i) val[i] *= s;
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
mat<T,r,c> & mat<T,r,c>::operator/=(const T s)
{
    for(uint i=0; i<d; ++i) val[i] /= s;
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
bool mat<T,r,c>::operator==(const mat<T,r,c> & m) const
{
    for(uint i=0; i<d; ++i)
    {
        if(val[i]!=m.val[i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
bool mat<T,r,c>::operator<(const mat<T,r,c> & m) const
{
    for(uint i=0; i<d; ++i)
    {
        if(val[i]<m.val[i]) return true;
        if(val[i]>m.val[i]) return false;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
bool mat<T,r,c>::is_null() const
{
    for(uint i=0; i<d; ++i)
    {
        if(val[i]!=0) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
bool mat<T,r,c>::is_nan() const
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isnan(val[i])) return true;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
bool mat<T,r,c>::is_inf() const
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isinf(val[i])) return true;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
bool mat<T,r,c>::is_degenerate() const
{
    return is_null() || is_nan() || is_inf();
}

}
