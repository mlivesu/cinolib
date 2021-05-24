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
#include <cinolib/geometry/mat_utils.h>
#include <cinolib/clamp.h>
#include <limits>
#include <ostream>
#include <assert.h>

namespace cinolib
{

// m3 = m1 + m2
template<uint r, uint c, typename T>
CINO_INLINE
void min_plus(const T * m1,
              const T * m2,
                    T * m3)
{
    for(uint i=0; i<r*c; ++i)
    {
        m3[i] = m1[i] + m2[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// m1 += m2
template<uint r, uint c, typename T>
CINO_INLINE
void min_plus(      T * m1,
              const T * m2)
{
    for(uint i=0; i<r*c; ++i)
    {
        m1[i] += m2[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// m3 = m1 - m2
template<uint r, uint c, typename T>
CINO_INLINE
void mat_minus(const T * m1,
               const T * m2,
                     T * m3)
{
    for(uint i=0; i<r*c; ++i)
    {
        m3[i] = m1[i] - m2[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// m1 -= m2
template<uint r, uint c, typename T>
CINO_INLINE
void min_minus(      T * m1,
               const T * m2)
{
    for(uint i=0; i<r*c; ++i)
    {
        m1[i] -= m2[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// m2 = m1 * s (element-wise)
template<uint r, uint c, typename T>
CINO_INLINE
void mat_scale(const T * m1,
               const T & s,
                     T * m2)
{
    for(uint i=0; i<r*c; ++i)
    {
        m2[i] = m1[i] * s;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// m1 *= s (element-wise)
template<uint r, uint c, typename T>
CINO_INLINE
void mat_scale(      T * m1,
               const T & s)
{
    for(uint i=0; i<r*c; ++i)
    {
        m1[i] *= s;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// m3 = m1 / s
template<uint r, uint c, typename T>
CINO_INLINE
void mat_divide(const T * m1,
                const T & s,
                      T * m2)
{
    for(uint i=0; i<r*m2; ++i)
    {
        m2[i] = m1[i] / s;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// m1 /= s
template<uint r, uint c, typename T>
CINO_INLINE
void mat_divide(      T * m1,
                const T & s)
{
    for(uint i=0; i<r*c; ++i)
    {
        m1[i] /= s;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// m = -m
template<uint r, uint c, typename T>
CINO_INLINE
void mat_flip_sign(const T *m)
{
    for(uint i=0; i<r*c; ++i)
    {
        m[i] = -m[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r1, uint c1, uint r2, uint c2, typename T>
CINO_INLINE
void mat_multiply(const T * m1,
                  const T * m2,
                        T * m3)
{
    assert(c1==r2 && "matrix size mismatch");

    for(uint i=0; i<r1; ++i)
    for(uint j=0; j<c2; ++j)
    {
        uint ij = c1*i+j;
        m3[ij] = 0;
        for(uint k=0; k<c1; ++k)
        {
            uint ik = c1*i+k;
            uint kj = c2*k+j;
            m3[ij] += m1[ik] * m2[kj];
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//// c = a * b
//template<uint R, uint C, typename T>
//CINO_INLINE
//void scale(const T *a,
//           const T  b,
//                 T *c)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        c[j][i] = a[j][i] * b;
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//// c = a / b
//template<uint R, uint C, typename T>
//CINO_INLINE
//void divide(const T *a,
//            const T  b,
//                  T *c)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        c[j][i] = a[j][i] / b;
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R0, uint C0, uint R1, uint C1, typename T>
//CINO_INLINE
//void multiply(const T *a,
//              const T *b,
//                    T *c)
//{
//    assert(C0==R1);
//    for(uint i=0; i<R0; ++i)
//    for(uint j=0; j<C1; ++i)
//    {
//        c[j][i] = vec::dot(row(a,i), col(b,j));
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//bool equals(const T *a,
//            const T *b)
//{
//    for(uint i=0; i<R; ++i)
//    for(uint j=0; j<C; ++j)
//    {
//        if(a[j][i] != b[j][i]) return false;
//    }
//    return true;
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//T min_entry(const T *a)
//{
//    return *std::min_element(a, a+(R*C));
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//T max_entry(const T *a)
//{
//    return *std::max_element(a, a+(R*C));
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//void set_ZERO(T *a)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        a[j][i] = 0;
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//void set_MIN(T *a)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        a[j][i] = std::numeric_limits<T>::min();
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//void set_MAX(T *a)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        a[j][i] = std::numeric_limits<T>::max();
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//void set_INF(T *a)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        a[j][i] = std::numeric_limits<T>::infinity();
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//void min(const T *a,
//         const T *b,
//               T *c)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        c[j][i] = std::min(a[j][i],b[j][i]);
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//T max(const T *a,
//      const T *b,
//            T *c)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        c[j][i] = std::max(a[j][i],b[j][i]);
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//void clamp(const T *a,
//           const T  min,
//           const T  max)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        a[j][i] = clamp(a[j][i], min, max);
//    }
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//bool is_null(const T *a)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        if(a[j][i]!=0) return false;
//    }
//    return true;
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//bool is_nan(const T *a)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        if(std::isnan(a[j][i])) return true;
//    }
//    return false;
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//bool has_inf(const T *a)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        if(std::isinf(a[j][i])) return true;
//    }
//    return false;
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//bool is_degenerate(const T *a)
//{
//    for(uint j=0; j<C; ++j)
//    for(uint i=0; i<R; ++i)
//    {
//        if(!std::isnormal(a[j][i])) return true;
//    }
//    return false;
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//void copy(const T *a,
//                T *b)
//{
//    std::copy(a, a+(R*C), b);
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint R, uint C, typename T>
//CINO_INLINE
//void row_ptr(T a[][C], const uint i, T* r[])
//{
//    for(uint j=0; j<C; ++j)
//    {
//        r[j] = &(a[j][i]);
//    }
//}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_print(const T * m)
{
    for(uint i=0; i<r; ++i)
    {
        for(uint j=0; j<c; ++j)
        {
            if(j%c==0) std::cout << "\n";
            std::cout << m[i*c+j] << " ";
        }
    }
    std::cout << "\n";
}

}
