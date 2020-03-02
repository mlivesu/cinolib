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
#ifndef CINO_MAT_H
#define CINO_MAT_H

#include <cinolib/cino_inline.h>
#include <initializer_list>

/* Base class for nxm dimensional matrices. */

namespace cinolib
{

template<class T, uint r, uint c> // T   => element type (float, double, int,...)
class mat                         // r,c => matrix rows and columns
{
    private:

        T val[r][c];

    public:

        explicit mat(const T s = 0);
        explicit mat(const std::initializer_list<T> & il);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static mat<T,r,c> DIAG(const T s);
        static mat<T,r,c> DIAG(const std::vector<T> & d);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static mat<T,r,r> SCALING(const std::vector<T> & d);
        static mat<T,r,r> ROTATION(/* axis , angle */);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        static mat<T,r,c> ZERO() { return mat<T,r,c>(0);                                  }
        static mat<T,r,c> MIN()  { return mat<T,r,c>(std::numeric_limits<T>::min());      }
        static mat<T,r,c> MAX()  { return mat<T,r,c>(std::numeric_limits<T>::max());      }
        static mat<T,r,c> INF()  { return mat<T,r,c>(std::numeric_limits<T>::infinity()); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

              T & at(const uint _r, const uint _c);
        const T & at(const uint _r, const uint _c) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        typedef T* T_ptr; // implements the double parenthesis matrix operator
        T_ptr        operator[] (const uint  i)        const;
        T            operator[] (const T_ptr ptr)      const;

        mat<T,r,c>   operator+  (const mat<T,r,c> & m) const;
        mat<T,r,c>   operator-  (const mat<T,r,c> & m) const;
        mat<T,r,c>   operator-  ()                     const;
        mat<T,r,c>   operator*  (const T s)            const;
        mat<T,r,c>   operator/  (const T s)            const;
        mat<T,r,c> & operator+= (const mat<T,r,c> & m);
        mat<T,r,c> & operator-= (const mat<T,r,c> & m);
        mat<T,r,c> & operator*= (const T s);
        mat<T,r,c> & operator/= (const T s);
        bool         operator== (const mat<T,r,c> & m) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const T * ptr() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void svd()        const;
        void ssvd()       const;
        void p_norm()     const;
        void transposed() const;
        void inverse()    const;
        void solve()      const; // solve linear system m * x = b
        void eigenvals()  const;
        void eigenvecs()  const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void to_Eigen();
        void from_Eigen();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool is_null()       const;
        bool is_nan()        const;
        bool is_inf()        const;
        bool is_degenerate() const; // either null, nan or inf
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// useful types to have
typedef mat<double,2,2> mat22d;
typedef mat<float,2,2>  mat22f;
typedef mat<double,3,3> mat33d;
typedef mat<float,3,3>  mat33f;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T, uint r, uint c>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const mat<T,r,c> & m);

}

#ifndef  CINO_STATIC_LIB
#include "mat.cpp"
#endif

#endif // CINO_MAT_H
