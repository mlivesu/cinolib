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
#include <cinolib/geometry/vec_mat.h>
#include <cinolib/geometry/mat_utils.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
std::ostream & operator<< (std::ostream & in, const vec_mat<r,c,T> & op)
{
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        if(c>1 && j%c==0) std::cout << "\n";
        std::cout << op.mat[i][j] << " ";
    }
    std::cout << "\n";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T>::vec_mat(const T & scalar)
{
//    mat_set_/*full*/<r,c,T>(mat, scalar);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T & vec_mat<r,c,T>::operator[](const uint pos)
{
    return vec[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
const T & vec_mat<r,c,T>::operator[](const uint pos) const
{
    return vec[pos];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> vec_mat<r,c,T>::operator+(const vec_mat<r,c,T> & v) const
{
    vec_mat<r,c,T> res;
//    mat_plus<r,c,T>(mat, v.mat, res.mat);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> vec_mat<r,c,T>::operator-(const vec_mat<r,c,T> & v) const
{
    vec_mat<r,c,T> res;
//    mat_minus<r,c,T>(mat, v.mat, res.mat);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> vec_mat<r,c,T>::operator-() const
{
    vec_mat<r,c,T> res;
//    mat_minus<r,c,T>(mat);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> vec_mat<r,c,T>::operator*(const T & s) const
{
    vec_mat<r,c,T> res;
//    mat_times<r,c,T>(mat, s, res.mat);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> vec_mat<r,c,T>::operator/(const T & s) const
{
    vec_mat<r,c,T> res;
//    mat_divide<r,c,T>(mat, s, res.mat);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> & vec_mat<r,c,T>::operator+=(const vec_mat<r,c,T> & v)
{
//    mat_plus<r,c,T>(mat, v.mat, mat);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> & vec_mat<r,c,T>::operator-=(const vec_mat<r,c,T> & v)
{
//    mat_minus<r,c,T>(mat, v.mat, mat);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> & vec_mat<r,c,T>::operator*=(const T & s)
{
//    mat_times<r,c,T>(mat, s, mat);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> & vec_mat<r,c,T>::operator/=(const T & s)
{
//    mat_divide<r,c,T>(mat, s, mat);
    return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool vec_mat<r,c,T>::operator==(const vec_mat<r,c,T> & v) const
{
//    return mat_equals<r,c,T>(mat, v.mat);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool vec_mat<r,c,T>::operator<(const vec_mat<r,c,T> & v) const
{
//    return mat_less<r,c,T>(mat, v.mat);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T vec_mat<r,c,T>::min_entry() const
{
//    return mat_min_entry<r,c,T>(mat);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
T vec_mat<r,c,T>::max_entry() const
{
//    return mat_max_entry<r,c,T>(mat);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> vec_mat<r,c,T>::min(const vec_mat<r,c,T> & v) const
{
    vec_mat<r,c,T> res;
//    mat_min<r,c,T>(mat, v.mat, res.mat);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
vec_mat<r,c,T> vec_mat<r,c,T>::max(const vec_mat<r,c,T> & v) const
{
    vec_mat<r,c,T> res;
//    mat_max<r,c,T>(mat, v.mat, res.mat);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool vec_mat<r,c,T>::is_null() const
{
//    return mat_is_null<r,c,T>(mat);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool vec_mat<r,c,T>::is_nan() const
{
//    return mat_is_nan<r,c,T>(mat);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool vec_mat<r,c,T>::is_inf() const
{
//    return mat_is_inf<r,c,T>(mat);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, class T>
CINO_INLINE
bool vec_mat<r,c,T>::is_degenerate() const
{
//    return mat_is_degenerate<r,c,T>(mat);
}

}
