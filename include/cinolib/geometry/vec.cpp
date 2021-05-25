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
#include <cinolib/geometry/vec.h>
#include <cinolib/geometry/vec_utils.h>
#include <iostream>

namespace cinolib
{

template<uint d, class T>
CINO_INLINE
std::ostream & operator<<(std::ostream & in, const vec<d,T> & v)
{
    for(uint i=0; i<d; ++i) in << v[i] << " ";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T>::vec() : vec_mat<d,1,T>()
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const T & scalar) : vec_mat<d,1,T>(scalar)
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const std::initializer_list<T> & il)
{
    assert(il.size()==d);
    auto it = il.begin();
    for(uint i=0; i<d; ++i,++it) this->ptr_vec()[i] = *it;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const T & v0, const T & v1)
{
    this->ptr_vec()[0] = v0;
    this->ptr_vec()[1] = v1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const T & v0, const T & v1, const T & v2)
{
    this->ptr_vec()[0] = v0;
    this->ptr_vec()[1] = v1;
    this->ptr_vec()[2] = v2;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const T & v0, const T & v1, const T & v2, const T & v3)
{
    this->ptr_vec()[0] = v0;
    this->ptr_vec()[1] = v1;
    this->ptr_vec()[2] = v2;
    this->ptr_vec()[3] = v3;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
double vec<d,T>::length() const
{
    return vec_length<d,T>(ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::length_sqrd() const
{
    return vec_length_sqrd<d,T>(this->ptr_vec());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
double vec<d,T>::dist(const vec<d,T> & v) const
{
    return vec_dist<d,T>(this->ptr_vec(), v.ptr_vec());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::dist_sqrd(const vec<d,T> & v) const
{
    return vec_dist_sqrd<d,T>(this->ptr_vec(), v.ptr_vec());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::normalize()
{
    return vec_normalize<d,T>(this->ptr_vec());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::: STATIC OPERATORS (FOR MORE READABLE CODE) :::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T dot(const vec<d,T> & v0, const vec<d,T> & v1)
{
    return vec_dot<d,T>(v0.ptr_vec(), v1.ptr_vec());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
vec<3,T> cross(const vec<3,T> & v0, const vec<3,T> & v1)
{
    vec<3,T> res;
    vec_cross<T>(v0.ptr_vec(), v1.ptr_vec(), res.ptr_vec());
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T angle_deg(const vec<d,T> & v0, const vec<d,T> & v1, const bool unit_length)
{
    return vec_angle_deg<d,T>(v0.ptr_vec(), v1.ptr_vec(), unit_length);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T angle_rad(const vec<d,T> & v0, const vec<d,T> & v1, const bool unit_length)
{
    return vec_angle_rad<d,T>(v0.ptr_vec(), v1.ptr_vec(), unit_length);
}

}
