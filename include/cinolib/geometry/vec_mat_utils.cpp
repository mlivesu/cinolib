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
#include <cinolib/geometry/vec_mat_utils.h>
#include <cinolib/clamp.h>
#include <cinolib/deg_rad.h>
#include <ostream>
#include <cmath>
#include <assert.h>

namespace cinolib
{

template<uint d, typename T>
CINO_INLINE
void vec_set(T * v, const std::initializer_list<T> & il)
{
    assert(il.size()==d);
    auto it = il.begin();
    for(uint i=0; i<d; ++i,++it) v[i] = *it;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_set_dense(T * vec, const T val)
{
    for(uint i=0; i<d; ++i)
    {
        vec[i] = val;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// v2 = v0 + v1
template<uint d, typename T>
CINO_INLINE
void vec_plus(const T * v0, const T * v1, T * v2)
{
    for(uint i=0; i<d; ++i)
    {
        v2[i] = v0[i] + v1[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// v2 = v0 - v1
template<uint d, typename T>
CINO_INLINE
void vec_minus(const T * v0, const T * v1, T * v2)
{
    for(uint i=0; i<d; ++i)
    {
        v2[i] = v0[i] - v1[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// v = -v
template<uint d, typename T>
CINO_INLINE
void vec_minus(const T * v0, T * v1)
{
    for(uint i=0; i<d; ++i)
    {
        v1[i] = -v0[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// v1 = v0 * val (element-wise)
template<uint d, typename T>
CINO_INLINE
void vec_times(const T * v0, const T val, T * v1)
{
    for(uint i=0; i<d; ++i)
    {
        v1[i] = v0[i] * val;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// v1 = v0 / val (element-wise)
template<uint d, typename T>
CINO_INLINE
void vec_divide(const T * v0, const T val, T * v1)
{
    for(uint i=0; i<d; ++i)
    {
        v1[i] = v0[i] / val;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if v0 == v1 (element-wise)
template<uint d, typename T>
CINO_INLINE
bool vec_equals(const T * v0, const T * v1)
{
    for(uint i=0; i<d; ++i)
    {
        if(v0[i] != v1[i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if v0 < v1 (element-wise)
template<uint d, typename T>
CINO_INLINE
bool vec_less(const T * v0, const T * v1)
{
    for(uint i=0; i<d; ++i)
    {
        if(v0[i] < v1[i]) return true;
        if(v0[i] > v1[i]) return false;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_dot(const T * v0, const T * v1)
{
    T res = 0;
    for(uint i=0; i<d; ++i)
    {
        res += v0[i]*v1[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void vec_cross(const T * v0, const T * v1, T * v2)
{
        v2[0] = v0[1] * v1[2] - v0[2] * v1[1];
        v2[1] = v0[2] * v1[0] - v0[0] * v1[2];
        v2[2] = v0[0] * v1[1] - v0[1] * v1[0];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_angle_deg(const T * v0, const T * v1, const bool prenormalize)
{
    return (T)to_deg((double)vec_angle_rad<d,T>(v0,v1,prenormalize));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_angle_rad(const T * v0, const T * v1, const bool prenormalize)
{
    T dot;
    if(prenormalize)
    {
        // normalize input vecs if they are not known to be BOTH already normal
        T tmp0[d], tmp1[d];
        vec_copy<d,T>(v0, tmp0);
        vec_copy<d,T>(v1, tmp1);
        vec_normalize<d,T>(tmp0);
        vec_normalize<d,T>(tmp1);
        if(vec_is_deg<d,T>(tmp0) || vec_is_deg<d,T>(tmp1))
        {
            return std::numeric_limits<T>::infinity();
        }
        dot = vec_dot<d,T>(tmp0,tmp1);
    }
    else
    {
        dot = vec_dot<d,T>(v0,v1);
    }
    return acos(clamp(dot,T(-1),T(1)));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// vector distance (in the L2 sense)
template<uint d, typename T>
CINO_INLINE
double vec_dist_sqrd(const T * v_0, const T * v_1)
{
    double res = 0.0;
    for(uint i=0; i<d; ++i)
    {
        T tmp = v_0[i] - v_1[i];
        res += tmp*tmp;
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// L2 vector norm
template<uint d, typename T>
CINO_INLINE
double vec_norm_sqrd(const T * v)
{
    double res = 0.0;
    for(uint i=0; i<d; ++i)
    {
        res += v[i]*v[i];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// vector distance (in the L2 sense)
template<uint d, typename T>
CINO_INLINE
double vec_dist(const T * v_0, const T * v_1)
{
    return sqrt(vec_dist_sqrd<d,T>(v_0,v_1));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// L2 vector norm
template<uint d, typename T>
CINO_INLINE
double vec_norm(const T * v)
{
    return sqrt(vec_norm_sqrd<d,T>(v));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Lp vector norm
template<uint d, typename T>
CINO_INLINE
double vec_norm_p(const T * v, const float p)
{
    double res = 0.0;
    for(uint i=0; i<d; ++i)
    {
        res += std::pow(std::fabs(v[i]),p);
    }
    res = std::pow(res,1.0/p);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
double vec_normalize(T * v)
{
    double n = vec_norm<d,T>(v);
    if(vec_is_deg<d,T>(v)) return -1;
    vec_divide<d,T>(v,n,v);
    return n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// element-wise min
template<uint d, typename T>
CINO_INLINE
void vec_min(const T * v0, const T * v1, T * v2)
{
    for(uint i=0; i<d; ++i)
    {
        v2[i] = std::min(v0[i], v1[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// element-wise max
template<uint d, typename T>
CINO_INLINE
void vec_max(const T * v0, const T * v1, T * v2)
{
    for(uint i=0; i<d; ++i)
    {
        v2[i] = std::max(v0[i], v1[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_min_entry(const T * v)
{
    return *std::min_element(v,v+d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T vec_max_entry(const T * v)
{
    return *std::max_element(v,v+d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// element-wise clamp
template<uint d, typename T>
CINO_INLINE
void vec_clamp(T * v, const T min, const T max)
{
    for(uint i=0; i<d; ++i)
    {
        v[i] = clamp(v[i], min, max);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_swap(T * v, const uint i, const uint j)
{
    assert(i<d && j<d);
    std::swap(v[i],v[j]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if all entries are zero
template<uint d, typename T>
CINO_INLINE
bool vec_is_null(const T * v)
{
    for(uint i=0; i<d; ++i)
    {
        if(v[i] !=0) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if at least one entry is NaN
template<uint d, typename T>
CINO_INLINE
bool vec_is_nan(const T * v)
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isnan(v[i])) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if at least one entry is inf
template<uint d, typename T>
CINO_INLINE
bool vec_is_inf(const T * v)
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isinf(v[i])) return true;
    }
    return false;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if at least one entry is NaN or Inf, or all entries are zero
template<uint d, typename T>
CINO_INLINE
bool vec_is_deg(const T * v)
{
    for(uint i=0; i<d; ++i)
    {
        if(std::isinf(v[i]) || std::isnan(v[i])) return true;
    }
    return vec_is_null<d,T>(v);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_copy(const T * v0, T * v1)
{
    std::copy(v0, v0+d, v1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void vec_print(const T * v)
{
    for(uint i=0; i<d; ++i)
    {
        std::cout << v[i] << " ";
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_set(T m[][c], const std::initializer_list<T> & il)
{
    assert(il.size()==r*c);
    auto it = il.begin();
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        m[i][j] = *it;
        ++it;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// initialize diagonal matrix
template<uint d, typename T>
CINO_INLINE
void mat_set_diag(T m[][d], const T val)
{
    for(uint i=0; i<d; ++i)
    for(uint j=0; j<d; ++j)
    {
        m[i][j] = (i==j) ? val : 0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// initialize diagonal matrix
template<uint d, typename T>
CINO_INLINE
void mat_set_diag(T m[][d], const T diag[])
{
    for(uint i=0; i<d; ++i)
    for(uint j=0; j<d; ++j)
    {
        m[i][j] = (i==j) ? diag[i] : 0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_set_row(T m[][c], const uint i, const T row[])
{
    for(uint j=0; j<c; ++j)
    {
        m[i][j] = row[j];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_set_col(T m[][c], const uint i, const T col[])
{
    for(uint j=0; j<r; ++j)
    {
        m[j][i] = col[j];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// initialize 2D rotation matrix
template<uint d,typename T>
CINO_INLINE
void mat_set_rot_2d(T m[][d], const T ang_rad)
{
    assert(d==2 || d==3);
    if(d==3) mat_set_diag<d,T>(m,1); // for transformations in homogeneous coordinates

    T rcos  = (T)cos(ang_rad);
    T rsin  = (T)sin(ang_rad);
    m[0][0] =  rcos;
    m[1][0] =  rsin;
    m[0][1] = -rsin;
    m[1][1] =  rcos;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// initialize 3D rotation matrix
template<uint d,typename T>
CINO_INLINE
void mat_set_rot_3d(T m[][d], const T ang_rad, const T axis[])
{
    assert(d==3 || d==4);
    if(d==4) mat_set_diag<d,T>(m, 1); // for transformations in homogeneous coordinates

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

// initialize translation matrix
// NOTE: translation is a non linear operation.
// Homogeneous coordinates are assumed here, hence tx is supposed to be a (d-1) vector
template<uint d, typename T>
CINO_INLINE
void mat_set_trans(T m[][d], const T tx[])
{
    mat_set_diag<d,T>(m,1);
    for(uint i=0; i<d-1; ++i)
    {
        m[i][d-1] = tx[i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_swap(T m[][c], const uint i, const uint j, const uint k, const uint l)
{
    assert(i<r && j<c && k<r && l<c);
    std::swap(m[i][j], m[k][l]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// copy ith column
template<uint r, uint c, typename T>
CINO_INLINE
void mat_col(const T m[][c], const uint i, T col[])
{
    assert(i<c);
    for(uint j=0; j<r; ++j)
    {
        col[j] = m[j][i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// copy ith row
template<uint r, uint c, typename T>
CINO_INLINE
void mat_row(const T m[][c], const uint i, T row[])
{
    assert(i<r);
    std::copy(m[i], m[i]+c, row);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// copy diagonal
template<uint d, typename T>
CINO_INLINE
void mat_diag(const T m[][d], T diag[])
{
    for(uint i=0; i<d; ++i)
    {
        diag[i] = m[i][i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// vector of pointers to elements in the ith column
template<uint r, uint c, typename T>
CINO_INLINE
void mat_col_ptr(const T m[][c], const uint i, T * col[])
{
    assert(i<c);
    for(uint j=0; j<r; ++j)
    {
        col[j] = & m[j][i];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// vector of pointers to elements in the ith row
template<uint r, uint c, typename T>
CINO_INLINE
void mat_row_ptr(const T m[][c], const uint i, T * row[])
{
    assert(i<r);
    for(uint j=0; j<c; ++j)
    {
        row[j] = & m[i][j];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// vector of pointers to elements in the diagonal
template<uint d, typename T>
CINO_INLINE
void mat_diag_ptr(const T m[][d], T * diag[])
{
    for(uint i=0; i<d; ++i)
    for(uint j=0; j<d; ++j)
    {
        diag[i] = & m[i][j];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
bool mat_is_symmetric(const T m[][d])
{
    for(uint i=0;   i<d-1; ++i)
    for(uint j=i+1; j<d;   ++j)
    {
        if(m[i][j]!=m[j][i]) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T mat_trace(const T m[][d])
{
    T res = 0;
    for(uint i=0; i<d; ++i)
    {
        res += m[i][i];
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
T mat_det(const T m[][d])
{
    switch(d)
    {
        case 2: return m[0][0]*m[1][1] - m[1][0]*m[0][1];

        case 3:
        {
            T m0[2][2];
            T m1[2][2];
            T m2[2][2];
            mat_set<2,2,T>(m0, {m[1][1], m[1][2], m[2][1], m[2][2]});
            mat_set<2,2,T>(m1, {m[1][0], m[1][2], m[2][0], m[2][2]});
            mat_set<2,2,T>(m2, {m[1][0], m[1][1], m[2][0], m[2][1]});

            return m[0][0] * mat_det<2,T>(m0) -
                   m[0][1] * mat_det<2,T>(m1) +
                   m[0][2] * mat_det<2,T>(m2);
        }

        default: std::cerr << "mat_determinant: unsupported matrix size" << std::endl;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_transpose(const T m[][c], T tr[][r])
{
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        tr[j][i] = m[i][j];
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
bool mat_inverse(const T m[][d], T in[][d])
{
    switch(d)
    {
        case 2:
        {
            // https://mathworld.wolfram.com/MatrixInverse.html
            // https://www.mathsisfun.com/algebra/matrix-inverse.html
            double one_over_det = 1.0 / mat_det(m);
            in[0][0] =  m[1][1] * one_over_det;
            in[0][1] = -m[0][1] * one_over_det;
            in[1][0] = -m[1][0] * one_over_det;
            in[1][1] =  m[0][0] * one_over_det;
            return one_over_det!=0; // false if the matrix is singular
        }

        default: std::cerr << "mat_inverse: unsupported matrix size" << std::endl;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// matrix eigen values and eigenvectors (from highest to lowest)s
template<uint r, uint c, typename T>
CINO_INLINE
void mat_eigendec(const T m[][c], T eval[], T evec[][c])
{
    assert(r==2 && c==2);

    // http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html

    mat_eigenval<r,c,T>(m, eval);

    if(std::fabs(m[1][0])>1e-5)
    {
        vec_set<2,T>(evec[0], { eval[0]-m[1][1], m[1][0] });
        vec_set<2,T>(evec[1], { eval[1]-m[1][1], m[1][0] });
    }
    else if(std::fabs(m[0][1])>1e-5)
    {
        vec_set<2,T>(evec[0], { m[0][1], eval[0]-m[0][0] });
        vec_set<2,T>(evec[1], { m[0][1], eval[1]-m[0][0] });
    }
    else if(m[0][0]>=m[1][1])
    {
        vec_set<2,T>(evec[0], { 1, 0 });
        vec_set<2,T>(evec[1], { 0, 1 });
    }
    else
    {
        vec_set<2,T>(evec[0], { 0, 1 });
        vec_set<2,T>(evec[1], { 1, 0 });
    }

    vec_normalize<2,T>(evec[0]);
    vec_normalize<2,T>(evec[1]);

//    Eigen::Matrix3d m;
//    m << a00, a01, a02,
//         a10, a11, a12,
//         a20, a21, a22;

//    bool symmetric = (a10==a01) && (a20==a02) && (a21==a12);

//    if(symmetric)
//    {
//        // eigen decomposition for self-adjoint matrices
//        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eig(m);
//        assert(eig.info() == Eigen::Success);

//        v_min = vec3d(eig.eigenvectors()(0,0), eig.eigenvectors()(1,0), eig.eigenvectors()(2,0));
//        v_mid = vec3d(eig.eigenvectors()(0,1), eig.eigenvectors()(1,1), eig.eigenvectors()(2,1));
//        v_max = vec3d(eig.eigenvectors()(0,2), eig.eigenvectors()(1,2), eig.eigenvectors()(2,2));

//        min = eig.eigenvalues()[0];
//        mid = eig.eigenvalues()[1];
//        max = eig.eigenvalues()[2];
//    }
//    else
//    {
//        // eigen decomposition for general matrices
//        Eigen::EigenSolver<Eigen::Matrix3d> eig(m);
//        assert(eig.info() == Eigen::Success);

//        // WARNING: I am taking only the real part!
//        v_min = vec3d(eig.eigenvectors()(0,0).real(), eig.eigenvectors()(1,0).real(), eig.eigenvectors()(2,0).real());
//        v_mid = vec3d(eig.eigenvectors()(0,1).real(), eig.eigenvectors()(1,1).real(), eig.eigenvectors()(2,1).real());
//        v_max = vec3d(eig.eigenvectors()(0,2).real(), eig.eigenvectors()(1,2).real(), eig.eigenvectors()(2,2).real());

//        // WARNING: I am taking only the real part!
//        min = eig.eigenvalues()[0].real();
//        mid = eig.eigenvalues()[1].real();
//        max = eig.eigenvalues()[2].real();
//    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// matrix eigen values (from highest to lowest)
template<uint r, uint c, typename T>
CINO_INLINE
void mat_eigenval(const T m[][c], T eval[])
{
    assert(r==2 && c==2);

    // https://lucidar.me/en/mathematics/singular-value-decomposition-of-a-2x2-matrix/

    T m00_2 = m[0][0]*m[0][0];
    T m01_2 = m[0][1]*m[0][1];
    T m10_2 = m[1][0]*m[1][0];
    T m11_2 = m[1][1]*m[0][0];
    T s1    = m00_2 + m01_2 + m10_2 + m11_2;
    T s2    = sqrt(std::pow(m00_2 + m01_2 - m10_2 - m11_2,2) + 4*pow(m[0][0]*m[1][0] + m[0][1]*m[1][1],2));
    eval[0] = sqrt((s1+s2)*0.5);
    eval[1] = sqrt((s1-s2)*0.5);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_eigenvec(const T m[][c], T evec[][c])
{
    assert(r==2 && c==2);

    T eval[2];
    mat_eigendec<r,c,T>(m, eval, evec);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_svd(const T m[][c], T S[][r], T V[], T D[][c])
{
    assert(false && "TODO");
    mat_ssvd<r,c,T>(m,S,V,D);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_ssvd(const T m[][c], T S[][r], T V[], T D[][c])
{
    assert(false && "TODO");
    mat_svd<r,c,T>(m,S,V,D);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint d, typename T>
CINO_INLINE
void mat_solve_Cramer(const T m[][d], const T b[], T x[])
{
    T det = mat_det<d,T>(m);
    if(det==0)
    {
        vec_set_dense<d,T>(x,0);
        return;
    }
    for(uint i=0; i<d; ++i)
    {
        T m_i[d][d];
        mat_copy<d,d,T>(m, m_i);
        mat_set_col<d,d,T>(m_i, i, b);
        x[i] = mat_det<d,T>(m_i) / det;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_copy(const T m1[][c], T m2[][c])
{
    std::copy(m1[0], m1[0]+(r*c), m2[0]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r, uint c, typename T>
CINO_INLINE
void mat_print(const T m[][c])
{
    for(uint i=0; i<r; ++i)
    for(uint j=0; j<c; ++j)
    {
        if(i>0 && j%c==0) std::cout << "\n";
        std::cout << m[i][j] << " ";
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<uint r0, uint c0, uint c1, typename T>
CINO_INLINE
void mat_times(const T m0[][c0], const T m1[][c1], T m2[][c1])
{
    for(uint i=0; i<r0; ++i)
    for(uint j=0; j<c1; ++j)
    {
        m2[i][j] = 0;
        for(uint k=0; k<c0; ++k)
        {
            m2[i][j] += m0[i][k] * m1[k][j];
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}
