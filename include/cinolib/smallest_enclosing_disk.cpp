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
#include <cinolib/smallest_enclosing_disk.h>

#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::pair<vec2d,double> make_disk(const std::vector<vec2d> & points,
                                  const uint                 id)
{
    // (degenerate) disk passing through 1 point
    std::pair<vec2d,double> disk;
    disk.first  = points.at(id);
    disk.second = 0.0;
    return disk;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::pair<vec2d,double> make_disk(const std::vector<vec2d> & points,
                                    const uint               id0,
                                    const uint               id1)
{
    // disk passing through 2 points
    std::pair<vec2d,double> disk;
    disk.first = (points.at(id0) + points.at(id1))*0.5;
    disk.second = disk.first.dist(points.at(id1));
    return disk;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::pair<vec2d,double> make_disk(const std::vector<vec2d> & points,
                                    const uint               id0,
                                    const uint               id1,
                                    const uint               id2)
{
    // disk passing through 3 points
    std::pair<vec2d,double> disk;
    double xd, yd, side_a, side_b, side_c, s;
    double term1, term2, term3, denom;
    xd = points.at(id0).x()-points.at(id1).x();
    yd = points.at(id0).y()-points.at(id1).y();
    side_a = sqrt(xd*xd + yd*yd);
    xd = points.at(id1).x()-points.at(id2).x();
    yd = points.at(id1).y()-points.at(id2).y();
    side_b = sqrt(xd*xd + yd*yd);
    xd = points.at(id2).x()-points.at(id0).x();
    yd = points.at(id2).y()-points.at(id0).y();
    side_c = sqrt(xd*xd + yd*yd);
    s = (side_a + side_b + side_c)*0.5;
    denom = sqrt(s*(s - side_a)*(s - side_b)*(s - side_c));
    disk.second = side_a * side_b * side_c * 0.25 / denom;
    term1 = points.at(id0).x()*points.at(id0).x() + points.at(id0).y()*points.at(id0).y();
    term2 = points.at(id1).x()*points.at(id1).x() + points.at(id1).y()*points.at(id1).y();
    term3 = points.at(id2).x()*points.at(id2).x() + points.at(id2).y()*points.at(id2).y();
    denom = 2.0*(points.at(id0).x()*(points.at(id1).y()-points.at(id2).y()) +
        points.at(id1).x()*(points.at(id2).y()-points.at(id0).y()) + points.at(id2).x()*(points.at(id0).y()-points.at(id1).y()));
    disk.first.x() = (term1*(points.at(id1).y()-points.at(id2).y()) + term2*(points.at(id2).y()-points.at(id0).y()) +
        term3*(points.at(id0).y()-points.at(id1).y()))/denom;
    disk.first.y() = (term1*(points.at(id2).x()-points.at(id1).x()) + term2*(points.at(id0).x()-points.at(id2).x()) +
        term3*(points.at(id1).x()-points.at(id0).x()))/denom;
    return disk;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Smallest Enclosing Disks (Balls and Ellipsoids)
// Emo Welzl
// New Results and Trends in Computer Science
// Lecture Notes in Computer Science, 555 (1991), pp359-370
//
CINO_INLINE
std::pair<vec2d,double> Welzl_algorithm(const std::vector<vec2d> & points,
                                        const std::vector<uint>  & P,
                                        const uint                 nP,
                                              std::vector<uint>  & R,
                                        const uint                 nR)
{
    if(nP == 1)
    {
        if(nR == 0) return make_disk(points, P.at(0));
        if(nR == 1) return make_disk(points, P.at(0), R.at(0));
    }
    else if(nP == 0)
    {
        if(nR == 1) return make_disk(points, R.at(0));
        if(nR == 2) return make_disk(points, R.at(0), R.at(1));
    }
    if (nR == 3) return make_disk(points, R.at(0), R.at(1), R.at(2));

    std::pair<vec2d,double> disk = Welzl_algorithm(points, P, nP-1, R, nR);
    if (disk.first.dist(points.at(P.at(nP-1))) < disk.second) return disk;
    R.at(nR) = P.at(nP-1);
    return Welzl_algorithm(points, P, nP-1, R, nR+1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void smallest_enclosing_disk(const std::vector<vec2d> & points,
                                   vec2d              & center,
                                   double             & radius)
{
    std::vector<uint> P(points.size());
    std::vector<uint> R(points.size(),0);
    std::iota(P.begin(), P.end(), 0);

    // randomize points (http://www.cplusplus.com/reference/algorithm/shuffle/)
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(P.begin(), P.end(), std::default_random_engine(seed));

    auto min_disk = Welzl_algorithm(points, P, P.size(), R, 0);
    center = min_disk.first;
    radius = min_disk.second;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void smallest_enclosing_disk(const std::vector<vec3d> & points,  // will drop z component
                                   vec3d              & center,  // will have z=0
                                   double             & radius)
{
    std::vector<vec2d> points_2d;
    for(auto p : points) points_2d.push_back(vec2d(p.x(), p.y()));

    vec2d center_2d;
    smallest_enclosing_disk(points_2d, center_2d, radius);
    center = vec3d(center_2d.x(), center_2d.y(), 0);
}

}
