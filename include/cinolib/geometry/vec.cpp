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
#include <cinolib/geometry/vec.h>
#include <cinolib/pi.h>
#include <cmath>
#include <iostream>
#include <assert.h>

namespace cinolib
{

template<class T, uint d>
CINO_INLINE
std::ostream & operator<<(std::ostream & in, const vec<T,d> & v)
{
    in << "[";
    for(uint i=0; i<d; ++i) in << v[i] << " ";
    in << "]";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d>::vec(const T s)
{
    for(uint i=0; i<d; ++i) val[i] = s;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d>::vec(const std::initializer_list<T> & il)
{
    assert(il.size()==d);
    auto it = il.begin();
    for(uint i=0; i<d; ++i,++it) at(i) = *it;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// template specialization for performance speedup for ubiquitous types
template<>
CINO_INLINE
vec<double,2>::vec(const double v0, const double v1)
{
    val[0] = v0;
    val[1] = v1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// template specialization for performance speedup for ubiquitous types
template<>
CINO_INLINE
vec<double,3>::vec(const double v0, const double v1, const double v2)
{
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
const T & vec<T,d>::at(const uint off) const
{
    assert(off<d);
    return val[off];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T & vec<T,d>::at(const uint off)
{
    assert(off<d);
    return val[off];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
const T * vec<T,d>::ptr() const
{
    return val;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::dot(const vec<T,d> & v) const
{
    T res = 0;
    for(uint i=0; i<d; ++i)
    {
        res += this->val[i] * v[i];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// template specialization for performance speedup for ubiquitous types
template<>
CINO_INLINE
double vec<double,2>::dot(const vec<double,2> & v) const
{
    return val[0] * v.val[0] +
           val[1] * v.val[1];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// template specialization for performance speedup for ubiquitous types
template<>
CINO_INLINE
double vec<double,3>::dot(const vec<double,3> & v) const
{
    return val[0] * v.val[0] +
           val[1] * v.val[1] +
           val[2] * v.val[2];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// template specialization for performance speedup for ubiquitous types
template<>
CINO_INLINE
vec<double,3> vec<double,3>::cross(const vec<double,3> & v) const
{
    return vec<double,3>(val[1] * v.val[2] - val[2] * v.val[1],
                         val[2] * v.val[0] - val[0] * v.val[2],
                         val[0] * v.val[1] - val[1] * v.val[0]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::cross(const vec<T,d> & v) const
{
    assert(d==3 && "cross product is defined only in R^3 so far");

    return vec<T,d>({val[1] * v.val[2] - val[2] * v.val[1],
                     val[2] * v.val[0] - val[0] * v.val[2],
                     val[0] * v.val[1] - val[1] * v.val[0]});
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T & vec<T,d>::operator[](const uint off)
{
    return val[off];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
const T & vec<T,d>::operator[](const uint off) const
{
    return val[off];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}
