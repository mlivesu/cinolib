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
int point_in_segment_exact(const vec2d & p,
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
int point_in_segment_exact(const vec3d & p,
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
int point_in_triangle_exact(const vec2d & p,
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
int point_in_triangle_exact(const vec3d & p,
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
int point_in_tet_exact(const vec3d & p,
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

// returns true if s0 and s1 intersect at any point (borders included).
// Coincident segments, or segments that meet at a shared sub-simplex
// forming a valid simplicial complex are not considered intersecting
CINO_INLINE
bool segment_segment_intersect_exact(const vec2d  s0[],
                                     const vec2d  s1[])
{
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
        if(s0[0]==s1[0] || s0[0]==s1[1] || s0[1]==s1[0] || s0[1]==s1[1]) return false;

        // at least one segment endpoint is involved in the intersection        
        //if(s00_wrt_s1==0 || s01_wrt_s1==0 || s10_wrt_s0==0 || s11_wrt_s0==0) return !strict;
        return true;
    }

    // degenerate case: colinear segments
    if(s00_wrt_s1==0 && s01_wrt_s1==0 && s10_wrt_s0==0 && s11_wrt_s0==0)
    {
        // coincident segments
        if((s0[0]==s1[0] && s0[1]==s1[1]) || (s0[0]==s1[1] && s0[1]==s1[0])) return false;

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
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if s0 and s1 intersect at any point (borders included).
// Coincident segments, or segments that meet at a shared sub-simplex
// forming a valid simplicial complex are not considered intersecting
CINO_INLINE
bool segment_segment_intersect_exact(const vec3d  s0[],
                                     const vec3d  s1[])
{
    // project on X,Y,Z and, if the check is never false in any of the
    // projections, then it must be true

    vec2d s0_x[2] = { vec2d(s0[0],DROP_X), vec2d(s0[1],DROP_X) };
    vec2d s1_x[2] = { vec2d(s1[0],DROP_X), vec2d(s1[1],DROP_X) };
    if(!segment_segment_intersect_exact(s0_x, s1_x)) return false;

    vec2d s0_y[2] = { vec2d(s0[0],DROP_Y), vec2d(s0[1],DROP_Y) };
    vec2d s1_y[2] = { vec2d(s1[0],DROP_Y), vec2d(s1[1],DROP_Y) };
    if(!segment_segment_intersect_exact(s0_y, s1_y)) return false;

    vec2d s0_z[2] = { vec2d(s0[0],DROP_Z), vec2d(s0[1],DROP_Z) };
    vec2d s1_z[2] = { vec2d(s1[0],DROP_Z), vec2d(s1[1],DROP_Z) };
    if(!segment_segment_intersect_exact(s0_z, s1_z)) return false;

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if s and t intersect at any point (borders included).
// If s is a sub-simplex of t, or s and t meet at shared sub-simplex
// forming a valid simplicial complex, they are not considered intersecting
CINO_INLINE
bool segment_triangle_intersect_exact(const vec2d s[],
                                      const vec2d t[])
{
    // check if s is a subsimplex of t
    if((s[0]==t[0] && s[1]==t[1]) || (s[1]==t[0] && s[0]==t[1]) ||
       (s[0]==t[1] && s[1]==t[2]) || (s[1]==t[1] && s[0]==t[2]) ||
       (s[0]==t[2] && s[1]==t[0]) || (s[1]==t[2] && s[0]==t[0]))
    {
        return false;
    }

    vec2d t01[2] = {t[0],t[1]};
    if(segment_segment_intersect_exact(s,t01)) return true;

    vec2d t12[2] = {t[1],t[2]};
    if(segment_segment_intersect_exact(s,t12)) return true;

    vec2d t20[2] = {t[2],t[0]};
    if(segment_segment_intersect_exact(s,t20)) return true;

    bool s0_wrt_t = point_in_triangle_exact(s[0],t);
    bool s1_wrt_t = point_in_triangle_exact(s[1],t);

    return (s0_wrt_t>=STRICTLY_INSIDE &&
            s1_wrt_t>=STRICTLY_INSIDE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if s and t intersect at any point (borders included).
// If s is a sub-simplex of t, or s and t meet at shared sub-simplex
// forming a valid simplicial complex, they are not considered intersecting
CINO_INLINE
bool segment_triangle_intersect_exact(const vec3d s[],
                                      const vec3d t[])
{
    // check if s is a subsimplex of t
    if((s[0]==t[0] && s[1]==t[1]) || (s[1]==t[0] && s[0]==t[1]) ||
       (s[0]==t[1] && s[1]==t[2]) || (s[1]==t[1] && s[0]==t[2]) ||
       (s[0]==t[2] && s[1]==t[0]) || (s[1]==t[2] && s[0]==t[0]))
    {
        return false;
    }

    double s0_wrt_t = orient3d(s[0],t[0],t[1],t[2]);
    double s1_wrt_t = orient3d(s[1],t[0],t[1],t[2]);

    if(s0_wrt_t==0 && s1_wrt_t==0) // coplanar
    {
        vec2d s_x[2] = { vec2d(s[0],DROP_X), vec2d(s[1],DROP_X) };
        vec2d t_x[3] = { vec2d(t[0],DROP_X), vec2d(t[1],DROP_X), vec2d(t[2],DROP_X) };
        if(!segment_triangle_intersect_exact(s_x, t_x)) return false;

        vec2d s_y[2] = { vec2d(s[0],DROP_Y), vec2d(s[1],DROP_Y) };
        vec2d t_y[3] = { vec2d(t[0],DROP_Y), vec2d(t[1],DROP_Y), vec2d(t[2],DROP_Y) };
        if(!segment_triangle_intersect_exact(s_y, t_y)) return false;

        vec2d s_z[2] = { vec2d(s[0],DROP_Z), vec2d(s[1],DROP_Z) };
        vec2d t_z[3] = { vec2d(t[0],DROP_Z), vec2d(t[1],DROP_Z), vec2d(t[2],DROP_Z) };
        if(!segment_triangle_intersect_exact(s_z, t_z)) return false;
    }

    // s strictly above/below t
    if((s0_wrt_t>0 && s1_wrt_t>0) || (s0_wrt_t<0 && s1_wrt_t<0)) return false;

    // s intersects t (border included), if the signs of the three tetrahedra
    // obtained combining s with the three edges of t are all equal

    double vol_s_t01 = orient3d(s[0], s[1], t[0], t[1]);
    double vol_s_t12 = orient3d(s[0], s[1], t[1], t[2]);
    double vol_s_t20 = orient3d(s[0], s[1], t[2], t[0]);

    if((vol_s_t01>0 && vol_s_t12<0) || (vol_s_t01<0 && vol_s_t12>0)) return false;
    if((vol_s_t12>0 && vol_s_t20<0) || (vol_s_t12<0 && vol_s_t20>0)) return false;
    if((vol_s_t20>0 && vol_s_t01<0) || (vol_s_t20<0 && vol_s_t01>0)) return false;

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t0 and t1 intersect at any point (borders included).
// Coincident triangles, or triangles that meet at meet at shared sub-simplex
// forming a valid simplicial complex are not considered intersecting
CINO_INLINE
bool triangle_triangle_intersect_exact(const vec2d t0[],
                                       const vec2d t1[])
{
    // check if t0 and t1 are coincident
    if((t0[0]==t1[0] && t0[1]==t1[1] && t0[2]==t1[2]) ||
       (t0[1]==t1[1] && t0[2]==t1[2] && t0[0]==t1[0]) ||
       (t0[2]==t1[2] && t0[0]==t1[0] && t0[1]==t1[1]) ||
       (t0[2]==t1[2] && t0[1]==t1[1] && t0[0]==t1[0]) ||
       (t0[0]==t1[0] && t0[2]==t1[2] && t0[1]==t1[1]) ||
       (t0[1]==t1[1] && t0[0]==t1[0] && t0[2]==t1[2]))
    {
        return false;
    }

    // test for point inside tris or along edges
    int t00_wrt_t1 = point_in_triangle_exact(t0[0],t1);
    if (t00_wrt_t1 == STRICTLY_INSIDE || t00_wrt_t1 >= ON_EDGE0) return true;
    int t01_wrt_t1 = point_in_triangle_exact(t0[1],t1);
    if (t01_wrt_t1 == STRICTLY_INSIDE || t01_wrt_t1 >= ON_EDGE0) return true;
    int t02_wrt_t1 = point_in_triangle_exact(t0[2],t1);
    if (t02_wrt_t1 == STRICTLY_INSIDE || t02_wrt_t1 >= ON_EDGE0) return true;
    int t10_wrt_t0 = point_in_triangle_exact(t1[0],t0);
    if (t10_wrt_t0 == STRICTLY_INSIDE || t10_wrt_t0 >= ON_EDGE0) return true;
    int t11_wrt_t0 = point_in_triangle_exact(t1[1],t0);
    if (t11_wrt_t0 == STRICTLY_INSIDE || t11_wrt_t0 >= ON_EDGE0) return true;
    int t12_wrt_t0 = point_in_triangle_exact(t1[2],t0);
    if (t12_wrt_t0 == STRICTLY_INSIDE || t12_wrt_t0 >= ON_EDGE0) return true;

    vec2d s00[2] = { t0[0], t0[1] };
    vec2d s01[2] = { t0[1], t0[2] };
    vec2d s02[2] = { t0[2], t0[0] };
    vec2d s10[2] = { t1[0], t1[1] };
    vec2d s11[2] = { t1[1], t1[2] };
    vec2d s12[2] = { t1[2], t1[0] };

    // test for segment intersections
    if(segment_segment_intersect_exact(s00,s10) ||
       segment_segment_intersect_exact(s00,s11) ||
       segment_segment_intersect_exact(s00,s12) ||
       segment_segment_intersect_exact(s01,s10) ||
       segment_segment_intersect_exact(s01,s11) ||
       segment_segment_intersect_exact(s01,s12) ||
       segment_segment_intersect_exact(s02,s10) ||
       segment_segment_intersect_exact(s02,s11) ||
       segment_segment_intersect_exact(s02,s12))
    {
        return true;
    }

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t0 and t1 intersect at any point (borders included).
// Coincident triangles, or triangles that meet at meet at shared sub-simplex
// forming a valid simplicial complex are not considered intersecting
CINO_INLINE
bool triangle_triangle_intersect_exact(const vec3d t0[],
                                       const vec3d t1[])
{
    // check if t0 and t1 are coincident
    if((t0[0]==t1[0] && t0[1]==t1[1] && t0[2]==t1[2]) ||
       (t0[1]==t1[1] && t0[2]==t1[2] && t0[0]==t1[0]) ||
       (t0[2]==t1[2] && t0[0]==t1[0] && t0[1]==t1[1]) ||
       (t0[2]==t1[2] && t0[1]==t1[1] && t0[0]==t1[0]) ||
       (t0[0]==t1[0] && t0[2]==t1[2] && t0[1]==t1[1]) ||
       (t0[1]==t1[1] && t0[0]==t1[0] && t0[2]==t1[2]))
    {
        return false;
    }

    // test for point inside tris or along edges
    int t00_wrt_t1 = point_in_triangle_exact(t0[0],t1);
    if (t00_wrt_t1 == STRICTLY_INSIDE || t00_wrt_t1 >= ON_EDGE0) return true;
    int t01_wrt_t1 = point_in_triangle_exact(t0[1],t1);
    if (t01_wrt_t1 == STRICTLY_INSIDE || t01_wrt_t1 >= ON_EDGE0) return true;
    int t02_wrt_t1 = point_in_triangle_exact(t0[2],t1);
    if (t02_wrt_t1 == STRICTLY_INSIDE || t02_wrt_t1 >= ON_EDGE0) return true;
    int t10_wrt_t0 = point_in_triangle_exact(t1[0],t0);
    if (t10_wrt_t0 == STRICTLY_INSIDE || t10_wrt_t0 >= ON_EDGE0) return true;
    int t11_wrt_t0 = point_in_triangle_exact(t1[1],t0);
    if (t11_wrt_t0 == STRICTLY_INSIDE || t11_wrt_t0 >= ON_EDGE0) return true;
    int t12_wrt_t0 = point_in_triangle_exact(t1[2],t0);
    if (t12_wrt_t0 == STRICTLY_INSIDE || t12_wrt_t0 >= ON_EDGE0) return true;

    vec3d s00[2] = { t0[0], t0[1] };
    vec3d s01[2] = { t0[1], t0[2] };
    vec3d s02[2] = { t0[2], t0[0] };
    vec3d s10[2] = { t1[0], t1[1] };
    vec3d s11[2] = { t1[1], t1[2] };
    vec3d s12[2] = { t1[2], t1[0] };

    // test for segment intersections
    if(segment_segment_intersect_exact(s00,s10) ||
       segment_segment_intersect_exact(s00,s11) ||
       segment_segment_intersect_exact(s00,s12) ||
       segment_segment_intersect_exact(s01,s10) ||
       segment_segment_intersect_exact(s01,s11) ||
       segment_segment_intersect_exact(s01,s12) ||
       segment_segment_intersect_exact(s02,s10) ||
       segment_segment_intersect_exact(s02,s11) ||
       segment_segment_intersect_exact(s02,s12))
    {
        return true;
    }

    return false;
}

}
