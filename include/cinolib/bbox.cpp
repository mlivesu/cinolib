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
#include <algorithm>

namespace cinolib
{

CINO_INLINE std::ostream & operator<<(std::ostream & in, const Bbox & bb)
{
    in << "\n[AABB] MIN: " << bb.min << "\tMAX: " << bb.max;
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Bbox::Bbox(const vec3d min, const vec3d max) : min(min), max(max)
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Bbox::Bbox(const std::vector<vec3d> & p_list, const double scaling_factor)
{    
    update(p_list, scaling_factor);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// AABB that contains all AABBs in b_list
CINO_INLINE
Bbox::Bbox(const std::vector<Bbox> & b_list, const double scaling_factor)
{
    std::vector<vec3d> p_list;
    p_list.reserve(b_list.size()*2);
    for(const Bbox & b : b_list)
    {
        p_list.push_back(b.min);
        p_list.push_back(b.max);
    }
    update(p_list, scaling_factor);
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
void Bbox::update(const std::vector<vec3d> & p_list, const double scaling_factor)
{
    assert(!p_list.empty());
    min = p_list.front();
    max = p_list.front();
    for(const vec3d & p : p_list)
    {
        min = min.min(p);
        max = max.max(p);
    }
    if(scaling_factor!=1) scale(scaling_factor);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Bbox::scale(const double s)
{
    vec3d c = center();
    min -= c;   max -= c;
    min *= s;   max *= s;
    min += c;   max += c;
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
    return std::max(min.max_entry(), max.max_entry());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// squared AABB to point distance
// https://gamedev.stackexchange.com/questions/44483/how-do-i-calculate-distance-between-a-point-and-an-axis-aligned-rectangle
//
CINO_INLINE
double Bbox::dist_to_point_sqrd(const vec3d & p) const
{
    vec3d  d = p - center();
    double dx = std::max(fabs(d.x()) - delta_x()*0.5, 0.0);
    double dy = std::max(fabs(d.y()) - delta_y()*0.5, 0.0);
    double dz = std::max(fabs(d.z()) - delta_z()*0.5, 0.0);
    return dx*dx + dy*dy + dz*dz;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Bbox::dist_to_point(const vec3d & p) const
{
    return sqrt(dist_to_point_sqrd(p));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Bbox::contains(const vec3d & p, const bool strict) const
{
    if(strict)
    {
        if(p.x()>min.x() && p.x()<max.x() &&
           p.y()>min.y() && p.y()<max.y() &&
           p.z()>min.z() && p.z()<max.z()) return true;
    }
    else
    {
        if(p.x()>=min.x() && p.x()<=max.x() &&
           p.y()>=min.y() && p.y()<=max.y() &&
           p.z()>=min.z() && p.z()<=max.z()) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Bbox::contains(const Bbox & box, const bool strict) const
{    
    return (contains(box.min, strict) ||
            contains(box.max, strict) ||
            box.contains(min, strict) ||
            box.contains(max, strict));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> Bbox::corners(const double scaling_factor) const
{
    double dx = delta_x(); assert(dx>=0);
    double dy = delta_y(); assert(dy>=0);
    double dz = delta_z(); assert(dz>=0);

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
    static std::vector<uint> t =
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
    static std::vector<uint> q =
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> Bbox::edges() const
{
    static std::vector<uint> e =
    {
        0, 1,
        0, 3,
        0, 4,
        1, 2,
        1, 5,
        2, 3,
        2, 6,
        3, 7,
        4, 5,
        4, 7,
        5, 6,
        6, 7,
    };
    return e;
}

}
