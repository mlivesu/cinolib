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
#include "intersection.h"

#include <cinolib/geometry/triangle.h>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/algorithms/intersection.hpp>

#include <Eigen/Dense>

namespace cinolib
{

namespace bg = boost::geometry;
typedef   bg::model::point<double,3,bg::cs::cartesian> Point;
typedef   bg::model::segment<Point>                    Segment2D;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool segment2D_intersection(const vec2d        & s0_beg,
                            const vec2d        & s0_end,
                            const vec2d        & s1_beg,
                            const vec2d        & s1_end,
                            std::vector<vec2d> & inters)
{
    assert(inters.empty());

    std::vector<Point> res;
    bg::intersection(Segment2D(Point(s0_beg.x(), s0_beg.y()),
                               Point(s0_end.x(), s0_end.y())),
                     Segment2D(Point(s1_beg.x(), s1_beg.y()),
                               Point(s1_end.x(), s1_end.y())),
                     res);

    // if s1 and s2 are colinear returns the endpoints of the shared portion
    //
    for(Point p : res) inters.push_back(vec2d(p.get<0>(), p.get<1>()));

    return !inters.empty();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool line_triangle_intersection(const Line  & l,
                                const vec3d & V0,
                                const vec3d & V1,
                                const vec3d & V2,
                                      vec3d & inters,
                                const double  tol)
{
    std::vector<Plane> planes = l.to_planes();
    planes.push_back(Plane(V0,V1,V2));

    if (least_squares_intersection(planes, inters))
    {
        std::vector<double> wgts;
        if (triangle_barycentric_coords(V0,V1,V2,inters, wgts, tol)) return true;
        return false;
    }    
    assert(false && "Something is off here...");
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool ray_triangle_intersection(const Ray   & r,
                               const vec3d & V0,
                               const vec3d & V1,
                               const vec3d & V2,
                                     vec3d & inters,
                               const double  tol)
{
    Line l(r.begin(), r.begin() + r.dir());
    if (line_triangle_intersection(l, V0, V1, V2, inters, tol))
    {
        vec3d u = inters - r.begin();
        if (u.dot(r.dir()) < 0) return false;
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool least_squares_intersection(const std::vector<Plane> & planes, vec3d & inters)
{
    if (planes.size() < 3) return false;

    Eigen::MatrixXd A(planes.size(), 3);
    Eigen::VectorXd b(planes.size());

    int row = 0;
    for(const Plane & p : planes)
    {
        A.coeffRef(row, 0) = p.a();
        A.coeffRef(row, 1) = p.b();
        A.coeffRef(row, 2) = p.c();
        b[row] = p.d;
        ++row;
    }

    // https://eigen.tuxfamily.org/dox-devel/group__LeastSquares.html
    Eigen::Vector3d res = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);

    inters[0] = res[0];
    inters[1] = res[1];
    inters[2] = res[2];

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool intersection(const Ray & r, const Segment & s, vec3d & inters, const double tol)
{
    if (((r.dir()).cross(s.dir())).length() == 0) return false;

    std::vector<Plane> r_planes = r.to_planes();
    std::vector<Plane> s_planes = s.to_planes();

    std::vector<Plane> planes;
    std::copy(r_planes.begin(), r_planes.end(), std::back_inserter(planes));
    std::copy(s_planes.begin(), s_planes.end(), std::back_inserter(planes));

    least_squares_intersection(planes, inters);

    if (s.dist(inters) < tol && r.dist(inters) < tol) return true;
    return false;
}

}
