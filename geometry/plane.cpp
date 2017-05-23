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
#include <cinolib/geometry/plane.h>

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Plane & plane)
{
    in << "[Plane] " << plane.n.x() << " * x + "
                     << plane.n.y() << " * y + "
                     << plane.n.z() << " * z = "
                     << plane.d << "\n";
    return in;
}


CINO_INLINE
Plane::Plane(const vec3d & p0, const vec3d & p1, const vec3d & p2)
{
    vec3d u = p1 - p0;
    vec3d v = p2 - p0;
    set_plane(p0, u.cross(v));
    assert(fabs(operator[](p0)) < 1e-10);
    assert(fabs(operator[](p1)) < 1e-10);
    assert(fabs(operator[](p2)) < 1e-10);
}


CINO_INLINE
Plane::Plane(const vec3d & point, const vec3d & normal)
{
    set_plane(point, normal);
    assert(fabs(operator[](point)) < 1e-10);
}


// http://www.ilikebigbits.com/blog/2015/3/2/plane-from-points
CINO_INLINE
Plane::Plane(const std::vector<vec3d> & samples)
{
    // centroid
    vec3d c(0,0,0);
    for(auto p : samples) c += p;
    c /= static_cast<double>(samples.size());

    // 3x3 covariance matrix
    double xx = 0.0; double yy = 0.0;
    double xy = 0.0; double yz = 0.0;
    double xz = 0.0; double zz = 0.0;
    //
    for(auto p : samples)
    {
        vec3d pc = p - c;
        xx += pc.x() * pc.x();
        xy += pc.x() * pc.y();
        xz += pc.x() * pc.z();
        yy += pc.y() * pc.y();
        yz += pc.y() * pc.z();
        zz += pc.z() * pc.z();
    }

    double det_x   = yy*zz - yz*yz;
    double det_y   = xx*zz - xz*xz;
    double det_z   = xx*yy - xy*xy;
    double det_max = std::max(det_x, std::max(det_y, det_z));

    if (det_max == 0) std::cerr << "WARNING : the points don't span a plane!" << std::endl;

    if (det_max == det_x)
    {
        vec3d n(1.0, (xz*yz - xy*zz) / det_x, (xy*yz - xz*yy) / det_x);
        set_plane(c,n);
    }
    else if (det_max == det_y)
    {
        vec3d n((yz*xz - xy*zz) / det_y, 1.0, (xy*xz - yz*xx) / det_y);
        set_plane(c,n);
    }
    else if (det_max == det_z)
    {
        vec3d n((yz*xy - xz*yy) / det_z, (xz*xy - yz*xx) / det_z, 1.0);
        set_plane(c,n);
    }
    else assert(false);
}


CINO_INLINE
void Plane::set_plane(const vec3d & point, const vec3d & normal)
{
    p = point;
    n = normal;
    n.normalize();
    d = n.dot(point);
    assert(fabs(operator[](point)) < 1e-10);
}


CINO_INLINE
double Plane::operator[](const vec3d & p) const
{
    return (n.dot(p) - d);
}



// http://mathworld.wolfram.com/HessianNormalForm.html
// http://mathworld.wolfram.com/Point-PlaneDistance.html (eq. 13)
//
CINO_INLINE
double Plane::point_plane_dist(const vec3d & p) const
{
    assert(fabs(n.length()-1.0) < 1e-10);
    vec3d u = p - this->p;
    if (u.length() == 0) return 0;
    double dist = fabs(u.dot(n));
    assert(dist >= 0);
    return dist;
}


CINO_INLINE
vec3d Plane::project_onto(const vec3d & p)
{
    vec3d res;
    if (this->operator [](p) > 0)
    {
        res = p - n * point_plane_dist(p);
        assert(point_plane_dist(res) < 1e-7);
    }
    else
    {
        res = p + n * point_plane_dist(p);
        assert(point_plane_dist(res) < 1e-7);
    }
    return res;
}

}
