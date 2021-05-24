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
#include <cinolib/geometry/mat.h>
#include <cinolib/geometry/mat_utils.h>
#include <iostream>

namespace cinolib
{

template<uint r, uint c, class T>
CINO_INLINE
std::ostream & operator<<(std::ostream & in, const mat<r,c,T> & v)
{
    in << "[";
    for(uint i=0; i<r*c; ++i)
    {
        in << v[i] << " ";
        if(i%c==0) in << "\n";
    }
    in << "]";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T>::mat(const T & s)
{
    for(uint i=0; i<r*c; ++i)
    {
        data[i] = s;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T>::mat(const std::initializer_list<T> & il)
{
    assert(il.size()==r*c);
    auto it = il.begin();
    for(uint i=0; i<r*c; ++i,++it) data[i] = *it;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T & mat<r,c,T>::operator[](const uint pos)
{
    assert(pos<r*c);
    return data[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
const T & mat<r,c,T>::operator[](const uint pos) const
{
    assert(pos<r*c);
    return data[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::operator+(const mat<r,c,T> & v) const
{
    mat<r,c,T> res;
    mat_plus<r,c,T>(data, v.data, res.data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::operator-(const mat<r,c,T> & v) const
{
    mat<r,c,T> res;
    mat_minus<r,c,T>(data, v.data, res.data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::operator-() const
{
    mat<r,c,T> res;
    //mat_flip_sign<r,c,T>(data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::operator*(const T & s) const
{
    mat<r,c,T> res;
    mat_multiply<r,c,T>(data, s, res.data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::operator/(const T & s) const
{
    mat<r,c,T> res;
    mat_divide<r,c,T>(data, s, res.data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> & mat<r,c,T>::operator+=(const mat<r,c,T> & v)
{
    mat_plus<r,c,T>(data, v.data);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> & mat<r,c,T>::operator-=(const mat<r,c,T> & v)
{
    mat_minus<r,c,T>(data, v.data);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> & mat<r,c,T>::operator*=(const T & s)
{
    mat_multiply<r,c,T>(data, s);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> & mat<r,c,T>::operator/=(const T & s)
{
    mat_divide<r,c,T>(data, s);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool mat<r,c,T>::operator==(const mat<r,c,T> & v) const
{
    return true;//mat_equals<r,c,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool mat<r,c,T>::operator<(const mat<r,c,T> & v) const
{
    return false;//mat_less<r,c,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
double mat<r,c,T>::length() const
{
    return 0;//mat_length<r,c,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T mat<r,c,T>::length_sqrd() const
{
    return 0;//mat_length_sqrd<r,c,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
double mat<r,c,T>::dist(const mat<r,c,T> & v) const
{
    return 0;//mat_dist<r,c,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T mat<r,c,T>::dist_sqrd(const mat<r,c,T> & v) const
{
    return 0;//mat_dist_sqrd<r,c,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T mat<r,c,T>::normalize()
{
    return 0;//mat_normalize<r,c,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T mat<r,c,T>::min_entry() const
{
    return 0;//mat_min_entry<r,c,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T mat<r,c,T>::max_entry() const
{
    return 0;//mat_max_entry<r,c,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::min(const mat<r,c,T> & v) const
{
    mat<r,c,T> res;
    //mat_min<r,c,T>(data, v.data, res.data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> mat<r,c,T>::max(const mat<r,c,T> & v) const
{
    mat<r,c,T> res;
    //mat_max<r,c,T>(data, v.data, res.data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool mat<r,c,T>::is_null() const
{
    return 0;//mat_is_null<r,c,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool mat<r,c,T>::is_nan() const
{
    return 0;//mat_is_nan<r,c,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool mat<r,c,T>::is_inf() const
{
    return 0;//mat_is_inf<r,c,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool mat<r,c,T>::is_degenerate() const
{
    return is_null() || is_nan() || is_inf();
}

}
