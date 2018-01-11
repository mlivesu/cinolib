/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/matrix.h>
#include <Eigen/Dense>

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
