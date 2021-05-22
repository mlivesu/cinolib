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
    in << "[";
    for(uint i=0; i<d; ++i) in << v[i] << " ";
    in << "]";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const T & s)
{
    for(uint i=0; i<d; ++i) data[i] = s;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const std::initializer_list<T> & il)
{
    assert(il.size()==d);
    auto it = il.begin();
    for(uint i=0; i<d; ++i,++it) data[i] = *it;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::dot(const vec<d,T> & v) const
{
    return vec_dot<d,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> vec<d,T>::cross(const vec<d,T> & v) const
{
    vec<d,T> res;
    vec_cross<d,T>(data, v.data, res.data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T & vec<d,T>::operator[](const uint pos)
{
    return data[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
const T & vec<d,T>::operator[](const uint pos) const
{
    return data[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> vec<d,T>::operator+(const vec<d,T> & v) const
{
    vec<d,T> res;
    vec_plus<d,T>(data, v.data, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> vec<d,T>::operator-(const vec<d,T> & v) const
{
    vec<d,T> res;
    vec_minus<d,T>(data, v.data, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> vec<d,T>::operator-() const
{
    vec<d,T> res;
    vec_flip_sign<d,T>(data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> vec<d,T>::operator*(const T & s) const
{
    vec<d,T> res;
    vec_multiply<d,T>(data, s, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> vec<d,T>::operator/(const T & s) const
{
    vec<d,T> res;
    vec_divide<d,T>(data, s, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> & vec<d,T>::operator+=(const vec<d,T> & v)
{
    vec_plus<d,T>(data, v.data);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> & vec<d,T>::operator-=(const vec<d,T> & v)
{
    vec_minus<d,T>(data, v.data);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> & vec<d,T>::operator*=(const T & s)
{
    vec_multiply<d,T>(data, s);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> & vec<d,T>::operator/=(const T & s)
{
    vec_divide<d,T>(data, s);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
bool vec<d,T>::operator==(const vec<d,T> & v) const
{
    return vec_equals<d,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
bool vec<d,T>::operator<(const vec<d,T> & v) const
{
    return vec_less<d,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
double vec<d,T>::length() const
{
    return vec_length<d,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::length_sqrd() const
{
    return vec_length_sqrd<d,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
double vec<d,T>::dist(const vec<d,T> & v) const
{
    return vec_dist<d,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::dist_sqrd(const vec<d,T> & v) const
{
    return vec_dist_sqrd<d,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::normalize()
{
    return vec_normalize<d,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::min_entry() const
{
    return vec_min_entry<d,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::max_entry() const
{
    return vec_max_entry<d,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> vec<d,T>::min(const vec<d,T> & v) const
{
    vec<d,T> res;
    vec_min<d,T>(data, v.data, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
vec<d,T> vec<d,T>::max(const vec<d,T> & v) const
{
    vec<d,T> res;
    vec_max<d,T>(data, v.data, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::angle_rad(const vec<d,T> & v) const
{
    return vec_angle_rad<d,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
T vec<d,T>::angle_deg(const vec<d,T> & v) const
{
    return vec_angle_deg<d,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
bool vec<d,T>::is_null() const
{
    return vec_is_null<d,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
bool vec<d,T>::is_nan() const
{
    return vec_is_nan<d,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
bool vec<d,T>::is_inf() const
{
    return vec_is_inf<d,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, class T>
CINO_INLINE
bool vec<d,T>::is_degenerate() const
{
    return is_null() || is_nan() || is_inf();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::     Template specializatios to obtain a performance boost on       ::
//::    the most ubiquitous vec types in the library (mainly vec3d)     ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// faster vec2d constructor
template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const T & v0, const T & v1)
{
    data[0] = v0;
    data[1] = v1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// faster vec3d constructor
template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const T & v0, const T & v1, const T & v2)
{
    data[0] = v0;
    data[1] = v1;
    data[2] = v2;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// faster vec4d constructor
template<uint d, class T>
CINO_INLINE
vec<d,T>::vec(const T & v0, const T & v1, const T & v2, const T & v3)
{
    data[0] = v0;
    data[1] = v1;
    data[2] = v2;
    data[3] = v3;
}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<>
//CINO_INLINE
//double vec<double,2>::dot(const vec<double,2> & v) const
//{
//    return val[0] * v.val[0] +
//           val[1] * v.val[1];
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<>
//CINO_INLINE
//double vec<double,3>::dot(const vec<double,3> & v) const
//{
//    return val[0] * v.val[0] +
//           val[1] * v.val[1] +
//           val[2] * v.val[2];
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<>
//CINO_INLINE
//vec<double,3> vec<double,3>::cross(const vec<double,3> & v) const
//{
//    return vec<double,3>(val[1] * v.val[2] - val[2] * v.val[1],
//                         val[2] * v.val[0] - val[0] * v.val[2],
//                         val[0] * v.val[1] - val[1] * v.val[0]);
//}

}
