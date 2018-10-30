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
#include <cinolib/matrix.h>
#include <Eigen/Eigenvalues>

namespace cinolib
{

// http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
//
CINO_INLINE
void eigen_decomposition_2x2(const double   a00,
                             const double   a01,
                             const double   a10,
                             const double   a11,
                                   vec2d  & v_min, // eigenvectors
                                   vec2d  & v_max,
                                   double & min,   // eigenvalues
                                   double & max)
{
    eigenvalues_2x2(a00,a01,a10,a11,min,max);

    if(std::fabs(a10)>1e-5)
    {
        v_max = vec2d(max-a11,a10);
        v_min = vec2d(min-a11,a10);
    }
    else if(std::fabs(a01)>1e-5)
    {
        v_max = vec2d(a01,max-a00);
        v_min = vec2d(a01,min-a00);
    }
    else
    {
        v_max = (a00>=a11) ? vec2d(1,0) : vec2d(0,1);
        v_min = (a00>=a11) ? vec2d(0,1) : vec2d(1,0);
    }

    v_max.normalize();
    v_min.normalize();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
//
CINO_INLINE
void eigenvalues_2x2(const double   a00,
                     const double   a01,
                     const double   a10,
                     const double   a11,
                           double & min,
                           double & max)
{
    double T = a00 + a11; // trace
    double D = determinant_2x2(a00,a01,a10,a11);

    min = T/2.0 - sqrt(T*T/4.0-D);
    max = T/2.0 + sqrt(T*T/4.0-D);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigenvectors_2x2(const double   a00,
                      const double   a01,
                      const double   a10,
                      const double   a11,
                            vec2d  & v_min,
                            vec2d  & v_max)
{
    double min, max;
    eigen_decomposition_2x2(a00, a01, a10, a11, v_min, v_max, min, max);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double determinant_2x2(const double a00, const double a01, const double a10, const double a11)
{
    return ((a00*a11) - (a10*a01));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double determinant_2x2(const vec2d a0, const vec2d a1)
{
    return determinant_2x2(a0[0], a0[1], a1[0], a1[1]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigen_decomposition_3x3(const double   a[3][3],
                                   vec3d  & v_min, // eigenvectors
                                   vec3d  & v_mid,
                                   vec3d  & v_max,
                                   double & min,   // eigenvalues
                                   double & mid,
                                   double & max)
{
    eigen_decomposition_3x3(a[0][0], a[0][1], a[0][2],
                            a[1][0], a[1][1], a[1][2],
                            a[2][0], a[2][1], a[2][2],
                            v_min, v_mid, v_max,
                            min, mid, max);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigen_decomposition_3x3(const double   a00,
                             const double   a01,
                             const double   a02,
                             const double   a10,
                             const double   a11,
                             const double   a12,
                             const double   a20,
                             const double   a21,
                             const double   a22,
                                   vec3d  & v_min, // eigenvectors
                                   vec3d  & v_mid,
                                   vec3d  & v_max,
                                   double & min,   // eigenvalues
                                   double & mid,
                                   double & max)
{
    Eigen::Matrix3d m;
    m << a00, a01, a02,
         a10, a11, a12,
         a20, a21, a22;

    bool symmetric = (a10==a01) && (a20==a02) && (a21==a12);

    if(symmetric)
    {
        // eigen decomposition for self-adjoint matrices
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eig(m);
        assert(eig.info() == Eigen::Success);

        v_min = vec3d(eig.eigenvectors()(0,0), eig.eigenvectors()(1,0), eig.eigenvectors()(2,0));
        v_mid = vec3d(eig.eigenvectors()(0,1), eig.eigenvectors()(1,1), eig.eigenvectors()(2,1));
        v_max = vec3d(eig.eigenvectors()(0,2), eig.eigenvectors()(1,2), eig.eigenvectors()(2,2));

        min = eig.eigenvalues()[0];
        mid = eig.eigenvalues()[1];
        max = eig.eigenvalues()[2];
    }
    else
    {
        // eigen decomposition for general matrices
        Eigen::EigenSolver<Eigen::Matrix3d> eig(m);
        assert(eig.info() == Eigen::Success);

        // WARNING: I am taking only the real part!
        v_min = vec3d(eig.eigenvectors()(0,0).real(), eig.eigenvectors()(1,0).real(), eig.eigenvectors()(2,0).real());
        v_mid = vec3d(eig.eigenvectors()(0,1).real(), eig.eigenvectors()(1,1).real(), eig.eigenvectors()(2,1).real());
        v_max = vec3d(eig.eigenvectors()(0,2).real(), eig.eigenvectors()(1,2).real(), eig.eigenvectors()(2,2).real());

        // WARNING: I am taking only the real part!
        min = eig.eigenvalues()[0].real();
        mid = eig.eigenvalues()[1].real();
        max = eig.eigenvalues()[2].real();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigenvalues_3x3(const double   a00,
                     const double   a01,
                     const double   a02,
                     const double   a10,
                     const double   a11,
                     const double   a12,
                     const double   a20,
                     const double   a21,
                     const double   a22,
                           double & min,
                           double & mid,
                           double & max)
{
    vec3d v_min, v_mid, v_max;
    eigen_decomposition_3x3(a00, a01, a02, a10, a11, a12, a20, a21, a22, v_min, v_mid, v_max, min, mid, max);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void eigenvectors_3x3(const double   a00,
                      const double   a01,
                      const double   a02,
                      const double   a10,
                      const double   a11,
                      const double   a12,
                      const double   a20,
                      const double   a21,
                      const double   a22,
                            vec3d  & v_min,
                            vec3d  & v_mid,
                            vec3d  & v_max)
{
    double min, mid, max;
    eigen_decomposition_3x3(a00, a01, a02, a10, a11, a12, a20, a21, a22, v_min, v_mid, v_max, min, mid, max);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double determinant_3x3(const double a00, const double a01, const double a02,
                       const double a10, const double a11, const double a12,
                       const double a20, const double a21, const double a22)
{
    return a00 * determinant_2x2(a11, a12, a21, a22) -
           a01 * determinant_2x2(a10, a12, a20, a22) +
           a02 * determinant_2x2(a10, a11, a20, a21);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_std_3x3_to_Eigen_3x3(const double stdM[3][3], Eigen::Matrix3d & eigenM)
{
    eigenM.coeffRef(0,0) = stdM[0][0];  eigenM.coeffRef(0,1) = stdM[0][1];  eigenM.coeffRef(0,2) = stdM[0][2];
    eigenM.coeffRef(1,0) = stdM[1][0];  eigenM.coeffRef(1,1) = stdM[1][1];  eigenM.coeffRef(1,2) = stdM[1][2];
    eigenM.coeffRef(2,0) = stdM[2][0];  eigenM.coeffRef(2,1) = stdM[2][1];  eigenM.coeffRef(2,2) = stdM[2][2];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void from_eigen_3x3_to_std_3x3(const Eigen::Matrix3d & eigenM, double stdM[3][3])
{
    stdM[0][0] = eigenM.coeffRef(0,0);  stdM[0][1] = eigenM.coeffRef(0,1);  stdM[0][2] = eigenM.coeffRef(0,2);
    stdM[1][0] = eigenM.coeffRef(1,0);  stdM[1][1] = eigenM.coeffRef(1,1);  stdM[1][2] = eigenM.coeffRef(1,2);
    stdM[2][0] = eigenM.coeffRef(2,0);  stdM[2][1] = eigenM.coeffRef(2,1);  stdM[2][2] = eigenM.coeffRef(2,2);
}

}
