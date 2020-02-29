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
#include <cinolib/geometry/segment.h>
#include <cinolib/exact_geometric_predicates.h>

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Segment & s)
{
    in << s.v[0] << "\t" << s.v[1] << "\n";
    return in;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Real Time Collision Detection", Section 5.1.2
CINO_INLINE
vec3d Segment::point_closest_to(const vec3d & p) const
{
    vec3d u = v[1] - v[0];

    // project p onto v[0]v[1], but deferring divide by dot(u,u)
    double t =(p-v[0]).dot(u);
    if(t<=0) return v[0];

    double den = u.dot(u);
    if(t>=den) return v[1];

    t = t/den;
    return v[0] + t*u;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ItemType Segment::item_type() const
{
    return SEGMENT;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
AABB Segment::aabb() const
{
    std::vector<vec3d> tmp = {v[0], v[1]};
    return AABB(tmp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Segment::intersects_ray(const vec3d & /*p*/, const vec3d & /*dir*/, double & /*t*/, vec3d & /*pos*/) const
{
    assert(false && "TODO");
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Segment::barycentric_coordinates(const vec3d & p, double bc[]) const
{
    assert(contains(p));

    vec3d  u = v[1] - v[0];
    double t = (p-v[0]).dot(u);
    bc[1] = t / u.length();
    bc[0] = 1.0 - bc[0];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Segment::contains_exact(const vec3d & p, bool strict) const
{
    int where = point_in_segment_exact(p,v);
    if(strict) return (where==STRICTLY_INSIDE);
               return (where>=STRICTLY_INSIDE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Segment::intersects_segment_exact(const vec3d s[]) const
{
    return segment_segment_intersect_exact(v,s);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Segment::intersects_triangle_exact(const vec3d t[]) const
{
    return segment_triangle_intersect_exact(v,t);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//vec3d Segment::project_onto(const vec3d &p) const
//{
//    vec3d v = second - first;
//    vec3d w = p  - first;

//    float cos_wv = w.dot(v);
//    float cos_vv = v.dot(v);

//    if (cos_wv <= 0.0)    return first;
//    if (cos_vv <= cos_wv) return second;

//    float b  = cos_wv / cos_vv;
//    vec3d Pb = first + v*b;

//    return Pb;
//}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//double Segment::dist_to_point(const vec3d & p) const
//{
//    return p.dist(project_onto(p));
//}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//bool Segment::is_in_between(const vec3d &p) const
//{
//    vec3d v = second - first;
//    vec3d w = p  - first;

//    float cos_wv = w.dot(v);
//    float cos_vv = v.dot(v);

//    if (cos_wv <= 0.0)    return false;
//    if (cos_vv <= cos_wv) return false;
//    return true;
//}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//std::vector<Plane> Segment::to_planes() const
//{
//    vec3d d = dir();

//    vec3d n0(-d.y(),  d.x(),     0);
//    vec3d n1(-d.z(),      0, d.x());
//    vec3d n2(     0, -d.z(), d.y());

//    std::vector<Plane> planes;
//    if(n0.length() > 0) planes.push_back(Plane(first, n0));
//    if(n1.length() > 0) planes.push_back(Plane(first, n1));
//    if(n2.length() > 0) if (planes.size() < 2) planes.push_back(Plane(first, n2));
//    assert(planes.size()==2);

//    return planes;
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//vec3d Segment::dir() const
//{
//    vec3d d = first-second;
//    d.normalize();
//    return d;
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//// Real Time Collision Detection", Section 5.1.2
//CINO_INLINE
//double Segment::dist_sqrd(const vec3d & p) const
//{
//    vec3d u = v[1] - v[0];
//    vec3d v =  p - v[0];
//    vec3d w =  p - v[1];

//    double e = v.dot(u);
//    if(e<=0.0f) return v.dot(v);

//    double f = u.dot(u);
//    if(e>=f) return w.dot(w);

//    return v.dot(v) - e*e/f;
//}

}
