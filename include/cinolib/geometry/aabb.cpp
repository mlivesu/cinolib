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
#include <cinolib/geometry/aabb.h>
#include <algorithm>

namespace cinolib
{

CINO_INLINE std::ostream & operator<<(std::ostream & in, const AABB & bb)
{
    in << "\n[AABB] MIN: " << bb.min << "\tMAX: " << bb.max;
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
AABB::AABB(const vec3d min, const vec3d max) : min(min), max(max)
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
AABB::AABB(const std::vector<vec3d> & p_list, const double scaling_factor)
{    
    update(p_list, scaling_factor);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// AABB that contains all AABBs in b_list
CINO_INLINE
AABB::AABB(const std::vector<AABB> & b_list, const double scaling_factor)
{
    std::vector<vec3d> p_list;
    p_list.reserve(b_list.size()*2);
    for(const AABB & b : b_list)
    {
        p_list.push_back(b.min);
        p_list.push_back(b.max);
    }
    update(p_list, scaling_factor);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void AABB::reset()
{
    min = vec3d( inf_double,  inf_double,  inf_double);
    max = vec3d(-inf_double, -inf_double, -inf_double);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void AABB::update(const std::vector<vec3d> & p_list, const double scaling_factor)
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
void AABB::scale(const double s)
{
    vec3d c = center();
    min -= c;   max -= c;
    min *= s;   max *= s;
    min += c;   max += c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d AABB::center() const
{
    return (min + max) * 0.5;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double AABB::diag() const
{
    if(min.x()==inf_double) return 0.0;
    return (min - max).length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double AABB::delta_x() const
{
    return (max.x() - min.x());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double AABB::delta_y() const
{
    return (max.y() - min.y());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double AABB::delta_z() const
{
    return (max.z() - min.z());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d AABB::delta() const
{
    return (max - min);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double AABB::min_entry() const
{
    return std::min(min.min_entry(), max.min_entry());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double AABB::max_entry() const
{
    return std::max(min.max_entry(), max.max_entry());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d AABB::point_closest_to(const vec3d & p) const
{
    // Real Time Collision Detection", Section 5.1.3
    vec3d res = p;
    res = res.max(min);
    res = res.min(max);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double AABB::dist_sqrd(const vec3d & p) const
{
    return p.dist_squared(point_closest_to(p));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double AABB::dist(const vec3d & p) const
{
    return p.dist(point_closest_to(p));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool AABB::contains(const vec3d & p, const bool strict) const
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
bool AABB::intersects_box(const AABB & box, const bool strict) const
{    
    if(strict)
    {
        if(max.x() <= box.min.x() || min.x() >= box.max.x()) return false;
        if(max.y() <= box.min.y() || min.y() >= box.max.y()) return false;
        if(max.z() <= box.min.z() || min.z() >= box.max.z()) return false;
    }
    else
    {
        if(max.x() < box.min.x() || min.x() > box.max.x()) return false;
        if(max.y() < box.min.y() || min.y() > box.max.y()) return false;
        if(max.z() < box.min.z() || min.z() > box.max.z()) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* Intersection between the AABB and the ray R = P + t * dir
 * If any intersection occurs, the function returns true, and
 * pos and t_min refer to the entry point of the ray in the box
 *
 * Ref: Real Time Collision Detection, Section 5.3.3
 *
 * IMPORTANT: there was a bug, see also the Errata Corrige at
 * http://realtimecollisiondetection.net/books/rtcd/errata/
*/
CINO_INLINE
bool AABB::intersects_ray(const vec3d & p, const vec3d & dir, double & t_min, vec3d & pos) const
{
           t_min = 0.0;        // set to -FLT_MAX to get first hit on line
    double t_max = inf_double; // set to max distance ray can travel (for segment)

    for(int i=0; i<3; ++i)
    {
        if(std::fabs(dir[i]) < 1e-15)
        {
            // ray is parallel to current axis. No hit if origin not within slab
            if(p[i]<min[i] || p[i]>max[i]) return false;
        }
        else
        {
            // Compute intersection t value of ray with near and far plane of slab
            double ood = 1.0/dir[i];
            double t_near = (min[i] - p[i]) * ood;
            double t_far  = (max[i] - p[i]) * ood;
            if(t_near > t_far) std::swap(t_near, t_far);

            // Incrementally intersects slabs across X,Y and Z coordinates
            // THIS PART IS WROnG IN THE BOOK!!!
            // (see http://realtimecollisiondetection.net/books/rtcd/errata/)
            t_min = std::max(t_min, t_near);
            t_max = std::min(t_max, t_far);

            // Exit with no collision as soon as slab intersection becomes empty
            if(t_min>t_max) return false;
        }
    }
    // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
    pos = p + dir*t_min;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> AABB::corners(const double scaling_factor) const
{
    double dx = delta_x(); assert(dx>=0);
    double dy = delta_y(); assert(dy>=0);
    double dz = delta_z(); assert(dz>=0);

    std::vector<vec3d> c =
    {
        min,
        min + vec3d(dx,  0,  0),
        min + vec3d(dx, dy,  0),
        min + vec3d( 0, dy,  0),
        min + vec3d( 0,  0, dz),
        min + vec3d(dx,  0, dz),
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
std::vector<uint> AABB::tris() const
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
std::vector<uint> AABB::quads() const
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
std::vector<uint> AABB::edges() const
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
