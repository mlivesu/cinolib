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
#include <cinolib/geometry/tetrahedron_utils.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/geometry/segment.h>
#include <cinolib/Moller_Trumbore_intersection.h>
#include <cinolib/geometry/triangle_utils.h>
#include <Eigen/Dense>
#include <set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Given a point P and a tetrahedron ABCD, finds the point in ABCD that
// is closest to P. This code was taken directly from Ericson's seminal
// book "Real Time Collision Detection", Section 5.1.6
//
vec3d tetrahedron_closest_point(const vec3d & P,
                                const vec3d & A,
                                const vec3d & B,
                                const vec3d & C,
                                const vec3d & D)
{
    // start assuming P is inside the tet, so closest to itself
    vec3d  best_point = P;
    double best_dist  = inf_double;

    // if point is outside face ACB, then compute closest point on ACB
    vec3d n_ACB = (C-A).cross(B-A);
    //std::cout << "\t" << n_ACB.dot(P-A) << std::endl;
    if(n_ACB.dot(P-A)>0)
    {
        vec3d  q = triangle_closest_point(P,A,C,B);
        double d = q.dist_squared(P);
        if(d < best_dist)
        {
            best_dist  = d;
            best_point = q;
        }
    }

    // repeat test for face ADC
    vec3d n_ADC = (D-A).cross(C-A);
    //std::cout << "\t" << n_ADC.dot(P-A) << std::endl;
    if(n_ADC.dot(P-A)>0)
    {
        vec3d  q = triangle_closest_point(P,A,D,C);
        double d = q.dist_squared(P);
        if(d < best_dist)
        {
            best_dist  = d;
            best_point = q;
        }
    }

    // Repeat test for face ABD
    vec3d n_ABD = (B-A).cross(D-A);
    //std::cout << "\t" << n_ABD.dot(P-A) << std::endl;
    if(n_ABD.dot(P-A)>0)
    {
        vec3d  q = triangle_closest_point(P,A,B,D);
        double d = q.dist_squared(P);
        if(d < best_dist)
        {
            best_dist  = d;
            best_point = q;
        }
    }

    // Repeat test for face BCD
    vec3d n_BCD = (C-B).cross(D-B);
    //std::cout << "\t" << n_BDC.dot(P-A) << std::endl;
    if(n_BCD.dot(P-B)>0)
    {
        vec3d  q = triangle_closest_point(P,B,C,D);
        double d = q.dist_squared(P);
        if(d < best_dist)
        {
            best_dist  = d;
            best_point = q;
        }
    }

    return best_point;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://steve.hollasch.net/cgindex/geometry/ptintet.html
//
CINO_INLINE
void tet_barycentric_coords(const vec3d & t0,
                            const vec3d & t1,
                            const vec3d & t2,
                            const vec3d & t3,
                            const vec3d & p,
                            double wgts[])
{
    Eigen::Matrix<double,4,4> m0;
    m0(0,0) = p[0];  m0(0,1) = p[1];  m0(0,2) = p[2];  m0(0,3) = 1;
    m0(1,0) = t1[0]; m0(1,1) = t1[1]; m0(1,2) = t1[2]; m0(1,3) = 1;
    m0(2,0) = t2[0]; m0(2,1) = t2[1]; m0(2,2) = t2[2]; m0(2,3) = 1;
    m0(3,0) = t3[0]; m0(3,1) = t3[1]; m0(3,2) = t3[2]; m0(3,3) = 1;

    Eigen::Matrix<double,4,4> m1;
    m1(0,0) = t0[0]; m1(0,1) = t0[1]; m1(0,2) = t0[2]; m1(0,3) = 1;
    m1(1,0) = p[0];  m1(1,1) = p[1];  m1(1,2) = p[2];  m1(1,3) = 1;
    m1(2,0) = t2[0]; m1(2,1) = t2[1]; m1(2,2) = t2[2]; m1(2,3) = 1;
    m1(3,0) = t3[0]; m1(3,1) = t3[1]; m1(3,2) = t3[2]; m1(3,3) = 1;

    Eigen::Matrix<double,4,4> m2;
    m2(0,0) = t0[0]; m2(0,1) = t0[1]; m2(0,2) = t0[2]; m2(0,3) = 1;
    m2(1,0) = t1[0]; m2(1,1) = t1[1]; m2(1,2) = t1[2]; m2(1,3) = 1;
    m2(2,0) = p[0];  m2(2,1) = p[1];  m2(2,2) = p[2];  m2(2,3) = 1;
    m2(3,0) = t3[0]; m2(3,1) = t3[1]; m2(3,2) = t3[2]; m2(3,3) = 1;

    Eigen::Matrix<double,4,4> m3;
    m3(0,0) = t0[0]; m3(0,1) = t0[1]; m3(0,2) = t0[2]; m3(0,3) = 1;
    m3(1,0) = t1[0]; m3(1,1) = t1[1]; m3(1,2) = t1[2]; m3(1,3) = 1;
    m3(2,0) = t2[0]; m3(2,1) = t2[1]; m3(2,2) = t2[2]; m3(2,3) = 1;
    m3(3,0) = p[0];  m3(3,1) = p[1];  m3(3,2) = p[2];  m3(3,3) = 1;

    //double det_M  = M.determinant();
    double det_m0 = m0.determinant();
    double det_m1 = m1.determinant();
    double det_m2 = m2.determinant();
    double det_m3 = m3.determinant();
    double sum    = det_m0 + det_m1 + det_m2 + det_m3;

    if(sum==0) // degenerate
    {
        wgts[0] = inf_double;
        wgts[1] = inf_double;
        wgts[2] = inf_double;
        wgts[3] = inf_double;
        return;
    }

    wgts[0] = det_m0/sum; assert(!std::isnan(wgts[0]));
    wgts[1] = det_m1/sum; assert(!std::isnan(wgts[1]));
    wgts[2] = det_m2/sum; assert(!std::isnan(wgts[2]));
    wgts[3] = det_m3/sum; assert(!std::isnan(wgts[3]));
}

}

