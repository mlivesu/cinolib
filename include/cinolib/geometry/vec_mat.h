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

#include <ostream>
#include <cinolib/geometry/vec_mat_utils.h>
#include <cinolib/symbols.h>

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

        explicit mat(const std::initializer_list<mat<r,1,T>> & il, const int mode = COLS);
        explicit mat(const std::initializer_list<T> & il);
        explicit mat(const T   scalar);
        explicit mat(const T * values);
        explicit mat(const T v0, const T v1);
        explicit mat(const T v0, const T v1, const T v2);
        explicit mat() {}
        virtual ~mat() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static mat<r,c,T> ZERO  ();
        static mat<r,c,T> ONES  ();
        static mat<r,c,T> DIAG  (const T scalar);
        static mat<r,c,T> DIAG  (const mat<r,  1,T> & diag);
        static mat<r,c,T> TRANS (const mat<r-1,1,T> & tx);
        static mat<r,c,T> ROT_3D(const mat<r,  1,T> & axis, const T angle_rad);
        static mat<r,c,T> ROT_2D(const T angle_rad);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const T * ptr() const { return _vec; }
              T * ptr()       { return _vec; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<c,1,T> row (const uint i) const;
        mat<r,1,T> col (const uint i) const;
        mat<r,1,T> diag()             const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void set_row (const uint i, const mat<c,1,T> & row);
        void set_col (const uint i, const mat<r,1,T> & col );
        void set_diag(              const mat<r,1,T> & diag);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T          det()       const;
        T          trace()     const;
        mat<c,r,T> transpose() const;
        mat<r,c,T> inverse()   const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void eigenvectors(mat<r,c,T> & evec)                    const;
        void eigenvalues (mat<r,1,T> & eval)                    const;
        void eigendecomp (mat<r,1,T> & eval, mat<r,c,T> & evec) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void SVD (mat<r,c,T> & U, mat<r,1,T> & S, mat<r,c,T> & V) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<r,c,T> closest_orthogonal_matrix(const bool force_positve_det) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<r,1,T> solve(const mat<c,1,T> & b);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const T          & operator[] (const uint i)  const;
              T          & operator[] (const uint i);
        const T          & operator() (const uint i, const uint j)  const;
              T          & operator() (const uint i, const uint j);
              mat<r,c,T>   operator-  ()                            const;
              mat<r,c,T>   operator-  (const mat<r,c,T> & op)       const;
              mat<r,c,T>   operator+  (const mat<r,c,T> & op)       const;
              mat<r,c,T> & operator+= (const mat<r,c,T> & op);
              mat<r,c,T> & operator-= (const mat<r,c,T> & op);
              mat<r,c,T>   operator*  (const T & scalar)            const;
              mat<r,c,T>   operator/  (const T & scalar)            const;
              mat<r,c,T> & operator*= (const T & scalar);
              mat<r,c,T> & operator/= (const T & scalar);
              bool         operator== (const mat<r,c,T> & op)       const;
              bool         operator<  (const mat<r,c,T> & op)       const;

        template<uint c2>
        mat<r,c2,T> operator*(const mat<c,c2,T> & op) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<r,c,T> & rotate(const mat<3,1,T> & axis, const T angle_rad);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double normalize();
        double norm     () const;
        T      norm_sqrd() const;
        double norm_p   (const float p) const;
        double dist     (const mat<r,c,T> & v) const;
        T      dist_sqrd(const mat<r,c,T> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T          min_entry()               const;
        T          max_entry()               const;
        mat<r,c,T> min(const mat<r,c,T> & v) const;
        mat<r,c,T> max(const mat<r,c,T> & v) const;
        void       clamp(const T min, const T max);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void swap(const uint i, const uint j, const uint k, const uint l);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<r+1,c,T> add_coord(const T & val) const; // e.g. from cartesian to homogeneous coords
        mat<r-1,c,T> rem_coord()              const; // e.g. from homogeneous to cartesian coords

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_null() const;
        bool is_nan()  const;
        bool is_inf()  const;
        bool is_deg()  const; // either null, nan or inf

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // Specialized Cartesian XYZ access
        const T & x() const { return _vec[0]; }
              T & x()       { return _vec[0]; }
        const T & y() const { return _vec[1]; }
              T & y()       { return _vec[1]; }
        const T & z() const { return _vec[2]; }
              T & z()       { return _vec[2]; }

        // Specialized Parametric UVW access
        const T & u() const { return _vec[0]; }
              T & u()       { return _vec[0]; }
        const T & v() const { return _vec[1]; }
              T & v()       { return _vec[1]; }
        const T & w() const { return _vec[2]; }
              T & w()       { return _vec[2]; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        mat<3,1,T> cross(const mat<3,1,T> & v) const;
        T          dot  (const mat<r,c,T> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T angle_deg(const mat<r,c,T> & v0, const bool normalize = true) const;
        T angle_rad(const mat<r,c,T> & v0, const bool normalize = true) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
mat<r,c,T> operator*(const T & scalar, const mat<r,c,T> & m)
{
    mat<r,c,T> res;
    vec_times<r*c,T>(m._vec, scalar, res._vec);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const mat<r,c,T> & op);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// useful types to have

typedef mat<2,2,double> mat2d;
typedef mat<2,2,float>  mat2f;
typedef mat<2,2,int>    mat2i;

typedef mat<3,3,double> mat3d;
typedef mat<3,3,float>  mat3f;
typedef mat<3,3,int>    mat3i;

typedef mat<4,4,double> mat4d;
typedef mat<4,4,float>  mat4f;
typedef mat<4,4,int>    mat4i;

typedef mat<2,1,double> vec2d;
typedef mat<2,1,float>  vec2f;
typedef mat<2,1,int>    vec2i;

typedef mat<3,1,double> vec3d;
typedef mat<3,1,float>  vec3f;
typedef mat<3,1,int>    vec3i;

typedef mat<4,1,double> vec4d;
typedef mat<4,1,float>  vec4f;
typedef mat<4,1,int>    vec4i;

}

#ifndef  CINO_STATIC_LIB
#include "vec_mat.cpp"
#endif

#endif // CINO_VEC_MAT_H
