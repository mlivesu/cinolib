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

namespace cinolib
{

/*
 * Base class for dense vectors and matrices. This is templated on matrix size (r,c), and scalar type (T).
 * Data is internally stored as both 1D and 2D C arrays with a shared memory allocation, so that both views
 * can be efficiently accessed without requiring any type casting.
 *
 * This class is a wrapper for (a subset of) basic linear algebra operations for raw C arrays.
 * The full set of per vector and per matrix functionalities can be directly accessed through
 * the original methods, defined in:
 *
 *    cinolib/geometry/vec_utils.h
 *    cinolib/geometry/mat_utils.h
*/

template<uint r, uint c, class T>
class vec_mat
{
    public:

        union
        {
            T vec[r*c];  // 1D view
            T mat[r][c]; // 2D view
        };

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit vec_mat() {}
        explicit vec_mat(const T & scalar);
        virtual ~vec_mat() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const T *  ptr_vec() const { return vec; }
              T *  ptr_vec()       { return vec; }

        const T ** ptr_mat() const { return mat; }
              T ** ptr_mat()       { return mat; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

              T              & operator[] (const uint pos);
        const T              & operator[] (const uint pos)            const;
              vec_mat<r,c,T>   operator-  ()                          const;
              vec_mat<r,c,T>   operator-  (const vec_mat<r,c,T> & op) const;
              vec_mat<r,c,T>   operator+  (const vec_mat<r,c,T> & op) const;
              vec_mat<r,c,T> & operator+= (const vec_mat<r,c,T> & op);
              vec_mat<r,c,T> & operator-= (const vec_mat<r,c,T> & op);
              vec_mat<r,c,T>   operator*  (const T & scalar)          const;
              vec_mat<r,c,T>   operator/  (const T & scalar)          const;
              vec_mat<r,c,T> & operator*= (const T & scalar);
              vec_mat<r,c,T> & operator/= (const T & scalar);
              bool             operator== (const vec_mat<r,c,T> & op) const;
              bool             operator<  (const vec_mat<r,c,T> & op) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T              min_entry()                   const;
        T              max_entry()                   const;
        vec_mat<r,c,T> min(const vec_mat<r,c,T> & v) const;
        vec_mat<r,c,T> max(const vec_mat<r,c,T> & v) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_null()       const;
        bool is_nan()        const;
        bool is_inf()        const;
        bool is_degenerate() const; // either null, nan or inf
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const vec_mat<r,c,T> & op);

}

#ifndef  CINO_STATIC_LIB
#include "vec_mat.cpp"
#endif

#endif // CINO_VEC_MAT_H
