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
#include <cinolib/stretch_tensor.h>
#include <cinolib/tangent_space.h>
#include <cinolib/matrix.h>
#include <Eigen/Dense>

namespace cinolib
{

// Starting from a reference triangle <p0,p1,p2> and a
// deformed version of it <q0,q1,q2>, this method first
// computes the associated deformation gradient F, and
// subsequently the stretch tensor U defined as the square
// root of the Right Cauchy-Green Deformation Tensor C. it
// returns the eigenvalues and eigenvectors of the stretch
// tensor U.
//
// These are all concepts and notations coming from continuum
// mechanics. You may want to refer to the following papers/books
// for some theoretical background:
//
//     Nonlinear Continuum Mechanics for Finite Element Analysis
//     Javier Bonet and Richard D.Wood
//     Cambridge University Press, 1997
//
//     https://en.wikiversity.org/wiki/Continuum_mechanics/Polar_decomposition
//
//     http://www.brown.edu/Departments/Engineering/Courses/En221/Notes/Kinematics/Kinematics.htm
//
// For some examples of computer graphics algorithm exploiting
// these concepts, you may also refer to:
//
// Real-Time Dynamic Wrinkling of Coarse Animated Cloth
// R.Gillette, C.Peters, N.Vining, E.Edwards, A.Sheffer
// Symposium on Computer Animation, 2015
//
CINO_INLINE
void stretch_tensor(const vec3d  & p0,    // reference triangle <p0,p1,p2>
                    const vec3d  & p1,    //
                    const vec3d  & p2,    //
                    const vec3d  & q0,    // deformed triangle <q0,q1,q2>
                    const vec3d  & q1,    //
                    const vec3d  & q2,    //
                          vec2d  & v_min, // eigenvector of the minimum eigenvalue
                          vec2d  & v_max, // eigenvector of the maximum eigenvalue
                          double & min,   // minimum eigenvalue
                          double & max)   // maximum eigenvalue
{
    // express triangle vertices as 2D coordinates
    vec2d P0,P1,P2,Q0,Q1,Q2;
    tangent_space_2d_coords(p0, p1, p2, P0, P1, P2);
    tangent_space_2d_coords(q0, q1, q2, Q0, Q1, Q2);

    //std::cout << "P0: " << P0 << std::endl;
    //std::cout << "P1: " << P1 << std::endl;
    //std::cout << "P2: " << P2 << std::endl;
    //std::cout << "Q0: " << Q0 << std::endl;
    //std::cout << "Q1: " << Q1 << std::endl;
    //std::cout << "Q2: " << Q2 << std::endl;

    // compute deformation gradient F
    vec2d u1_ref = P1-P0;
    vec2d u2_ref = P2-P0;
    vec2d u1     = Q1-Q0;
    vec2d u2     = Q2-Q0;
    //
    Eigen::Matrix<double,2,2> U_ref;
    U_ref(0,0)=u1_ref.x();  U_ref(0,1)=u2_ref.x();
    U_ref(1,0)=u1_ref.y();  U_ref(1,1)=u2_ref.y();
    //
    Eigen::Matrix<double,2,2> U;
    U(0,0)=u1.x();  U(0,1)=u2.x();
    U(1,0)=u1.y();  U(1,1)=u2.y();
    //
    Eigen::Matrix<double,2,2> F = U * U_ref.inverse();

    // right Green Cauchy deformation tensor C (multiplying F by its transpose cancels out rotations)
    Eigen::Matrix<double,2,2> C = F.transpose() * F;

    // Stretch tensor is defined as: U = sqrt(C).
    // Here I compute the eigenvectors and eigenvalues of sqrt(C)
    // by means of spectral decomposition and eigenprojection
    // https://en.wikiversity.org/wiki/Continuum_mechanics/Polar_decomposition
    eigen_decomposition_2x2(C(0,0), C(0,1), C(1,0), C(1,1), v_min, v_max, min, max);
    min = sqrt(min);
    max = sqrt(max);
}

}
