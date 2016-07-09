/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include "intersection.h"

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/algorithms/intersection.hpp>

#include <eigen3/Eigen/Dense>

namespace cinolib
{
namespace intersection
{

namespace bg = boost::geometry;
typedef   bg::model::point<double,3,bg::cs::cartesian> Point;
typedef   bg::model::segment<Point>                    Segment;

CINO_INLINE
bool segment2D(const vec2d        & s0_beg,
               const vec2d        & s0_end,
               const vec2d        & s1_beg,
               const vec2d        & s1_end,
               std::vector<vec2d> & inters)
{
    assert(inters.empty());

    std::vector<Point> res;
    bg::intersection(Segment(Point(s0_beg.x(), s0_beg.y()),
                             Point(s0_end.x(), s0_end.y())),
                     Segment(Point(s1_beg.x(), s1_beg.y()),
                             Point(s1_end.x(), s1_end.y())),
                     res);

    // if s1 and s2 are colinear returns the endpoints of the shared portion
    //
    for(Point p : res) inters.push_back(vec2d(p.get<0>(), p.get<1>()));

    return !inters.empty();
}


//CINO_INLINE
//bool lines3D(const vec3d   l0_pos,
//             const vec3d   l0_dir,
//             const vec3d   l1_pos,
//             const vec3d   l1_dir,
//                   vec3d & inters)
//{
//    assert(l0_dir.length() == 1);
//    assert(l1_dir.length() == 1);

//    if (l0_dir.dot(l1_dir) == 1) return false;

//    vec3d n00( -l0_dir.y(),  l0_dir.x(),          0); n00.normalize();
//    vec3d n01(           0, -l0_dir.z(), l0_dir.y()); n01.normalize();
//    vec3d n10( -l1_dir.y(),  l1_dir.x(),          0); n10.normalize();
//    vec3d n11(           0, -l1_dir.z(), l1_dir.y()); n11.normalize();

//    assert(n00.dot(l0_dir) == 0);
//    assert(n01.dot(l0_dir) == 0);
//    assert(n10.dot(l1_dir) == 0);
//    assert(n11.dot(l1_dir) == 0);

//    Plane P00(l0_pos, n00);
//    Plane P01(l0_pos, n01);
//    Plane P10(l1_pos, n10);
//    Plane P11(l1_pos, n11);

//    Eigen::Matrix<double,4,4> A;
//    Eigen::Vector4d b;
//    A(0,0) = P00.n[0]; A(0,1) = P00.n[1]; A(0,2) = P00.n[2]; b[0] = P00.d;
//    A(1,0) = P01.n[0]; A(1,1) = P01.n[1]; A(1,2) = P01.n[2]; b[1] = P01.d;
//    A(2,0) = P10.n[0]; A(2,1) = P10.n[1]; A(2,2) = P10.n[2]; b[2] = P10.d;
//    A(3,0) = P11.n[0]; A(3,1) = P11.n[1]; A(3,2) = P11.n[2]; b[3] = P11.d;
//    assert(A.determinant() != 0);

//    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> solver(A);
//    assert(solver.info() == Eigen::Success);
//    Eigen::Vector3d x = solver.solve(b);
//    inters = vec3d(x[0],x[1],x[2]);

//    return true;
//}


//CINO_INLINE
//bool planes(const std::vector<Plane> & planes, vec3d & inters)
//{
//    Eigen::MatrixXd A(planes.size(), 3);
//    Eigen::VectorXd b(planes.size());

//    int row = 0;
//    for(const Plane p : planes)
//    {
//        A.coeffRef(row,0) = p.n[0];
//        A.coeffRef(row,1) = p.n[1];
//        A.coeffRef(row,2) = p.n[2];
//        b[row] = p.d;
//        ++row;
//    }

//    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> solver(A);
//    assert(solver.info() == Eigen::Success);
//    Eigen::Vector3d x = solver.solve(b);

//    inters = vec3d(x[0],x[1],x[2]);
//    return true;
//}

}
}
