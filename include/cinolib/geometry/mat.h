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
#ifndef CINO_MAT_H
#define CINO_MAT_H

#include <cinolib/cino_inline.h>
#include <initializer_list>

namespace cinolib
{

template<uint r, uint c, class T>
class mat
{
    public:

        T data[r*c];

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit mat(const T & s = 0);
        explicit mat(const std::initializer_list<T> & il);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static mat<r,c,T> ZERO() { return mat<r,c,T>(0);                                  }
        static mat<r,c,T> MIN()  { return mat<r,c,T>(std::numeric_limits<T>::min());      }
        static mat<r,c,T> MAX()  { return mat<r,c,T>(std::numeric_limits<T>::max());      }
        static mat<r,c,T> INF()  { return mat<r,c,T>(std::numeric_limits<T>::infinity()); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const T * ptr() const { return data; }
              T * ptr()       { return data; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

              T          & operator[] (const uint pos);
        const T          & operator[] (const uint pos)        const;
              mat<r,c,T>   operator+  (const mat<r,c,T> & v)  const;
              mat<r,c,T>   operator-  (const mat<r,c,T> & v)  const;
              mat<r,c,T>   operator-  ()                      const;
              mat<r,c,T>   operator*  (const T & s)           const;
              mat<r,c,T>   operator/  (const T & s)           const;
              mat<r,c,T> & operator+= (const mat<r,c,T> & v);
              mat<r,c,T> & operator-= (const mat<r,c,T> & v);
              mat<r,c,T> & operator*= (const T & s);
              mat<r,c,T> & operator/= (const T & s);
              bool         operator== (const mat<r,c,T> & v) const;
              bool         operator<  (const mat<r,c,T> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T      length_sqrd() const;
        double length     () const;
        double dist       (const mat<r,c,T> & v) const;
        T      dist_sqrd  (const mat<r,c,T> & v) const;
        T      normalize  ();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T        min_entry()                   const;
        T        max_entry()                   const;
        mat<r,c,T> min      (const mat<r,c,T> & v) const;
        mat<r,c,T> max      (const mat<r,c,T> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_null()       const;
        bool is_nan()        const;
        bool is_inf()        const;
        bool is_degenerate() const; // either null, nan or inf
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// useful types to have
//typedef mat<double,2> mat2d;
//typedef mat<float,2>  mat2f;
//typedef mat<int,2>    mat2i;
//typedef mat<double,3> mat3d;
//typedef mat<float,3>  mat3f;
//typedef mat<int,3>    mat3i;
//typedef mat<double,4> mat4d;
//typedef mat<float,4>  mat4f;
//typedef mat<int,4>    mat4i;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const mat<r,c,T> & v);

}

#ifndef  CINO_STATIC_LIB
#include "mat.cpp"
#endif

#endif // CINO_MAT_H
