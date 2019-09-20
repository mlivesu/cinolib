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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Plane::Plane(const vec3d & point, const vec3d & normal)
{
    set_plane(point, normal);
    assert(fabs(operator[](point)) < 1e-10);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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

    //if(fabs(det_max) <= 1e-5) std::cerr << "WARNING : the samples don't span a plane!" << std::endl;

    vec3d n;
    if (det_max == det_x) n = vec3d(1.0, (xz*yz - xy*zz) / det_x, (xy*yz - xz*yy) / det_x); else
    if (det_max == det_y) n = vec3d((yz*xz - xy*zz) / det_y, 1.0, (xy*xz - yz*xx) / det_y); else
    if (det_max == det_z) n = vec3d((yz*xy - xz*yy) / det_z, (xz*xy - yz*xx) / det_z, 1.0);
    else assert(false);

    set_plane(c,n);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Plane::set_plane(const vec3d & point, const vec3d & normal)
{
    if(point.is_degenerate() || normal.is_degenerate())
    {
        //std::cout << "WARNING : failed to set degenerate plane!" << std::endl;
        p = vec3d(0,0,0);
        n = vec3d(0,0,0);
        return;
    }
    p = point;
    n = normal;
    n.normalize();
    d = n.dot(point);
    assert(fabs(operator[](point)) < 1e-10);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Plane::operator[](const vec3d & p) const
{
    return (n.dot(p) - d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://mathworld.wolfram.com/HessianNormalForm.html
// http://mathworld.wolfram.com/Point-PlaneDistance.html (eq. 13)
//
CINO_INLINE
double Plane::point_plane_dist_signed(const vec3d & p) const
{
    assert(fabs(n.length()-1.0) < 1e-10);
    vec3d u = p - this->p;
    return u.dot(n);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Plane::point_plane_dist(const vec3d & p) const
{
    return std::fabs(point_plane_dist_signed(p));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Plane::project_onto(const vec3d & p) const
{
    vec3d res = p - n * point_plane_dist_signed(p);
    assert(point_plane_dist(res) < 1e-10);
    return res;
}

}
