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
#include <cinolib/exact_geometric_predicates.h>
#include <cinolib/Shewchuk_predicates.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the area of the triangle p0-p1-p2 is zero
CINO_INLINE
bool points_are_colinear_exact(const vec2d & p0,
                               const vec2d & p1,
                               const vec2d & p2)
{
    return orient2d(p0,p1,p2) == 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the area of all the orthogonal 2d projections of the triangle p0-p1-p2 is zero
CINO_INLINE
bool points_are_colinear_exact(const vec3d & p0,
                               const vec3d & p1,
                               const vec3d & p2)
{
    // check if all the 2d orthogonal projections of p are colinear
    if(points_are_colinear_exact(vec2d(p0,DROP_X), vec2d(p1,DROP_X), vec2d(p2,DROP_X)) &&
       points_are_colinear_exact(vec2d(p0,DROP_Y), vec2d(p1,DROP_Y), vec2d(p2,DROP_Y)) &&
       points_are_colinear_exact(vec2d(p0,DROP_Z), vec2d(p1,DROP_Z), vec2d(p2,DROP_Z)))
    {
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the volume of the tetrahedron p0-p1-p2-p3 is zero
CINO_INLINE
bool points_are_coplanar_exact(const vec3d & p0,
                               const vec3d & p1,
                               const vec3d & p2,
                               const vec3d & p3)
{
    return orient3d(p0,p1,p2,p3) == 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of s
// STRICTLY_INSIDE  if p lies inside segment s (endpoints excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_segment_exact(const vec2d & p,
                           const vec2d   s[])
{
    if(p==s[0]) return ON_VERT0;
    if(p==s[1]) return ON_VERT1;

    if(!points_are_colinear_exact(s[0],s[1],p)) return STRICTLY_OUTSIDE;

    if((p.x()>std::min(s[0].x(),s[1].x()) && p.x()<std::max(s[0].x(),s[1].x())) ||
       (p.y()>std::min(s[0].y(),s[1].y()) && p.y()<std::max(s[0].y(),s[1].y())))
    {        
        return STRICTLY_INSIDE;
    }

    return STRICTLY_OUTSIDE;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of s
// STRICTLY_INSIDE  if p lies inside segment s (endpoints excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_segment_exact(const vec3d & p,
                                      const vec3d   s[])
{
    if(p==s[0]) return ON_VERT0;
    if(p==s[1]) return ON_VERT1;

    if(!points_are_colinear_exact(s[0],s[1],p)) return STRICTLY_OUTSIDE;

    if((p.x()>std::min(s[0].x(),s[1].x()) && p.x()<std::max(s[0].x(),s[1].x())) ||
       (p.y()>std::min(s[0].y(),s[1].y()) && p.y()<std::max(s[0].y(),s[1].y())) ||
       (p.z()>std::min(s[0].z(),s[1].z()) && p.z()<std::max(s[0].z(),s[1].z())))
    {
        return STRICTLY_INSIDE;
    }

    return STRICTLY_OUTSIDE;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// STRICTLY_INSIDE  if p lies inside triangle t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_triangle_exact(const vec2d & p,
                                       const vec2d   t[])
{
    if(p==t[0]) return ON_VERT0;
    if(p==t[1]) return ON_VERT1;
    if(p==t[2]) return ON_VERT2;

    double e0p_area = orient2d(t[0],t[1],p);
    double e1p_area = orient2d(t[1],t[2],p);
    double e2p_area = orient2d(t[2],t[0],p);

    bool hit = (e0p_area>=0 && e1p_area>=0 && e2p_area>=0) ||
               (e0p_area<=0 && e1p_area<=0 && e2p_area<=0);

    if(hit)
    {
        if(e0p_area==0) return ON_EDGE0;
        if(e1p_area==0) return ON_EDGE1;
        if(e2p_area==0) return ON_EDGE2;

        return STRICTLY_INSIDE;
    }

    return STRICTLY_OUTSIDE;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// STRICTLY_INSIDE  if p lies inside triangle t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_triangle_exact(const vec3d & p,
                                       const vec3d   t[])
{
    // test for point in vert
    if(p==t[0]) return ON_VERT0;
    if(p==t[1]) return ON_VERT1;
    if(p==t[2]) return ON_VERT2;

    // test for point in edge in 3D
    vec3d e0[2] = {t[0],t[1]};
    vec3d e1[2] = {t[1],t[2]};
    vec3d e2[2] = {t[2],t[0]};
    if(point_in_segment_exact(p,e0)==ON_EDGE0) return ON_EDGE0;
    if(point_in_segment_exact(p,e1)==ON_EDGE1) return ON_EDGE1;
    if(point_in_segment_exact(p,e2)==ON_EDGE2) return ON_EDGE2;

    // test for the interior: project t on XYZ and, if the check is never false in
    // any of the projections, then p must be inside t

    vec2d p_x(p,DROP_X);
    vec2d t_x[3] =
    {
        vec2d(t[0],DROP_X),
        vec2d(t[1],DROP_X),
        vec2d(t[2],DROP_X)
    };
    if(point_in_triangle_exact(p_x,t_x)==STRICTLY_OUTSIDE) return STRICTLY_OUTSIDE;

    vec2d p_y(p,DROP_Y);
    vec2d t_y[3] =
    {
        vec2d(t[0],DROP_Y),
        vec2d(t[1],DROP_Y),
        vec2d(t[2],DROP_Y)
    };
    if(point_in_triangle_exact(p_y,t_y)==STRICTLY_OUTSIDE) return STRICTLY_OUTSIDE;

    vec2d p_z(p,DROP_Z);
    vec2d t_z[3] =
    {
        vec2d(t[0],DROP_Z),
        vec2d(t[1],DROP_Z),
        vec2d(t[2],DROP_Z)
    };
    if(point_in_triangle_exact(p_z,t_z)==STRICTLY_OUTSIDE) return STRICTLY_OUTSIDE;

    return STRICTLY_INSIDE;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// ON_FACEk         if p lies inside the k-th face of t (borders excluded)
// STRICTLY_INSIDE  if p lies inside tetrahedron t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_tet_exact(const vec3d & p,
                                  const vec3d   t[])
{
    // test for point in vert
    if(p==t[0]) return ON_VERT0;
    if(p==t[1]) return ON_VERT1;
    if(p==t[2]) return ON_VERT2;
    if(p==t[3]) return ON_VERT2;

    // according to refrence tet as in cinolib/standard_elements_tables.h
    double f0p_vol = orient3d(t[0],t[2],t[1],p);
    double f1p_vol = orient3d(t[0],t[1],t[3],p);
    double f2p_vol = orient3d(t[0],t[3],t[2],p);
    double f3p_vol = orient3d(t[1],t[2],t[3],p);

    bool hit = (f0p_vol>=0 && f1p_vol>=0 && f2p_vol>=0 && f3p_vol>=0) ||
               (f0p_vol<=0 && f1p_vol<=0 && f2p_vol<=0 && f3p_vol<=0);

    if(hit)
    {
        bool on_f0 = (f0p_vol==0);
        bool on_f1 = (f1p_vol==0);
        bool on_f2 = (f2p_vol==0);
        bool on_f3 = (f3p_vol==0);

        // according to refrence tet as in cinolib/standard_elements_tables.h
        if(on_f0 && on_f2) return ON_EDGE0;
        if(on_f0 && on_f3) return ON_EDGE1;
        if(on_f0 && on_f1) return ON_EDGE2;
        if(on_f1 && on_f3) return ON_EDGE3;
        if(on_f1 && on_f2) return ON_EDGE4;
        if(on_f2 && on_f3) return ON_EDGE5;

        if(on_f0) return ON_FACE0;
        if(on_f1) return ON_FACE1;
        if(on_f2) return ON_FACE2;
        if(on_f3) return ON_FACE3;

        return STRICTLY_INSIDE;
    }

    return STRICTLY_OUTSIDE;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if segments are fully disjoint
// SIMPLICIAL_COMPLEX   if segments coincide or intersect at a shared vertex
// INTERSECT            if segments intersect at an inner point (for s0, s1, or both)
// OVERLAP              if segments are colinear and partially overlapped
CINO_INLINE
SimplexIntersection segment_segment_intersect_exact(const vec2d s0[],
                                                    const vec2d s1[])
{
    assert(!segment_is_degenerate_exact(s0) &&
           !segment_is_degenerate_exact(s1));

    // https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    double det_s00 = orient2d(s1[0],s1[1],s0[0]);
    double det_s01 = orient2d(s1[0],s1[1],s0[1]);
    double det_s10 = orient2d(s0[0],s0[1],s1[0]);
    double det_s11 = orient2d(s0[0],s0[1],s1[1]);

    // Shewchuk's orient predicates return a rough approximation of the determinant.
    // I am converting values to { -1, 0, 1 } for a simpler check of intersection cases
    int s00_wrt_s1 = (det_s00>0) ? 1 : ((det_s00<0) ? -1 : 0);
    int s01_wrt_s1 = (det_s01>0) ? 1 : ((det_s01<0) ? -1 : 0);
    int s10_wrt_s0 = (det_s10>0) ? 1 : ((det_s10<0) ? -1 : 0);
    int s11_wrt_s0 = (det_s11>0) ? 1 : ((det_s11<0) ? -1 : 0);

    // segments intersect at a single point
    if(s00_wrt_s1!=s01_wrt_s1 && s10_wrt_s0!=s11_wrt_s0)
    {
        // edges share an endpoint
        if(s0[0]==s1[0] || s0[0]==s1[1] || s0[1]==s1[0] || s0[1]==s1[1]) return SIMPLICIAL_COMPLEX;

        // at least one segment endpoint is involved in the intersection
        //if(s00_wrt_s1==0 || s01_wrt_s1==0 || s10_wrt_s0==0 || s11_wrt_s0==0) return !strict;
        return INTERSECT;
    }

    // degenerate case: colinear segments
    if(s00_wrt_s1==0 && s01_wrt_s1==0 && s10_wrt_s0==0 && s11_wrt_s0==0)
    {
        // coincident segments
        if((s0[0]==s1[0] && s0[1]==s1[1]) || (s0[0]==s1[1] && s0[1]==s1[0])) return SIMPLICIAL_COMPLEX;

        double Xmin_s1 = std::min(s1[0].x(),s1[1].x());
        double Xmax_s1 = std::max(s1[0].x(),s1[1].x());
        double Ymin_s1 = std::min(s1[0].y(),s1[1].y());
        double Ymax_s1 = std::max(s1[0].y(),s1[1].y());
        double Xmin_s0 = std::min(s0[0].x(),s0[1].x());
        double Xmax_s0 = std::max(s0[0].x(),s0[1].x());
        double Ymin_s0 = std::min(s0[0].y(),s0[1].y());
        double Ymax_s0 = std::max(s0[0].y(),s0[1].y());

        if(// test s0 endpoints against s1 range
           (s0[0].x() > Xmin_s1 && s0[0].x() < Xmax_s1) ||
           (s0[0].y() > Ymin_s1 && s0[0].y() < Ymax_s1) ||
           (s0[1].x() > Xmin_s1 && s0[1].x() < Xmax_s1) ||
           (s0[1].y() > Ymin_s1 && s0[1].y() < Ymax_s1) ||
           // test s1 endpoints against s0 range
           (s1[0].x() > Xmin_s0 && s1[0].x() < Xmax_s0) ||
           (s1[0].y() > Ymin_s0 && s1[0].y() < Ymax_s0) ||
           (s1[1].x() > Xmin_s0 && s1[1].x() < Xmax_s0) ||
           (s1[1].y() > Ymin_s0 && s1[1].y() < Ymax_s0))
        {
            return INTERSECT;
        }
    }
    return DO_NOT_INTERSECT;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if segments are fully disjoint
// SIMPLICIAL_COMPLEX   if segments coincide or intersect at a shared vertex
// INTERSECT            if segments intersect at an inner point (for s0, s1, or both)
// OVERLAP              if segments are colinear and partially overlapped
CINO_INLINE
SimplexIntersection segment_segment_intersect_exact(const vec3d s0[],
                                                    const vec3d s1[])
{
    assert(!segment_is_degenerate_exact(s0) &&
           !segment_is_degenerate_exact(s1));

    // check for coincident segments
    bool s00_is_shared = (s0[0]==s1[0] || s0[0]==s1[1]);
    bool s01_is_shared = (s0[1]==s1[0] || s0[1]==s1[1]);
    bool s10_is_shared = (s1[0]==s0[0] || s1[0]==s0[1]);
    bool s11_is_shared = (s1[1]==s0[0] || s1[1]==s0[1]);

    // s0 and s1 are coincident or one edge is degenerate and coincides with one vertex of the other
    if(s00_is_shared && s01_is_shared && s10_is_shared && s11_is_shared) return SIMPLICIAL_COMPLEX;

    // check 2D projections of the segments

    vec2d s0_x[2] = { vec2d(s0[0],DROP_X), vec2d(s0[1],DROP_X) };
    vec2d s1_x[2] = { vec2d(s1[0],DROP_X), vec2d(s1[1],DROP_X) };
    int x_res = segment_segment_intersect_exact(s0_x, s1_x);
    if (x_res == DO_NOT_INTERSECT) return DO_NOT_INTERSECT;

    vec2d s0_y[2] = { vec2d(s0[0],DROP_Y), vec2d(s0[1],DROP_Y) };
    vec2d s1_y[2] = { vec2d(s1[0],DROP_Y), vec2d(s1[1],DROP_Y) };
    int y_res = segment_segment_intersect_exact(s0_y, s1_y);
    if (y_res == DO_NOT_INTERSECT) return DO_NOT_INTERSECT;

    vec2d s0_z[2] = { vec2d(s0[0],DROP_Z), vec2d(s0[1],DROP_Z) };
    vec2d s1_z[2] = { vec2d(s1[0],DROP_Z), vec2d(s1[1],DROP_Z) };
    int z_res = segment_segment_intersect_exact(s0_z, s1_z);
    if (z_res == DO_NOT_INTERSECT) return DO_NOT_INTERSECT;

    // segments are deemed overlapping if they are so in at least two projections our of three
    // (overlapping axis aligned segments will look like a valid simplcial complex in one projection)
    if((x_res==OVERLAP && y_res==OVERLAP) ||
       (x_res==OVERLAP && z_res==OVERLAP) ||
       (y_res==OVERLAP && z_res==OVERLAP))
    {
        return OVERLAP;
    }

    return INTERSECT;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if s and t are fully disjoint
// SIMPLICIAL_COMPLEX   if s is an edge of t, or s is degenerate and coincides with a vertex of t
// INTERSECT            if s and t intersect and do not forma a valid simplex
CINO_INLINE
SimplexIntersection segment_triangle_intersect_exact(const vec2d s[],
                                                     const vec2d t[])
{    
    assert(!segment_is_degenerate_exact(s) &&
           !triangle_is_degenerate_exact(t));

    if(point_in_triangle_exact(s[0],t)>=STRICTLY_INSIDE ||
       point_in_triangle_exact(s[1],t)>=STRICTLY_INSIDE)
    {
        return INTERSECT;
    }

    bool simpl_complex = false;

    vec2d t01[] = { t[0], t[1] };
    switch(segment_segment_intersect_exact(s,t01))
    {
        case SIMPLICIAL_COMPLEX : simpl_complex = true; break;
        case INTERSECT          : return INTERSECT;
        case OVERLAP            : return INTERSECT;
        case DO_NOT_INTERSECT   : break;
    }

    vec2d t12[] = { t[1], t[2] };
    switch(segment_segment_intersect_exact(s,t12))
    {
        case SIMPLICIAL_COMPLEX : simpl_complex = true; break;
        case INTERSECT          : return INTERSECT;
        case OVERLAP            : return INTERSECT;
        case DO_NOT_INTERSECT   : break;
    }

    vec2d t20[] = { t[2], t[0] };
    switch(segment_segment_intersect_exact(s,t20))
    {
        case SIMPLICIAL_COMPLEX : simpl_complex = true; break;
        case INTERSECT          : return INTERSECT;
        case OVERLAP            : return INTERSECT;
        case DO_NOT_INTERSECT   : break;
    }

    if(simpl_complex) return SIMPLICIAL_COMPLEX;
    return DO_NOT_INTERSECT;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if s and t are fully disjoint
// SIMPLICIAL_COMPLEX   if s is an edge of t, or s is degenerate and coincides with a vertex of t
// INTERSECT            if s and t intersect and do not forma a valid simplex
CINO_INLINE
SimplexIntersection segment_triangle_intersect_exact(const vec3d s[],
                                                     const vec3d t[])
{
    assert(!segment_is_degenerate_exact(s) &&
           !triangle_is_degenerate_exact(t));

    auto vol_s0_t = orient3d(s[0], t[0], t[1], t[2]);
    auto vol_s1_t = orient3d(s[1], t[0], t[1], t[2]);

    if(vol_s0_t >0 && vol_s1_t >0) return DO_NOT_INTERSECT; // s is above t
    if(vol_s0_t <0 && vol_s1_t <0) return DO_NOT_INTERSECT; // s is below t
    if(vol_s0_t==0 && vol_s1_t==0)                          // s and t are coplanar
    {
        // same code as the 2D version, I just copied it here....

        if(point_in_triangle_exact(s[0],t) || point_in_triangle_exact(s[1],t)) return INTERSECT;

        uint simpl_complex = 0;

        vec3d t01[] = { t[0], t[1] };
        switch(segment_segment_intersect_exact(s,t01))
        {
            case SIMPLICIAL_COMPLEX : ++simpl_complex; break;
            case INTERSECT          : return INTERSECT;
            case OVERLAP            : return INTERSECT;
            case DO_NOT_INTERSECT   : break;
        }

        vec3d t12[] = { t[1], t[2] };
        switch(segment_segment_intersect_exact(s,t12))
        {
            case SIMPLICIAL_COMPLEX : ++simpl_complex; break;
            case INTERSECT          : return INTERSECT;
            case OVERLAP            : return INTERSECT;
            case DO_NOT_INTERSECT   : break;
        }

        vec3d t20[] = { t[2], t[0] };
        switch(segment_segment_intersect_exact(s,t20))
        {
            case SIMPLICIAL_COMPLEX : ++simpl_complex; break;
            case INTERSECT          : return INTERSECT;
            case OVERLAP            : return INTERSECT;
            case DO_NOT_INTERSECT   : break;
        }

        // if it is a simplicial complex from any view, then it really is...
        if(simpl_complex==3) return SIMPLICIAL_COMPLEX;
        return DO_NOT_INTERSECT;
    }

    // s intersects t (borders included), if the signs of the three tetrahedra
    // obtained combining s with the three edges of t are all equal

    // if one point is coplanar and coincides with a triangle vertex, then they form a valid complex
    if(s[0]==t[0] || s[0]==t[1] || s[0]==t[2] ||
       s[1]==t[0] || s[1]==t[1] || s[1]==t[2])
    {
        return SIMPLICIAL_COMPLEX;
    }

    double vol_s_t01 = orient3d(s[0], s[1], t[0], t[1]);
    double vol_s_t12 = orient3d(s[0], s[1], t[1], t[2]);
    double vol_s_t20 = orient3d(s[0], s[1], t[2], t[0]);

    assert(vol_s_t01!=0);
    assert(vol_s_t12!=0);
    assert(vol_s_t20!=0);

    if((vol_s_t01>0 && vol_s_t12<0) || (vol_s_t01<0 && vol_s_t12>0)) return DO_NOT_INTERSECT;
    if((vol_s_t12>0 && vol_s_t20<0) || (vol_s_t12<0 && vol_s_t20>0)) return DO_NOT_INTERSECT;
    if((vol_s_t20>0 && vol_s_t01<0) || (vol_s_t20<0 && vol_s_t01>0)) return DO_NOT_INTERSECT;

    return INTERSECT;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if triangles are fully disjoint
// SIMPLICIAL_COMPLEX   if triangles coincide or intersect at a shared sub-simplex
// INTERSECT            if triangles intersect without making a valid simplcial complex
CINO_INLINE
SimplexIntersection triangle_triangle_intersect_exact(const vec2d t0[],
                                                      const vec2d t1[])
{
    assert(!triangle_is_degenerate_exact(t0) &&
           !triangle_is_degenerate_exact(t1));

    // binary flags to mark coincident vertices in t0 and t1
    std::bitset<3> t0_shared = { 0b000 };
    std::bitset<3> t1_shared = { 0b000 };

    // find vert correspondences
    if(t0[0]==t1[0]) { t0_shared[0] = true; t1_shared[0] = true; }
    if(t0[0]==t1[1]) { t0_shared[0] = true; t1_shared[1] = true; }
    if(t0[0]==t1[2]) { t0_shared[0] = true; t1_shared[2] = true; }
    if(t0[1]==t1[0]) { t0_shared[1] = true; t1_shared[0] = true; }
    if(t0[1]==t1[1]) { t0_shared[1] = true; t1_shared[1] = true; }
    if(t0[1]==t1[2]) { t0_shared[1] = true; t1_shared[2] = true; }
    if(t0[2]==t1[0]) { t0_shared[2] = true; t1_shared[0] = true; }
    if(t0[2]==t1[1]) { t0_shared[2] = true; t1_shared[1] = true; }
    if(t0[2]==t1[2]) { t0_shared[2] = true; t1_shared[2] = true; }

    // count number of coincident vertices in t0 and t1
    uint t0_count = t0_shared.count();
    uint t1_count = t1_shared.count();

    // either t0 and t1 are coincident or one of the two triangles
    // is degenerate and is an edge/vertex of the other
    if(t0_count==3 || t1_count==3) return SIMPLICIAL_COMPLEX;

    // t0 and t1 share an edge. Let e be the shared edge and { opp0, opp1 } be the two vertices opposite to
    // e in t0 and t1, respectively. If opp0 and opp1 lie at the same side of e, the two triangles overlap.
    // Otherwise they are edge-adjacent and form a valid simplicial complex
    if(t0_count==2 && t1_count==2)
    {
        uint e[2];      // indices of the shared vertices (in t0)
        uint count = 0; // index for e (to fill it)
        uint opp0  = 0; // index of the vertex opposite to e in t0
        uint opp1  = 0; // index of the vertex opposite to e in t1
        for(uint i=0; i<3; ++i)
        {
            if(!t0_shared[i]) opp0 = i; else e[count++] = i;
            if(!t1_shared[i]) opp1 = i;
        }

        double opp0_wrt_e = orient2d(t0[e[0]], t0[e[1]], t0[opp0]);
        double opp1_wrt_e = orient2d(t0[e[0]], t0[e[1]], t1[opp1]);

        if((opp0_wrt_e>0 && opp1_wrt_e<0) || (opp0_wrt_e<0 && opp1_wrt_e>0)) return SIMPLICIAL_COMPLEX;
        return INTERSECT;
    }

    // t0 and t1 share a vertex. Let v be the shared vertex and { opp0 , opp1 } be the two edges opposite to
    // v in t0 and t1, respectively. If v-opp0 intersects t1, or v-opp1 interects t0, the two triangles overlap.
    // Otherwise they are verte-adjacent and form a valid simplicial complex
    if(t0_count==1 && t1_count==1)
    {
        uint v0; // index of the shared vertex in t0
        uint v1; // index of the shared vertex in t1
        for(uint i=0; i<3; ++i)
        {
            if(t0_shared[i]) v0 = i;
            if(t1_shared[i]) v1 = i;
        }

        // check for intersection with t0 and t1 edges emanating from v1 and v0, respectively
        vec2d e_v0_0[] = { t0[v0], t0[(v0+1)%3] };
        vec2d e_v0_1[] = { t0[v0], t0[(v0+2)%3] };
        vec2d e_v1_0[] = { t1[v1], t1[(v1+1)%3] };
        vec2d e_v1_1[] = { t1[v1], t1[(v1+2)%3] };
        if(segment_triangle_intersect_exact(e_v0_0, t1)==INTERSECT ||
           segment_triangle_intersect_exact(e_v0_1, t1)==INTERSECT ||
           segment_triangle_intersect_exact(e_v1_0, t0)==INTERSECT ||
           segment_triangle_intersect_exact(e_v1_1, t0)==INTERSECT)
        {
            return INTERSECT;
        }
        return SIMPLICIAL_COMPLEX;
    }

    // t0 and t1 do not share sub-simplices. They can be fully disjoint, intersecting at a single point, or overlapping

    vec2d t00[2] = { t0[0], t0[1] };
    vec2d t01[2] = { t0[1], t0[2] };
    vec2d t02[2] = { t0[2], t0[0] };
    vec2d t10[2] = { t1[0], t1[1] };
    vec2d t11[2] = { t1[1], t1[2] };
    vec2d t12[2] = { t1[2], t1[0] };

    if(segment_segment_intersect_exact(t00,t10)>=INTERSECT ||
       segment_segment_intersect_exact(t00,t11)>=INTERSECT ||
       segment_segment_intersect_exact(t00,t12)>=INTERSECT ||
       segment_segment_intersect_exact(t01,t10)>=INTERSECT ||
       segment_segment_intersect_exact(t01,t11)>=INTERSECT ||
       segment_segment_intersect_exact(t01,t12)>=INTERSECT ||
       segment_segment_intersect_exact(t02,t10)>=INTERSECT ||
       segment_segment_intersect_exact(t02,t11)>=INTERSECT ||
       segment_segment_intersect_exact(t02,t12)>=INTERSECT )
    {
        return INTERSECT;
    }

    if(point_in_triangle_exact(t0[0],t1)>=STRICTLY_INSIDE ||
       point_in_triangle_exact(t0[1],t1)>=STRICTLY_INSIDE ||
       point_in_triangle_exact(t0[2],t1)>=STRICTLY_INSIDE ||
       point_in_triangle_exact(t1[0],t0)>=STRICTLY_INSIDE ||
       point_in_triangle_exact(t1[1],t0)>=STRICTLY_INSIDE ||
       point_in_triangle_exact(t1[2],t0)>=STRICTLY_INSIDE )
    {
        return INTERSECT;
    }

    return DO_NOT_INTERSECT;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if triangles are fully disjoint
// SIMPLICIAL_COMPLEX   if triangles coincide or intersect at a shared sub-simplex
// INTERSECT            if triangles intersect without making a valid simplcial complex
CINO_INLINE
SimplexIntersection triangle_triangle_intersect_exact(const vec3d t0[],
                                                      const vec3d t1[])
{
    assert(!triangle_is_degenerate_exact(t0) &&
           !triangle_is_degenerate_exact(t1));

    // binary flags to mark coincident vertices in t0 and t1
    std::bitset<3> t0_shared = { 0b000 };
    std::bitset<3> t1_shared = { 0b000 };

    // find vert correspondences
    if(t0[0]==t1[0]) { t0_shared[0] = true; t1_shared[0] = true; }
    if(t0[0]==t1[1]) { t0_shared[0] = true; t1_shared[1] = true; }
    if(t0[0]==t1[2]) { t0_shared[0] = true; t1_shared[2] = true; }
    if(t0[1]==t1[0]) { t0_shared[1] = true; t1_shared[0] = true; }
    if(t0[1]==t1[1]) { t0_shared[1] = true; t1_shared[1] = true; }
    if(t0[1]==t1[2]) { t0_shared[1] = true; t1_shared[2] = true; }
    if(t0[2]==t1[0]) { t0_shared[2] = true; t1_shared[0] = true; }
    if(t0[2]==t1[1]) { t0_shared[2] = true; t1_shared[1] = true; }
    if(t0[2]==t1[2]) { t0_shared[2] = true; t1_shared[2] = true; }

    // count number of coincident vertices in t0 and t1
    uint t0_count = t0_shared.count();
    uint t1_count = t1_shared.count();

    // either t0 and t1 are coincident
    if(t0_count==3) { assert(t1_count==3); return SIMPLICIAL_COMPLEX; }

    // t0 and t1 share an edge. Let e be the shared edge and { opp0, opp1 } be the two vertices opposite to
    // e in t0 and t1, respectively. If opp0 and opp1 lie at the same side of e, the two triangles overlap.
    // Otherwise they are edge-adjacent and form a valid simplicial complex
    if(t0_count==2)
    {
        assert(t1_count==2);

        uint e[2];      // indices of the shared vertices (in t0)
        uint count = 0; // index for e (to fill it)
        uint opp0  = 0; // index of the vertex opposite to e in t0
        uint opp1  = 0; // index of the vertex opposite to e in t1
        for(uint i=0; i<3; ++i)
        {
            if(!t0_shared[i]) opp0 = i; else e[count++] = i;
            if(!t1_shared[i]) opp1 = i;
        }

        vec2d e0_x   = vec2d(t0[e[0]],DROP_X);
        vec2d e1_x   = vec2d(t0[e[1]],DROP_X);
        vec2d opp0_x = vec2d(t0[opp0],DROP_X);
        vec2d opp1_x = vec2d(t1[opp1],DROP_X);
        double opp0_wrt_e = orient2d(e0_x, e1_x, opp0_x);
        double opp1_wrt_e = orient2d(e0_x, e1_x, opp1_x);
        if((opp0_wrt_e>0 && opp1_wrt_e<0) || (opp0_wrt_e<0 && opp1_wrt_e>0)) return SIMPLICIAL_COMPLEX;

        vec2d e0_y   = vec2d(t0[e[0]],DROP_Y);
        vec2d e1_y   = vec2d(t0[e[1]],DROP_Y);
        vec2d opp0_y = vec2d(t0[opp0],DROP_Y);
        vec2d opp1_y = vec2d(t1[opp1],DROP_Y);
        opp0_wrt_e = orient2d(e0_y, e1_y, opp0_y);
        opp1_wrt_e = orient2d(e0_y, e1_y, opp1_y);
        if((opp0_wrt_e>0 && opp1_wrt_e<0) || (opp0_wrt_e<0 && opp1_wrt_e>0)) return SIMPLICIAL_COMPLEX;

        vec2d e0_z   = vec2d(t0[e[0]],DROP_Z);
        vec2d e1_z   = vec2d(t0[e[1]],DROP_Z);
        vec2d opp0_z = vec2d(t0[opp0],DROP_Z);
        vec2d opp1_z = vec2d(t1[opp1],DROP_Z);
        opp0_wrt_e = orient2d(e0_z, e1_z, opp0_z);
        opp1_wrt_e = orient2d(e0_z, e1_z, opp1_z);
        if((opp0_wrt_e>0 && opp1_wrt_e<0) || (opp0_wrt_e<0 && opp1_wrt_e>0)) return SIMPLICIAL_COMPLEX;

        return INTERSECT;
    }

    // t0 and t1 share a vertex. Let v be the shared vertex and { opp0 , opp1 } be the two edges opposite to
    // v in t0 and t1, respectively. If opp0 intersects t1, or opp1 interects t0, the two triangles overlap.
    // Otherwise they are vertex-adjacent and form a valid simplicial complex
    if(t0_count==1)
    {
        assert(t1_count==1);

        uint v0; // index of the shared vertex in t0
        uint v1; // index of the shared vertex in t1
        for(uint i=0; i<3; ++i)
        {
            if(t0_shared[i]) v0 = i;
            if(t1_shared[i]) v1 = i;
        }
        vec3d opp0[] = { t0[(v0+1)%3], t0[(v0+2)%3] };
        vec3d opp1[] = { t1[(v1+1)%3], t1[(v1+2)%3] };
        if(segment_triangle_intersect_exact(opp0,t1)>=INTERSECT ||
           segment_triangle_intersect_exact(opp1,t0)>=INTERSECT)
        {
            return INTERSECT;
        }
        return SIMPLICIAL_COMPLEX;
    }

    // t0 and t1 do not share sub-simplices. They can be fully disjoint, intersecting at a single point, or overlapping

    vec3d t00[2] = { t0[0], t0[1] };
    vec3d t01[2] = { t0[1], t0[2] };
    vec3d t02[2] = { t0[2], t0[0] };
    vec3d t10[2] = { t1[0], t1[1] };
    vec3d t11[2] = { t1[1], t1[2] };
    vec3d t12[2] = { t1[2], t1[0] };

    if(segment_triangle_intersect_exact(t00,t1)>=INTERSECT ||
       segment_triangle_intersect_exact(t01,t1)>=INTERSECT ||
       segment_triangle_intersect_exact(t02,t1)>=INTERSECT ||
       segment_triangle_intersect_exact(t10,t0)>=INTERSECT ||
       segment_triangle_intersect_exact(t11,t0)>=INTERSECT ||
       segment_triangle_intersect_exact(t12,t0)>=INTERSECT)
    {
        return INTERSECT;
    }

    return DO_NOT_INTERSECT;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if s[0]==s[1]
template<typename vec>
CINO_INLINE
bool segment_is_degenerate_exact(const vec s[])
{
    return (s[0]==s[1]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t[0], t[1] and t[2] are colinear
template<typename vec>
CINO_INLINE
bool triangle_is_degenerate_exact(const vec t[])
{
    return points_are_colinear_exact(t[0], t[1], t[2]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t[0], t[1], t[2] and t32] are coplanar
CINO_INLINE
bool tet_is_degenerate_exact(const vec3d t[])
{
    return points_are_coplanar_exact(t[0], t[1], t[2], t[3]);
}

}
