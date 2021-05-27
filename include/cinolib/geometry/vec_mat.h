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
#ifndef CINO_VEC_MAT_H
#define CINO_VEC_MAT_H

#include <cinolib/cino_inline.h>
#include <sys/types.h>
#include <ostream>

namespace cinolib
{

template<uint r, uint c, class T>
class mat
{
    public:

        union
        {
            T _vec[r*c];  // 1D view
            T _mat[r][c]; // 2D view
        };

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit mat(const std::initializer_list<T> & il);
        explicit mat();
        virtual ~mat();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static mat<r,c,T> DENSE      (const T   scalar);
        static mat<r,c,T> DIAGONAL   (const T   scalar);
        static mat<r,c,T> DIAGONAL   (const T * scalar);
        static mat<r,c,T> ROTATION   (const T   angle_rad);
        static mat<r,c,T> ROTATION   (const T   angle_rad, const T * axis);
        static mat<r,c,T> TRANSLATION(const T * tx);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const T * ptr() const { return _vec; }
              T * ptr()       { return _vec; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<1,c,T> row (const uint i) const;
        mat<r,1,T> col (const uint i) const;
        T*         diag()             const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T          det()       const;
        T          trace()     const;
        mat<r,c,T> transpose() const;
        mat<r,c,T> inverse()   const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void eigenvalues()  const;
        void eigenvectors() const;
        void eigendcomp()   const;
        void SVD()          const;
        void SSVD()         const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<c,1,T> solve(const mat<c,1,T> & b);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

              T          & operator[] (const uint pos);
        const T          & operator[] (const uint pos)        const;
              mat<r,c,T>   operator-  ()                      const;
              mat<r,c,T>   operator-  (const mat<r,c,T> & op) const;
              mat<r,c,T>   operator+  (const mat<r,c,T> & op) const;
              mat<r,c,T> & operator+= (const mat<r,c,T> & op);
              mat<r,c,T> & operator-= (const mat<r,c,T> & op);
              mat<r,c,T>   operator*  (const T & scalar)      const;
              mat<r,c,T>   operator/  (const T & scalar)      const;
              mat<r,c,T> & operator*= (const T & scalar);
              mat<r,c,T> & operator/= (const T & scalar);
              bool         operator== (const mat<r,c,T> & op) const;
              bool         operator<  (const mat<r,c,T> & op) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double norm     (const mat<r,c,T> & v) const;
        double norm_p   (const mat<r,c,T> & v, const float p) const;
        double dist     (const mat<r,c,T> & v) const;
        T      norm_sqrd(const mat<r,c,T> & v) const;
        T      dist_sqrd(const mat<r,c,T> & v) const;
        double normalize();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T          min_entry()               const;
        T          max_entry()               const;
        mat<r,c,T> min(const mat<r,c,T> & v) const;
        mat<r,c,T> max(const mat<r,c,T> & v) const;
        void       clamp(const T min, const T max);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void swap(const uint i, const uint j, const uint k, const uint l);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_null() const;
        bool is_nan()  const;
        bool is_inf()  const;
        bool is_deg()  const; // either null, nan or inf
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//template<uint d, class T> CINO_INLINE T        dot      (const vec<d,1,T> & v0, const vec<d,1,T> & v1);
//template<        class T> CINO_INLINE vec<3,T> cross    (const vec<3,T> & v0, const vec<3,T> & v1);
//template<uint d, class T> CINO_INLINE T        angle_deg(const vec<d,T> & v0, const vec<d,T> & v1, const bool unit_length = false);
//template<uint d, class T> CINO_INLINE T        angle_rad(const vec<d,T> & v0, const vec<d,T> & v1, const bool unit_length = false);

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

template<uint r, uint c, class T>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const mat<r,c,T> & op);

}

#ifndef  CINO_STATIC_LIB
#include "vec_mat.cpp"
#endif

#endif // CINO_VEC_MAT_H
