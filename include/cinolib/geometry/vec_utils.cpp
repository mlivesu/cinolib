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
#include <cinolib/geometry/vec_utils.h>
#include <cinolib/clamp.h>
#include <cinolib/deg_rad.h>
#include <limits>

namespace cinolib
{

// c = a + b
template<uint d, typename T>
CINO_INLINE
void vec_plus(const T * a,
              const T * b,
                    T * c)
{
    for(uint i=0; i<d; ++i)
    {
        c[i] = a[i] + b[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a += b
template<uint d, typename T>
CINO_INLINE
void vec_plus(      T * a,
              const T * b)
{
    for(uint i=0; i<d; ++i)
    {
        a[i] += b[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a - b
template<uint d, typename T>
CINO_INLINE
void vec_minus(const T * a,
               const T * b,
                     T * c)
{
    for(uint i=0; i<d; ++i)
    {
        c[i] = a[i] - b[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a -= b
template<uint d, typename T>
CINO_INLINE
void vec_minus(      T * a,
               const T * b)
{
    for(uint i=0; i<d; ++i)
    {
        a[i] -= b[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a * b
template<uint d, typename T>
CINO_INLINE
void vec_multiply(const T * a,
                  const T & b,
                        T * c)
{
    for(uint i=0; i<d; ++i)
    {
        c[i] = a[i] * b;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a *= b
template<uint d, typename T>
CINO_INLINE
void vec_multiply(      T * a,
                  const T & b)
{
    for(uint i=0; i<d; ++i)
    {
        a[i] *= b;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a / b
template<uint d, typename T>
CINO_INLINE
void vec_divide(const T * a,
                const T & b,
                      T * c)
{
    for(uint i=0; i<d; ++i)
    {
        c[i] = a[i] / b;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a /= b
template<uint d, typename T>
CINO_INLINE
void vec_divide(      T * a,
                const T & b)
{
    for(uint i=0; i<d; ++i)
    {
        a[i] /= b;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a = -a
template<uint d, typename T>
CINO_INLINE
void vec_flip_sign(const T * a)
{
    for(uint i=0; i<d; ++i)
    {
        a[i] = -a[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a == b
template<uint d, typename T>
CINO_INLINE
bool vec_equals(const T * a,
                const T * b)
{
    for(uint i=0; i<d; ++i)
    {
        if(a[i] != b[i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a < b
template<uint d, typename T>
CINO_INLINE
bool vec_less(const T * a,
              const T * b)
{
    for(uint i=0; i<d; ++i)
    {
        if(a[i] < b[i]) return true;
        if(a[i] > b[i]) return false;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_dot(const T * a,
          const T * b)
{
    T res = 0;
    for(uint i=0; i<d; ++i)
    {
        res += a[i] * b[i];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a x b (cross product)
template<uint d, typename T>
CINO_INLINE
void vec_cross(const T * a,
               const T * b,
                     T * c)
{
    assert(d==3);
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
double vec_length(const T * a)
{
    return sqrt(vec_length_sqrd<d,T>(a));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_length_sqrd(const T * a)
{
    T len = 0;
    for(uint i=0; i<d; ++i)
    {
        len += a[i] * a[i];
    }
    return len;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
double vec_dist(const T * a,
                const T * b)
{
    return sqrt(vec_dist_sqrd<d,T>(a,b));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_dist_sqrd(const T * a,
                const T * b)
{
    T tmp[d];
    vec_minus<d,T>(a,b,tmp);
    return vec_length_sqrd<d,T>(tmp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
double vec_normalize(T * a)
{
    double len = vec_length<d,T>(a);
    vec_divide<d,T>(a, len, a);
    return len;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_swap(T * a, const uint i, const uint j)
{
    assert(i<d && j<d);
    std::swap(a[i],a[j]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_min_entry(const T * a)
{
    return *std::min_element(a, a+d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_max_entry(const T * a)
{
    return *std::max_element(a, a+d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_set_val(      T * a,
                 const T & val)
{
    for(uint i=0; i<d; ++i)
    {
        a[i] = val;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_set_ZERO(T * a)
{
    vec_set_val<d,T>(a, 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_set_MIN(T * a)
{
    vec_set_val<d,T>(a, std::numeric_limits<T>::min());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_set_MAX(T * a)
{
    vec_set_val<d,T>(a, std::numeric_limits<T>::max());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_set_INF(T * a)
{
    vec_set_val<d,T>(a, std::numeric_limits<T>::infinity());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_min(const T * a,
             const T * b,
                   T * c)
{
    for(uint i=0; i<d; ++i)
    {
        c[i] = std::min(a[i],b[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_max(const T * a,
             const T * b,
                   T * c)
{
    for(uint i=0; i<d; ++i)
    {
        c[i] = std::max(a[i],b[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_clamp(const T * a,
               const T & min,
               const T & max)
{
    for(uint i=0; i<d; ++i)
    {
        a[i] = clamp(a[i], min, max);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
double vec_angle_deg(const T * a,
                     const T * b)
{
    return to_deg(vec_angle_rad<d,T>(a,b));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
double vec_angle_rad(const T * a,
                     const T * b)
{
    T tmp_a[d], tmp_b[d];
    vec_copy<d,T>(a, tmp_a);
    vec_copy<d,T>(b, tmp_b);
    vec_normalize<d,T>(tmp_a);
    vec_normalize<d,T>(tmp_b);
    if(vec_is_degenerate<d,T>(tmp_a) || vec_is_degenerate<d,T>(tmp_b))
    {
        return std::numeric_limits<double>::infinity();
    }
    T dot = vec_dot<d,T>(tmp_a, tmp_b);
    return acos(clamp(dot,T(-1),T(1)));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
bool vec_is_null(const T * a)
{
    for(uint i=0; i<d; ++i)
    {
        if(a[i]!=0) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
bool vec_is_nan(const T * a)
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isnan(a[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
bool vec_is_inf(const T * a)
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isinf(a[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
bool vec_is_degenerate(const T * a)
{
    return (vec_is_null<d,T>(a) || vec_is_inf<d,T>(a) || vec_is_nan<d,T>(a));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_copy(const T * a,
                    T * b)
{
    std::copy(a, a+d, b);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_print(const T * a)
{
    std::cout << "[";
    for(uint i=0; i<d-1; ++i)
    {
        std::cout << a[i] << ", ";
    }
    std::cout << a[d-1] << "]" << std::endl;
}

}
