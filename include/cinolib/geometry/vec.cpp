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
#include <cinolib/clamp.h>
#include <cinolib/deg_rad.h>
#include <cinolib/ANSI_color_codes.h>
#include <cmath>
#include <algorithm>
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

template<class T, uint d>
CINO_INLINE
const T & vec<T,d>::at(const uint pos) const
{
    assert(pos<d);
    return val[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T & vec<T,d>::at(const uint pos)
{
    assert(pos<d);
    return val[pos];
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
T & vec<T,d>::operator[](const uint pos)
{
    return val[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
const T & vec<T,d>::operator[](const uint pos) const
{
    return val[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator+(const vec<T,d> & v) const
{
    vec<T,d> res;
    for(uint i=0; i<d; ++i) res.val[i] = val[i] + v.val[i];
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator-(const vec<T,d> & v) const
{
    vec<T,d> res;
    for(uint i=0; i<d; ++i) res.val[i] = val[i] - v.val[i];
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator-() const
{
    vec<T,d> res;
    for(uint i=0; i<d; ++i) res.val[i] = -val[i];
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator*(const T s) const
{
    vec<T,d> res;
    for(uint i=0; i<d; ++i) res.val[i] = val[i] * s;
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::operator/(const T s) const
{
    vec<T,d> res;
    for(uint i=0; i<d; ++i) res.val[i] = val[i] / s;
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> & vec<T,d>::operator+=(const vec<T,d> & v)
{
    for(uint i=0; i<d; ++i) val[i] += v.val[i];
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> & vec<T,d>::operator-=(const vec<T,d> & v)
{
    for(uint i=0; i<d; ++i) val[i] -= v.val[i];
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> & vec<T,d>::operator*=(const T s)
{
    for(uint i=0; i<d; ++i) val[i] *= s;
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> & vec<T,d>::operator/=(const T s)
{
    for(uint i=0; i<d; ++i) val[i] /= s;
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::operator==(const vec<T,d> & v) const
{
    for(uint i=0; i<d; ++i)
    {
        if(val[i]!=v.val[i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::operator<(const vec<T,d> & v) const
{
    for(uint i=0; i<d; ++i)
    {
        if(val[i]<v.val[i]) return true;
        if(val[i]>v.val[i]) return false;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::length() const
{
    return sqrt(length_squared());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::length_squared() const
{
    T res = 0;
    for(uint i=0; i<d; ++i)
    {
        res += val[i]*val[i];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::dist(const vec<T,d> & v) const
{
    return (*this - v).length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::dist_squared(const vec<T,d> & v) const
{
    return (*this - v).length_squared();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::normalize()
{
    T len = length();
    if(len == 0)
    {
        len = 1e-10;
        std::cout << ANSI_fg_color_red << "WARNING: normalization of zero length vector!" << ANSI_fg_color_default << std::endl;
    }
    *this/=len;
    return len;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::min_entry() const
{
    return *std::min_element(val,val+d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::max_entry() const
{
    return *std::max_element(val,val+d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::min(const vec<T,d> & v) const
{
    vec<T,d> res;
    for(uint i=0; i<d; ++i) res.val[i] = std::min(val[i], v.val[i]);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
vec<T,d> vec<T,d>::max(const vec<T,d> & v) const
{
    vec<T,d> res;
    for(uint i=0; i<d; ++i) res.val[i] = std::max(val[i], v.val[i]);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::angle_rad(const vec<T,d> & v) const
{
    // pre normalize vectors
    auto v0 = *this/length();
    auto v1 = v/v.length();
    if(v0.is_degenerate() || v1.is_degenerate()) return std::numeric_limits<T>::infinity();
    return acos(clamp(v0.dot(v1), -1, 1));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
T vec<T,d>::angle_deg(const vec<T,d> & v) const
{
    return to_deg(angle_rad(v));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::is_null() const
{
    for(uint i=0; i<d; ++i)
    {
        if(val[i]!=0) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::is_nan() const
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isnan(val[i])) return true;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
bool vec<T,d>::is_inf() const
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isinf(val[i])) return true;
    }
    return true;
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
template<>
CINO_INLINE
vec<double,2>::vec(const double v0, const double v1)
{
    val[0] = v0;
    val[1] = v1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// faster vec3d constructor
template<>
CINO_INLINE
vec<double,3>::vec(const double v0, const double v1, const double v2)
{
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
double vec<double,2>::dot(const vec<double,2> & v) const
{
    return val[0] * v.val[0] +
           val[1] * v.val[1];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
double vec<double,3>::dot(const vec<double,3> & v) const
{
    return val[0] * v.val[0] +
           val[1] * v.val[1] +
           val[2] * v.val[2];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<>
CINO_INLINE
vec<double,3> vec<double,3>::cross(const vec<double,3> & v) const
{
    return vec<double,3>(val[1] * v.val[2] - val[2] * v.val[1],
                         val[2] * v.val[0] - val[0] * v.val[2],
                         val[0] * v.val[1] - val[1] * v.val[0]);
}

}
