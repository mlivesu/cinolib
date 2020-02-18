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
bool tet_barycentric_coords(const vec3d & A,
                            const vec3d & B,
                            const vec3d & C,
                            const vec3d & D,
                            const vec3d & P,
                            std::vector<double> & wgts,
                            const double  tol)
{
    wgts = std::vector<double>(4, 0.0);

    //Eigen::Matrix<double,4,4> M;
    //M(0,0) = A[0]; M(0,1) = A[1]; M(0,2) = A[2]; M(0,3) = 1;
    //M(1,0) = B[0]; M(1,1) = B[1]; M(1,2) = B[2]; M(1,3) = 1;
    //M(2,0) = C[0]; M(2,1) = C[1]; M(2,2) = C[2]; M(2,3) = 1;
    //M(3,0) = D[0]; M(3,1) = D[1]; M(3,2) = D[2]; M(3,3) = 1;

    Eigen::Matrix<double,4,4> M0;
    M0(0,0) = P[0]; M0(0,1) = P[1]; M0(0,2) = P[2]; M0(0,3) = 1;
    M0(1,0) = B[0]; M0(1,1) = B[1]; M0(1,2) = B[2]; M0(1,3) = 1;
    M0(2,0) = C[0]; M0(2,1) = C[1]; M0(2,2) = C[2]; M0(2,3) = 1;
    M0(3,0) = D[0]; M0(3,1) = D[1]; M0(3,2) = D[2]; M0(3,3) = 1;

    Eigen::Matrix<double,4,4> M1;
    M1(0,0) = A[0]; M1(0,1) = A[1]; M1(0,2) = A[2]; M1(0,3) = 1;
    M1(1,0) = P[0]; M1(1,1) = P[1]; M1(1,2) = P[2]; M1(1,3) = 1;
    M1(2,0) = C[0]; M1(2,1) = C[1]; M1(2,2) = C[2]; M1(2,3) = 1;
    M1(3,0) = D[0]; M1(3,1) = D[1]; M1(3,2) = D[2]; M1(3,3) = 1;

    Eigen::Matrix<double,4,4> M2;
    M2(0,0) = A[0]; M2(0,1) = A[1]; M2(0,2) = A[2]; M2(0,3) = 1;
    M2(1,0) = B[0]; M2(1,1) = B[1]; M2(1,2) = B[2]; M2(1,3) = 1;
    M2(2,0) = P[0]; M2(2,1) = P[1]; M2(2,2) = P[2]; M2(2,3) = 1;
    M2(3,0) = D[0]; M2(3,1) = D[1]; M2(3,2) = D[2]; M2(3,3) = 1;

    Eigen::Matrix<double,4,4> M3;
    M3(0,0) = A[0]; M3(0,1) = A[1]; M3(0,2) = A[2]; M3(0,3) = 1;
    M3(1,0) = B[0]; M3(1,1) = B[1]; M3(1,2) = B[2]; M3(1,3) = 1;
    M3(2,0) = C[0]; M3(2,1) = C[1]; M3(2,2) = C[2]; M3(2,3) = 1;
    M3(3,0) = P[0]; M3(3,1) = P[1]; M3(3,2) = P[2]; M3(3,3) = 1;

    //double det_M  = M.determinant();
    double det_M0 = M0.determinant();
    double det_M1 = M1.determinant();
    double det_M2 = M2.determinant();
    double det_M3 = M3.determinant();
    double sum    = det_M0 + det_M1 + det_M2 + det_M3;

    if(sum==0) return false; // degenerate

    wgts[0] = det_M0/sum; assert(!std::isnan(wgts[0]));
    wgts[1] = det_M1/sum; assert(!std::isnan(wgts[1]));
    wgts[2] = det_M2/sum; assert(!std::isnan(wgts[2]));
    wgts[3] = det_M3/sum; assert(!std::isnan(wgts[3]));

    for(double w : wgts) if (w < -tol || w > 1.0+tol) return false; // outside
    return true; // inside
}


//CINO_INLINE
//void tet_closest_vertex(const vec3d  & A,
//                        const vec3d  & B,
//                        const vec3d  & C,
//                        const vec3d  & D,
//                        const vec3d  & query,
//                              uint   & id,
//                              double & dist)
//{
//    vec3d tet[4] = { A, B, C, D };

//    std::set< std::pair<double,int> > sorted_verts;
//    for(uint i=0; i<4; ++i)
//    {
//        sorted_verts.insert(std::make_pair(tet[i].dist(query),i));
//    }

//    dist = (*sorted_verts.begin()).first;
//    id   = (*sorted_verts.begin()).second;
//}


//CINO_INLINE
//void tet_closest_edge(const vec3d  & A,
//                      const vec3d  & B,
//                      const vec3d  & C,
//                      const vec3d  & D,
//                      const vec3d  & query,
//                            uint   & id,
//                            double & dist)
//{
//    vec3d tet[4] = { A, B, C, D };

//    std::set< std::pair<double,uint>> sorted_segs;
//    for(uint i=0; i<6; ++i)
//    {
//        Segment s(tet[TET_EDGES[i][0]], tet[TET_EDGES[i][1]]);
//        sorted_segs.insert(std::make_pair(s.dist(query),i));
//    }

//    dist = (*sorted_segs.begin()).first;
//    id   = (*sorted_segs.begin()).second;
//}

}

