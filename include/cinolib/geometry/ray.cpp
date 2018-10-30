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
#include <cinolib/geometry/ray.h>

namespace cinolib
{

CINO_INLINE
Ray::Ray(const vec3d & p, const vec3d & dir)
{
    start     = p;
    direction = dir;
    direction.normalize();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<Plane> Ray::to_planes() const
{
    vec3d n0(-direction.y(),  direction.x(),             0);
    vec3d n1(-direction.z(),              0, direction.x());
    vec3d n2(             0, -direction.z(), direction.y());

    std::vector<Plane> planes;
    if (n0.length() > 0) planes.push_back(Plane(start, n0));
    if (n1.length() > 0) planes.push_back(Plane(start, n1));
    if (n2.length() > 0) if (planes.size() < 2) planes.push_back(Plane(start, n2));
    assert(planes.size() == 2);

    return planes;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
const vec3d & Ray::dir() const
{
    return direction;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
const vec3d & Ray::begin() const
{
    return start;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Ray::on_positive_half_space(const vec3d & p) const
{
    if ((p - start).dot(direction) >= 0) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Ray::dist_to_point(const vec3d & p) const
{
    vec3d u = direction;
    vec3d w = p - start;

    float cos_wv = w.dot(u);
    float cos_uu = u.dot(u);

    if (cos_wv <= 0.0) return start.dist(p);

    float b  = cos_wv / cos_uu;
    vec3d Pb = start + u*b;
    return (p-Pb).length();
}

}
