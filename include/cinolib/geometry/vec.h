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
#ifndef CINO_VEC_H
#define CINO_VEC_H

#include <cinolib/cino_inline.h>
#include <cinolib/min_max_inf.h>
#include <array>

/* Base class for n dimensional points and vectors in R^d,
 * as well as for colors, quaternions ecc.
*/

namespace cinolib
{

template<class T, uint d> // T => type      (float, double, int,...)
class vec                 // d => dimension (2,3,...)
{
    private:

        T val[(d>0) ? d : 1]; // avoid constructing a vector with zero elements

    public:

        explicit vec(const T s = 0);
        explicit vec(const std::initializer_list<T> & il);

        // Specialized for R^2 and R^3 :::::::::::::::::::::::::::::::::::::::::::

        explicit vec(const double v0, const double v1);
        explicit vec(const double v0, const double v1, const double v2);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static vec<T,d> ZERO() { return vec<T,d>(0);                                  }
        static vec<T,d> MIN()  { return vec<T,d>(std::numeric_limits<T>::min());      }
        static vec<T,d> MAX()  { return vec<T,d>(std::numeric_limits<T>::max());      }
        static vec<T,d> INF()  { return vec<T,d>(std::numeric_limits<T>::infinity()); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

              T & at(const uint off);
        const T & at(const uint off) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

              T        & operator[] (const uint off);
        const T        & operator[] (const uint off)      const;
              vec<T,d>   operator+  (const vec<T,d> & v)  const;
              vec<T,d>   operator-  (const vec<T,d> & v)  const;
              vec<T,d>   operator*  (const vec<T,d> & v)  const;
              vec<T,d>   operator+= (const vec<T,d> & v);
              vec<T,d>   operator-= (const vec<T,d> & v);
              vec<T,d>   operator-  ()                    const;
              vec<T,d>   operator*  (const T s)           const;
              vec<T,d>   operator/  (const T s)           const;
              vec<T,d>   operator*= (const T s);
              vec<T,d>   operator/= (const T s);
              bool       operator== (const vec<T,d> & v) const;
              bool       operator<  (const vec<T,d> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const T * ptr() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T        dot  (const vec<T,d> & v) const;
        vec<T,d> cross(const vec<T,d> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static T        dot  (const vec<T,d> & v0, const vec<T,d> & v1) { return v0.dot(v1);   }
        static vec<T,d> cross(const vec<T,d> & v0, const vec<T,d> & v1) { return v0.cross(v1); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T length_squared() const;
        T length        () const;
        T dist          (const vec<T,d> & v) const;
        T dist_squared  (const vec<T,d> & v) const;
        T normalize     ();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // Cartesian XYZ access
        const T & x() const { return val.at(0); }
              T & x()       { return val.at(0); }
        const T & y() const { return val.at(1); }
              T & y()       { return val.at(1); }
        const T & z() const { return val.at(2); }
              T & z()       { return val.at(2); }

        // Parametric UVW access
        const T & u() const { return val.at(0); }
              T & u()       { return val.at(0); }
        const T & v() const { return val.at(1); }
              T & v()       { return val.at(1); }
        const T & w() const { return val.at(2); }
              T & w()       { return val.at(2); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double   min_entry()                   const;
        double   max_entry()                   const;
        vec<T,d> min      (const vec<T,d> & v) const;
        vec<T,d> max      (const vec<T,d> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double angle_rad(const vec<T,d> & v) const;
        double angle_deg(const vec<T,d> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_null()       const;
        bool is_nan()        const;
        bool is_inf()        const;
        bool is_degenerate() const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// useful types to have
typedef vec<double,2> Vec2d;
typedef vec<double,3> Vec3d;
typedef vec<int,2>    Vec2i;
typedef vec<int,3>    Vec3i;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint d>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const vec<T,d> & v);

}

#ifndef  CINO_STATIC_LIB
#include "vec.cpp"
#endif

#endif // CINO_VEC_H
