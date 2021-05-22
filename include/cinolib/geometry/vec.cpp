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
vec<T,d>::vec(const T & s)
{
    for(uint i=0; i<d; ++i) data[i] = s;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d>::vec(const std::initializer_list<T> & il)
{
    assert(il.size()==d);
    auto it = il.begin();
    for(uint i=0; i<d; ++i,++it) data[i] = *it;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::dot(const vec<T,d> & v) const
{
    return vec_dot<3,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::cross(const vec<T,d> & v) const
{
    vec<T,d> res;
    vec_cross<3,T>(data, v.data, res.data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T & vec<T,d>::operator[](const uint pos)
{
    return data[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
const T & vec<T,d>::operator[](const uint pos) const
{
    return data[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator+(const vec<T,d> & v) const
{
    vec<T,d> res;
    vec_plus<3,T>(data, v.data, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator-(const vec<T,d> & v) const
{
    vec<T,d> res;
    vec_minus<3,T>(data, v.data, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator-() const
{
    vec<T,d> res;
    vec_flip_sign<3,T>(data);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator*(const T & s) const
{
    vec<T,d> res;
    vec_multiply<3,T>(data, s, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator/(const T & s) const
{
    vec<T,d> res;
    vec_divide<3,T>(data, s, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> & vec<T,d>::operator+=(const vec<T,d> & v)
{
    vec_plus<3,T>(data, v.data);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> & vec<T,d>::operator-=(const vec<T,d> & v)
{
    vec_minus<3,T>(data, v.data);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> & vec<T,d>::operator*=(const T & s)
{
    vec_multiply<3,T>(data, s);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> & vec<T,d>::operator/=(const T & s)
{
    vec_divide<3,T>(data, s);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::operator==(const vec<T,d> & v) const
{
    return vec_equals<3,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::operator<(const vec<T,d> & v) const
{
    return vec_less<3,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
double vec<T,d>::length() const
{
    return vec_length<3,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::length_sqrd() const
{
    return vec_length_sqrd<3,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
double vec<T,d>::dist(const vec<T,d> & v) const
{
    return vec_dist<3,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::dist_sqrd(const vec<T,d> & v) const
{
    return vec_dist_sqrd<3,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::normalize()
{
    return vec_normalize<3,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::min_entry() const
{
    return vec_min_entry<3,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::max_entry() const
{
    return vec_max_entry<3,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::min(const vec<T,d> & v) const
{
    vec<T,d> res;
    vec_min<3,T>(data, v.data, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::max(const vec<T,d> & v) const
{
    vec<T,d> res;
    vec_max<3,T>(data, v.data, res);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::angle_rad(const vec<T,d> & v) const
{
    return vec_angle_rad<3,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::angle_deg(const vec<T,d> & v) const
{
    return vec_angle_deg<3,T>(data, v.data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::is_null() const
{
    return vec_is_null<3,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::is_nan() const
{
    return vec_is_nan<3,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::is_inf() const
{
    return vec_is_inf<3,T>(data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::is_degenerate() const
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
template<class T, uint d>
CINO_INLINE
vec<T,d>::vec(const T & v0, const T & v1)
{
    data[0] = v0;
    data[1] = v1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// faster vec3d constructor
template<class T, uint d>
CINO_INLINE
vec<T,d>::vec(const T & v0, const T & v1, const T & v2)
{
    data[0] = v0;
    data[1] = v1;
    data[2] = v2;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// faster vec4d constructor
template<class T, uint d>
CINO_INLINE
vec<T,d>::vec(const T & v0, const T & v1, const T & v2, const T & v3)
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
