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
#include "cinolib/geometry/line.h"
#include <cinolib/cino_inline.h>

namespace cinolib
{

CINO_INLINE
Line::Line(const vec3d & p0, const vec3d & p1)
{
    assert(p0.dist(p1) > 0);
    this->p0 = p0;
    this->p1 = p1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<Plane> Line::to_planes() const
{
    vec3d d = dir();

    vec3d n0(-d.y(),  d.x(),     0);
    vec3d n1(-d.z(),      0, d.x());
    vec3d n2(     0, -d.z(), d.y());

    std::vector<Plane> planes;
    if (n0.length() > 0) planes.push_back(Plane(p0, n0));
    if (n1.length() > 0) planes.push_back(Plane(p0, n1));
    if (n2.length() > 0) if (planes.size() < 2) planes.push_back(Plane(p0, n2));
    assert(planes.size() == 2);

    return planes;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Line::dir() const
{
    vec3d d = p0-p1;
    d.normalize();
    return d;
}

}
