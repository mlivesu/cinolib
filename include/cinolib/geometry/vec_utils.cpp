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
#include <cinolib/geometry/vec_utils.h>
#include <cinolib/clamp.h>
#include <cinolib/deg_rad.h>
#include <limits>

namespace cinolib
{

namespace vec
{

// c = a + b
template<uint D, typename T>
CINO_INLINE
void plus(const T *a,
          const T *b,
                T *c)
{
    for(uint i=0; i<D; ++i)
    {
        c[i] = a[i] + b[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a - b
template<uint D, typename T>
CINO_INLINE
void minus(const T * a,
           const T * b,
                 T * c)
{
    for(uint i=0; i<D; ++i)
    {
        c[i] = a[i] - b[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a = -a
template<uint D, typename T>
CINO_INLINE
void flip_sign(const T *a)
{
    for(uint i=0; i<D; ++i)
    {
        a[i] = -a[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a * b
template<uint D, typename T>
CINO_INLINE
void scale(const T *a,
           const T  b,
                 T *c)
{
    for(uint i=0; i<D; ++i)
    {
        c[i] = a[i] * b;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a / b
template<uint D, typename T>
CINO_INLINE
void divide(const T *a,
            const T  b,
                  T *c)
{
    for(uint i=0; i<D; ++i)
    {
        c[i] = a[i] / b;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
bool equals(const T *a,
            const T *b)
{
    for(uint i=0; i<D; ++i)
    {
        if(a[i] != b[i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
bool less(const T *a,
          const T *b)
{
    for(uint i=0; i<D; ++i)
    {
        if(a[i] < b[i]) return true;
        if(a[i] > b[i]) return false;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
T dot(const T *a,
      const T *b)
{
    T res = 0;
    for(uint i=0; i<D; ++i)
    {
        res += a[i] * b[i];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a x b (cross product)
template<uint D, typename T>
CINO_INLINE
void cross(const T *a,
           const T *b,
           const T *c)
{
    assert(D==3);
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
double length(const T *a)
{
    return sqrt(length_sqrd(a));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
double length_sqrd(const T *a)
{
    double len = 0;
    for(uint i=0; i<D; ++i)
    {
        len += a[i] * a[i];
    }
    return len;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
double dist(const T *a,
            const T *b)
{
    return sqrt(dist_sqrd(a,b));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
double dist_sqrd(const T *a,
                 const T *b)
{
    T tmp[D];
    minus(a,b,tmp);
    return length_sqrd(tmp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
double normalize(T *a)
{
    double len = length(a);
    divide(a, len, a);
    return len;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void swap(T *a, const uint i, const uint j)
{
    assert(i<D && j<D);
    std::swap(a[i],a[j]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
T min_entry(const T *a)
{
    return *std::min_element(a, a+D);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
T max_entry(const T *a)
{
    return *std::max_element(a, a+D);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void set_ZERO(T *a)
{
    for(uint i=0; i<D; ++i)
    {
        a[i] = 0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void set_MIN(T *a)
{
    for(uint i=0; i<D; ++i)
    {
        a[i] = std::numeric_limits<T>::min();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void set_MAX(T *a)
{
    for(uint i=0; i<D; ++i)
    {
        a[i] = std::numeric_limits<T>::max();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void set_INF(T *a)
{
    for(uint i=0; i<D; ++i)
    {
        a[i] = std::numeric_limits<T>::infinity();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void min(const T *a,
         const T *b,
               T *c)
{
    for(uint i=0; i<D; ++i)
    {
        c[i] = std::min(a[i],b[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
T max(const T *a,
      const T *b,
            T *c)
{
    for(uint i=0; i<D; ++i)
    {
        c[i] = std::max(a[i],b[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void clamp(const T *a,
           const T  min,
           const T  max)
{
    for(uint i=0; i<D; ++i)
    {
        a[i] = clamp(a[i], min, max);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
double angle_deg(const T *a,
                 const T *b)
{
    return to_deg(angle_rad(a,b));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
double angle_rad(const T *a,
                 const T *b)
{
    T tmp_a[D], tmp_b[D];
    copy(a, tmp_a);
    copy(b, tmp_b);
    normalize(tmp_a);
    normalize(tmp_b);
    if(is_degenerate(tmp_a) || is_degenerate(tmp_b))
    {
        return std::numeric_limits<double>::infinity();
    }
    return acos(clamp(dot(tmp_a, tmp_b), -1, 1));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
bool is_null(const T *a)
{
    for(uint i=0; i<D; ++i)
    {
        if(a[i]!=0) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
bool has_nan(const T *a)
{
    for(uint i=0; i<D; ++i)
    {
        if(std::isnan(a[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
bool has_inf(const T *a)
{
    for(uint i=0; i<D; ++i)
    {
        if(std::isinf(a[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
bool is_degenerate(const T *a)
{
    for(uint i=0; i<D; ++i)
    {
        if(!std::isnormal(a[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void copy(const T *a,
                T *b)
{
    std::copy(a, a+D, b);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint D, typename T>
CINO_INLINE
void print(const T* a)
{
    std::cout << "[";
    for(uint i=0; i<D-1; ++i)
    {
        std::cout << a[i] << ", ";
    }
    std::cout << a[D-1] << "]" << std::endl;
}

}

}
