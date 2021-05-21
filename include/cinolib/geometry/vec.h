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
#ifndef CINO_VEC_H
#define CINO_VEC_H

#include <cinolib/cino_inline.h>
#include <initializer_list>

/* Base class for n dimensional points and vectors in R^d.
 * Useful also for colors, quaternions ecc..
*/
namespace cinolib
{

template<class T, uint d> // T => type      (float, double, int,...)
class vec                 // d => dimension (2,3,...)
{
    public:

        T data[(d>0) ? d : 1]; // avoid constructing a vector with zero elements

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit vec(const T & s = 0);
        explicit vec(const std::initializer_list<T> & il); // slow, avoid using it if possible

        // Specialized (faster) for R^2, R^3 and R^4 :::::::::::::::::::::::::::::

        explicit vec(const T & v0, const T & v1);
        explicit vec(const T & v0, const T & v1, const T & v2);
        explicit vec(const T & v0, const T & v1, const T & v2, const T & v3);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static vec<T,d> ZERO() { return vec<T,d>(0);                                  }
        static vec<T,d> MIN()  { return vec<T,d>(std::numeric_limits<T>::min());      }
        static vec<T,d> MAX()  { return vec<T,d>(std::numeric_limits<T>::max());      }
        static vec<T,d> INF()  { return vec<T,d>(std::numeric_limits<T>::infinity()); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const T * ptr() const { return data; }
              T * ptr()       { return data; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // Cartesian XYZ access
        const T & x() const { return data[0]; }
              T & x()       { return data[0]; }
        const T & y() const { return data[1]; }
              T & y()       { return data[1]; }
        const T & z() const { return data[2]; }
              T & z()       { return data[2]; }

        // Parametric UVW access
        const T & u() const { return data[0]; }
              T & u()       { return data[0]; }
        const T & v() const { return data[1]; }
              T & v()       { return data[1]; }
        const T & w() const { return data[2]; }
              T & w()       { return data[2]; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

              T        & operator[] (const uint pos);
        const T        & operator[] (const uint pos)      const;
              vec<T,d>   operator+  (const vec<T,d> & v)  const;
              vec<T,d>   operator-  (const vec<T,d> & v)  const;
              vec<T,d>   operator-  ()                    const;
              vec<T,d>   operator*  (const T & s)         const;
              vec<T,d>   operator/  (const T & s)         const;
              vec<T,d> & operator+= (const vec<T,d> & v);
              vec<T,d> & operator-= (const vec<T,d> & v);
              vec<T,d> & operator*= (const T & s);
              vec<T,d> & operator/= (const T & s);
              bool       operator== (const vec<T,d> & v) const;
              bool       operator<  (const vec<T,d> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T        dot  (const vec<T,d> & v) const;
        vec<T,d> cross(const vec<T,d> & v) const;

        // Static members for dot and cross products (watchout the overhead!) ::::

        static T        dot  (const vec<T,d> & v0, const vec<T,d> & v1) { return v0.dot(v1);   }
        static vec<T,d> cross(const vec<T,d> & v0, const vec<T,d> & v1) { return v0.cross(v1); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T      length_sqrd() const;
        double length     () const;
        double dist       (const vec<T,d> & v) const;
        T      dist_sqrd  (const vec<T,d> & v) const;
        T      normalize  ();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T        min_entry()                   const;
        T        max_entry()                   const;
        vec<T,d> min      (const vec<T,d> & v) const;
        vec<T,d> max      (const vec<T,d> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T angle_rad(const vec<T,d> & v) const;
        T angle_deg(const vec<T,d> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_null()       const;
        bool has_nan()       const;
        bool has_inf()       const;
        bool is_degenerate() const; // either null, nan or inf
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// useful types to have
//typedef vec<double,2> vec2d;
//typedef vec<float,2>  vec2f;
//typedef vec<int,2>    vec2i;
//typedef vec<double,3> vec3d;
//typedef vec<float,3>  vec3f;
//typedef vec<int,3>    vec3i;
//typedef vec<double,4> vec4d;
//typedef vec<float,4>  vec4f;
//typedef vec<int,4>    vec4i;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const vec<T,d> & v);

}

#ifndef  CINO_STATIC_LIB
#include "vec.cpp"
#endif

#endif // CINO_VEC_H
