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
#include <cinolib/simplicial_map.h>
#include <Eigen/Dense>

namespace cinolib
{

CINO_INLINE
void affine_simplicial_map(const vec3d & A0,
                           const vec3d & A1,
                           const vec3d & A2,
                           const vec3d & A3,
                           const vec3d & B0,
                           const vec3d & B1,
                           const vec3d & B2,
                           const vec3d & B3,
                           double        m[3][3],
                           double        t[3])
{
    Eigen::VectorXd rhs(12);
    Eigen::MatrixXd M;
    M.setZero(12,12);

    M.coeffRef( 0, 0) = A0.x();
    M.coeffRef( 0, 1) = A0.y();
    M.coeffRef( 0, 2) = A0.z();
    M.coeffRef( 0, 3) = 1.0;
    M.coeffRef( 1, 4) = A0.x();
    M.coeffRef( 1, 5) = A0.y();
    M.coeffRef( 1, 6) = A0.z();
    M.coeffRef( 1, 7) = 1.0;
    M.coeffRef( 2, 8) = A0.x();
    M.coeffRef( 2, 9) = A0.y();
    M.coeffRef( 2,10) = A0.z();
    M.coeffRef( 2,11) = 1.0;

    M.coeffRef( 3, 0) = A1.x();
    M.coeffRef( 3, 1) = A1.y();
    M.coeffRef( 3, 2) = A1.z();
    M.coeffRef( 3, 3) = 1.0;
    M.coeffRef( 4, 4) = A1.x();
    M.coeffRef( 4, 5) = A1.y();
    M.coeffRef( 4, 6) = A1.z();
    M.coeffRef( 4, 7) = 1.0;
    M.coeffRef( 5, 8) = A1.x();
    M.coeffRef( 5, 9) = A1.y();
    M.coeffRef( 5,10) = A1.z();
    M.coeffRef( 5,11) = 1.0;

    M.coeffRef( 6, 0) = A2.x();
    M.coeffRef( 6, 1) = A2.y();
    M.coeffRef( 6, 2) = A2.z();
    M.coeffRef( 6, 3) = 1.0;
    M.coeffRef( 7, 4) = A2.x();
    M.coeffRef( 7, 5) = A2.y();
    M.coeffRef( 7, 6) = A2.z();
    M.coeffRef( 7, 7) = 1.0;
    M.coeffRef( 8, 8) = A2.x();
    M.coeffRef( 8, 9) = A2.y();
    M.coeffRef( 8,10) = A2.z();
    M.coeffRef( 8,11) = 1.0;

    M.coeffRef( 9, 0) = A3.x();
    M.coeffRef( 9, 1) = A3.y();
    M.coeffRef( 9, 2) = A3.z();
    M.coeffRef( 9, 3) = 1.0;
    M.coeffRef(10, 4) = A3.x();
    M.coeffRef(10, 5) = A3.y();
    M.coeffRef(10, 6) = A3.z();
    M.coeffRef(10, 7) = 1.0;
    M.coeffRef(11, 8) = A3.x();
    M.coeffRef(11, 9) = A3.y();
    M.coeffRef(11,10) = A3.z();
    M.coeffRef(11,11) = 1.0;

    rhs( 0) = B0.x();
    rhs( 1) = B0.y();
    rhs( 2) = B0.z();
    rhs( 3) = B1.x();
    rhs( 4) = B1.y();
    rhs( 5) = B1.z();
    rhs( 6) = B2.x();
    rhs( 7) = B2.y();
    rhs( 8) = B2.z();
    rhs( 9) = B3.x();
    rhs(10) = B3.y();
    rhs(11) = B3.z();

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(M, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::VectorXd x = svd.solve(rhs);

    m[0][0]=x(0);   m[0][1]=x(1);   m[0][2]=x(2);   t[0]=x(3);
    m[1][0]=x(4);   m[1][1]=x(5);   m[1][2]=x(6);   t[1]=x(7);
    m[2][0]=x(8);   m[2][1]=x(9);   m[2][2]=x(10);  t[2]=x(11);

    // safety checks
    //
    //#ifndef NDEBUG
    //vec3d test0 = A0;
    //vec3d test1 = A1;
    //vec3d test2 = A2;
    //vec3d test3 = A3;
    //transform(test0, m); test0 += vec3d(t[0], t[1], t[2]);
    //transform(test1, m); test1 += vec3d(t[0], t[1], t[2]);
    //transform(test2, m); test2 += vec3d(t[0], t[1], t[2]);
    //transform(test3, m); test3 += vec3d(t[0], t[1], t[2]);
    //if((B0 - test0).length() > 1e-10) std::cout << (B0 - test0).length() << "\t" << tet_scaled_jacobian(A0,A1,A2,A3) << "\t" << tet_scaled_jacobian(B0,B1,B2,B3) << std::endl;
    //if((B1 - test1).length() > 1e-10) std::cout << (B1 - test1).length() << "\t" << tet_scaled_jacobian(A0,A1,A2,A3) << "\t" << tet_scaled_jacobian(B0,B1,B2,B3) << std::endl;
    //if((B2 - test2).length() > 1e-10) std::cout << (B2 - test2).length() << "\t" << tet_scaled_jacobian(A0,A1,A2,A3) << "\t" << tet_scaled_jacobian(B0,B1,B2,B3) << std::endl;
    //if((B3 - test3).length() > 1e-10) std::cout << (B3 - test3).length() << "\t" << tet_scaled_jacobian(A0,A1,A2,A3) << "\t" << tet_scaled_jacobian(B0,B1,B2,B3) << std::endl;
    //assert((B0 - test0).length() < 1e-10);
    //assert((B1 - test1).length() < 1e-10);
    //assert((B2 - test2).length() < 1e-10);
    //assert((B3 - test3).length() < 1e-10);
    //#endif
}

}
