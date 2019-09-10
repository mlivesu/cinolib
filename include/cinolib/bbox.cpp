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
#include <cinolib/bbox.h>

namespace cinolib
{

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb)
{
    in << "\n[Bbox Info] MIN: " << bb.min << "\tMAX: " << bb.max <<
          "\nDiag: " << bb.diag() << "\tDeltaX: " << bb.delta_x() <<
          "\tDeltaY: " << bb.delta_y() << "\tDeltaZ: " << bb.delta_z() << "\n";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Bbox::Bbox(const std::vector<vec3d> & p_list)
{
    update(p_list);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Bbox::reset()
{
    min = vec3d( inf_double,  inf_double,  inf_double);
    max = vec3d(-inf_double, -inf_double, -inf_double);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Bbox::update(const std::vector<vec3d> & p_list)
{
    for(const vec3d & p : p_list)
    {
        min = min.min(p);
        max = max.max(p);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Bbox::center() const
{
    return (min + max) * 0.5;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::diag() const
{
    if(min.x()==inf_double) return 0.0;
    return (min - max).length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::delta_x() const
{
    return (max.x() - min.x());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::delta_y() const
{
    return (max.y() - min.y());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::delta_z() const
{
    return (max.z() - min.z());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Bbox::delta() const
{
    return (max - min);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::min_entry() const
{
    return std::min(min.min_entry(), max.min_entry());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::max_entry() const
{
    return  std::max(min.max_entry(), max.max_entry());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Bbox::contains(const vec3d & p, const bool strictly_contains) const
{
    if(strictly_contains)
    {
        if(p.x()<min.x() || p.x()>max.x()) return false;
        if(p.y()<min.y() || p.y()>max.y()) return false;
        if(p.z()<min.z() || p.z()>max.z()) return false;
    }
    else
    {
        if(p.x()<=min.x() || p.x()>=max.x()) return false;
        if(p.y()<=min.y() || p.y()>=max.y()) return false;
        if(p.z()<=min.z() || p.z()>=max.z()) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Bbox::intersects(const Bbox & box) const
{
    return (contains(box.min) || contains(box.max));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> Bbox::corners(const double scaling_factor) const
{
    double dx = delta_x();
    double dy = delta_y();
    double dz = delta_z();

    std::vector<vec3d> c =
    {
        min,
        min + vec3d(dx,  0,  0),
        min + vec3d(dx,  0, dz),
        min + vec3d( 0,  0, dz),
        min + vec3d( 0, dy,  0),
        min + vec3d(dx, dy,  0),
        min + vec3d(dx, dy, dz),
        min + vec3d( 0, dy, dz),
    };

    if(scaling_factor!=1.0)
    {
        for(vec3d & p : c) p -= center();
        for(vec3d & p : c) p *= scaling_factor;
        for(vec3d & p : c) p += center();
    }

    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> Bbox::tris() const
{
    std::vector<uint> t =
    {
        0, 3, 2,
        0, 2, 1,
        1, 2, 6,
        1, 6, 5,
        4, 5, 6,
        4, 6, 7,
        3, 0, 4,
        3, 4, 7,
        0, 1, 5,
        0, 5, 4,
        2, 3, 7,
        2, 7, 6
    };
    return t;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> Bbox::quads() const
{
    std::vector<uint> q =
    {
        0, 3, 2, 1,
        1, 2, 6, 5,
        4, 5, 6, 7,
        3, 0, 4, 7,
        0, 1, 5, 4,
        2, 3, 7, 6
    };
    return q;
}
}
