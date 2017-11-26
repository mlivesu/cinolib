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
#include <cinolib/geometry/tetrahedron.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/segment.h>

#include <Eigen/Dense>
#include <set>

namespace cinolib
{

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

    if (sum==0) return false; // degenerate

    wgts[0] = det_M0/sum; assert(!std::isnan(wgts[0]));
    wgts[1] = det_M1/sum; assert(!std::isnan(wgts[1]));
    wgts[2] = det_M2/sum; assert(!std::isnan(wgts[2]));
    wgts[3] = det_M3/sum; assert(!std::isnan(wgts[3]));

    for(double w : wgts) if (w < -tol || w > 1.0+tol) return false; // outside
    return true; // inside
}


CINO_INLINE
void tet_closest_vertex(const vec3d  & A,
                        const vec3d  & B,
                        const vec3d  & C,
                        const vec3d  & D,
                        const vec3d  & query,
                              uint   & id,
                              double & dist)
{
    vec3d tet[4] = { A, B, C, D };

    std::set< std::pair<double,int> > sorted_verts;
    for(uint i=0; i<4; ++i)
    {
        sorted_verts.insert(std::make_pair(tet[i].dist(query),i));
    }

    dist = (*sorted_verts.begin()).first;
    id   = (*sorted_verts.begin()).second;
}


CINO_INLINE
void tet_closest_edge(const vec3d  & A,
                      const vec3d  & B,
                      const vec3d  & C,
                      const vec3d  & D,
                      const vec3d  & query,
                            uint   & id,
                            double & dist)
{
    vec3d tet[4] = { A, B, C, D };

    std::set< std::pair<double,uint>> sorted_segs;
    for(uint i=0; i<6; ++i)
    {
        Segment s(tet[TET_EDGES[i][0]], tet[TET_EDGES[i][1]]);
        sorted_segs.insert(std::make_pair(s.dist_to_point(query),i));
    }

    dist = (*sorted_segs.begin()).first;
    id   = (*sorted_segs.begin()).second;
}

}

