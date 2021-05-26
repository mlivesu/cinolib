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

// c = a + b
template<uint r, uint c, typename T>
CINO_INLINE
void mat_plus(const T ma[r][c],
              const T mb[r][c],
                    T mc[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        ((T*)mc)[i] = ((T*)ma)[i] + ((T*)mb)[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// c = a - b
template<uint r, uint c, typename T>
CINO_INLINE
void mat_minus(const T ma[r][c],
               const T mb[r][c],
                     T mc[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        ((T*)mc)[i] = ((T*)ma)[i] - ((T*)mb)[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// a = -a
template<uint r, uint c, typename T>
CINO_INLINE
void mat_minus(T ma[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        ((T*)ma)[i] = - ((T*)ma)[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// b = a * scalar (element-wise)
template<uint r, uint c, typename T>
CINO_INLINE
void mat_times(const T ma[r][c],
               const T scalar,
                     T mb[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        ((T*)mb)[i] = ((T*)ma)[i] * scalar;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// b = a / scalar (element-wise)
template<uint r, uint c, typename T>
CINO_INLINE
void mat_divide(const T ma[r][c],
                const T scalar,
                      T mb[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        ((T*)mb)[i] = ((T*)ma)[i] / scalar;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint ra, uint ca, uint rb, uint cb, typename T>
CINO_INLINE
void mat_times(const T ma[ra][ca],
               const T mb[rb][cb],
                     T mc[ra][cb])
{
    assert(ca==rb && "matrix size mismatch");

    for(uint i=0; i<ra; ++i)
    for(uint j=0; j<cb; ++j)
    {
        mc[i][j] = 0;
        for(uint k=0; k<ca; ++k)
        {
            mc[i][j] += ma[i][k] * mb[k][j];
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
bool mat_equals(const T ma[r][c],
                const T mb[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        if(((T*)ma)[i] != ((T*)mb)[i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
bool mat_less(const T ma[r][c],
              const T mb[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        if(((T*)ma)[i] < ((T*)mb)[i]) return true;
        if(((T*)ma)[i] > ((T*)mb)[i]) return false;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_set_full(T m[r][c], const T scalar)
{
    for(uint i=0; i<r*c; ++i)
    {
        ((T*)m)[i] = scalar;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_set_diag(T m[r][c], const T scalar)
{
    assert(r==c);
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        m[i][j] = (i==j) ? scalar : 0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_set_diag(T m[r][c], const T diag[])
{
    assert(r==c);
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        m[i][j] = (i==j) ? diag[i] : 0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void mat_set_rot(T m[2][2], const T ang_rad)
{
    T rcos  = (T)cos(ang_rad);
    T rsin  = (T)sin(ang_rad);
    m[0][0] =  rcos;
    m[1][0] =  rsin;
    m[0][1] = -rsin;
    m[1][1] =  rcos;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_set_rot(T m[r][c], const T ang_rad, const T axis[3])
{    
    assert(r==c);
    if(r>3) mat_set_diag<r,c,T>(m, 1); // for transformations in homogeneous coordinates

    T u     = axis[0];
    T v     = axis[1];
    T w     = axis[2];
    T rcos  = (T)cos(ang_rad);
    T rsin  = (T)sin(ang_rad);
    m[0][0] =      rcos + u*u*(1-rcos);
    m[1][0] =  w * rsin + v*u*(1-rcos);
    m[2][0] = -v * rsin + w*u*(1-rcos);
    m[0][1] = -w * rsin + u*v*(1-rcos);
    m[1][1] =      rcos + v*v*(1-rcos);
    m[2][1] =  u * rsin + w*v*(1-rcos);
    m[0][2] =  v * rsin + u*w*(1-rcos);
    m[1][2] = -u * rsin + v*w*(1-rcos);
    m[2][2] =      rcos + w*w*(1-rcos);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void mat_set_trans(T m[4][4], const T trans[3])
{
    mat_set_diag<4,4,T>(m, 1);
    m[0][4] = trans[0];
    m[1][4] = trans[1];
    m[2][4] = trans[2];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_min(const T m1[r][c], const T m2[r][c], T m3[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        ((T*)m3)[i] = std::min(((T*)m1)[i], ((T*)m2)[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_max(const T m1[r][c], const T m2[r][c], T m3[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        ((T*)m3)[i] = std::max(((T*)m1)[i], ((T*)m2)[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
T mat_min_entry(const T m[r][c])
{
    return *std::min_element(m[0], m[0]+(r*c));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
T mat_max_entry(const T m[r][c])
{
    return *std::max_element(m[0], m[0]+(r*c));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_clamp(const T m[r][c], const T min, const T max)
{
    for(uint i=0; i<r*c; ++i)
    {
        clamp(((T*)m)[i], min, max);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
bool mat_is_null(const T m[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        if(((T*)m)[i] !=0) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
bool mat_is_nan(const T m[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        if(std::isnan(((T*)m)[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
bool mat_is_inf(const T m[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        if(std::isinf(((T*)m)[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
bool mat_is_degenerate(const T m[r][c])
{
    for(uint i=0; i<r*c; ++i)
    {
        if(std::isnormal(((T*)m)[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
bool mat_is_symmetric(const T m[r][c])
{
    assert(r==c);
    for(uint i=0;   i<r-1; ++i)
    for(uint j=i+1; j<c;   ++j)
    {
        if(m[i][j] != m[j][i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_svd (const T m[r][c], T S[r][c], T V[], T D[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_ssvd(const T m[r][c], T S[r][c], T V[], T D[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
double mat_norm_Frobenius(const T m[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_eigen_decomp(const T m[r][c], T eval[], T evec[][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_eigenvalues(const T m[r][c], T eval[])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_eigenvectors(const T m[r][c], T evec[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
T mat_det(const T m[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
T mat_trace(const T m[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_transposed(const T m[r][c], T tr[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_inverse(const T m[r][c], T in[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
T mat_det_Cramer(const T m[r][c])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_solve_w_Cramer(const T m[r][c], const T b[], T x[])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_col(const T m[r][c], T col [])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_row(const T m[r][c], T row [])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_diag(const T m[r][c], T diag[])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_col_ptr(const T m[r][c], const uint i, T *col [])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_row_ptr(T m[r][c], const uint i, T *row [])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_diag_ptr(const T m[r][c], const uint i, T *diag[])
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_copy(const T m1[r][c], T m2[r][c])
{
    std::copy(((T*)m1), ((T*)m1)+(r*c), ((T*)m2));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_print(const T m[r][c])
{
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        if(j%c==0) std::cout << "\n";
        std::cout << m[i][j] << " ";
    }
    std::cout << "\n";
}

}
