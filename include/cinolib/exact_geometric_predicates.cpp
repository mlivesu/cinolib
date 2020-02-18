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

CINO_INLINE
bool points_are_colinear_exact(const vec2d & p0,
                               const vec2d & p1,
                               const vec2d & p2)
{
    return orient2d(p0,p1,p2) == 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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

CINO_INLINE
bool points_are_coplanar_exact(const vec3d & p0,
                               const vec3d & p1,
                               const vec3d & p2,
                               const vec3d & p3)
{
    return orient3d(p0,p1,p2,p3) == 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_segment_exact(const vec2d & p,
                            const vec2d   s[],
                            const bool    strict,
                                  int   & where)
{
    where = STRICTLY_OUTSIDE;

    if(!strict)
    {
        if(p==s[0]) { where = ON_VERT_0; return true; }
        if(p==s[1]) { where = ON_VERT_1; return true; }
    }

    if(!points_are_colinear_exact(s[0],s[1],p)) return false;

    if(p.x() > std::min(s[0].x(),s[1].x()) && p.x() < std::max(s[0].x(),s[1].x())) { where = STRICTLY_INSIDE; return true; }
    if(p.y() > std::min(s[0].y(),s[1].y()) && p.y() < std::max(s[0].y(),s[1].y())) { where = STRICTLY_INSIDE; return true; }

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_segment_exact(const vec3d & p,
                            const vec3d   s[],
                            const bool    strict,
                                  int   & where)
{
    where = STRICTLY_OUTSIDE;

    if(!strict)
    {
        if(p==s[0]) { where = ON_VERT_0; return true; }
        if(p==s[1]) { where = ON_VERT_1; return true; }
    }

    if(!points_are_colinear_exact(s[0],s[1],p)) return false;

    if(p.x() > std::min(s[0].x(),s[1].x()) && p.x() < std::max(s[0].x(),s[1].x())) { where = STRICTLY_INSIDE; return true; }
    if(p.y() > std::min(s[0].y(),s[1].y()) && p.y() < std::max(s[0].y(),s[1].y())) { where = STRICTLY_INSIDE; return true; }
    if(p.z() > std::min(s[0].z(),s[1].z()) && p.z() < std::max(s[0].z(),s[1].z())) { where = STRICTLY_INSIDE; return true; }

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_triangle_exact(const vec2d & p,
                             const vec2d   t[],
                             const bool    strict,
                                   int   & where)
{
    where = STRICTLY_OUTSIDE;

    if(!strict)
    {
        if(p==t[0]) { where = ON_VERT_0; return true; }
        if(p==t[1]) { where = ON_VERT_1; return true; }
        if(p==t[2]) { where = ON_VERT_2; return true; }
    }

    double e0p_area = orient2d(t[0],t[1],p);
    double e1p_area = orient2d(t[1],t[2],p);
    double e2p_area = orient2d(t[2],t[0],p);

    bool hit = (e0p_area>=0 && e1p_area>=0 && e2p_area>=0) ||
               (e0p_area<=0 && e1p_area<=0 && e2p_area<=0);

    if(hit)
    {
        if(e0p_area==0) { if(strict) return false; else { where = ON_EDGE_0; return true; }}
        if(e1p_area==0) { if(strict) return false; else { where = ON_EDGE_1; return true; }}
        if(e2p_area==0) { if(strict) return false; else { where = ON_EDGE_2; return true; }}

        where = STRICTLY_INSIDE;
        return true;
    }

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_triangle_exact(const vec3d & p,
                             const vec3d   t[],
                                   int   & where)
{
    where = STRICTLY_OUTSIDE;

    if(p==t[0]) { where = ON_VERT_0; return true; }
    if(p==t[1]) { where = ON_VERT_1; return true; }
    if(p==t[2]) { where = ON_VERT_2; return true; }

    int w;
    vec3d e0[2] = {t[0],t[1]};
    vec3d e1[2] = {t[1],t[2]};
    vec3d e2[2] = {t[2],t[0]};
    if(point_in_segment_exact(p, e0, true, w)) { where = ON_EDGE_0; return true; }
    if(point_in_segment_exact(p, e1, true, w)) { where = ON_EDGE_1; return true; }
    if(point_in_segment_exact(p, e2, true, w)) { where = ON_EDGE_2; return true; }

    // for the interior: project t on XYZ and, if the check is never false in any of the
    // projections, then p must be inside it

    vec2d p_x(p,DROP_X);
    vec2d t_x[3] =
    {
        vec2d(t[0],DROP_X),
        vec2d(t[1],DROP_X),
        vec2d(t[2],DROP_X)
    };
    if(!point_in_triangle_exact(p_x, t_x, true, w)) return false;

    vec2d p_y(p,DROP_Y);
    vec2d t_y[3] =
    {
        vec2d(t[0],DROP_Y),
        vec2d(t[1],DROP_Y),
        vec2d(t[2],DROP_Y)
    };
    if(!point_in_triangle_exact(p_y, t_y, true, w)) return false;

    vec2d p_z(p,DROP_Z);
    vec2d t_z[3] =
    {
        vec2d(t[0],DROP_Z),
        vec2d(t[1],DROP_Z),
        vec2d(t[2],DROP_Z)
    };
    if(!point_in_triangle_exact(p_z, t_z, true, w)) return false;

    where = STRICTLY_INSIDE;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_tet_exact(const vec3d & p,
                        const vec3d   t[],
                              int   & where)
{
    where = STRICTLY_OUTSIDE;

    if(p==t[0]) { where = ON_VERT_0; return true; }
    if(p==t[1]) { where = ON_VERT_1; return true; }
    if(p==t[2]) { where = ON_VERT_2; return true; }
    if(p==t[3]) { where = ON_VERT_3; return true; }

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
        if(on_f0 && on_f2) { where = ON_EDGE_0; return true; }
        if(on_f0 && on_f3) { where = ON_EDGE_1; return true; }
        if(on_f0 && on_f1) { where = ON_EDGE_2; return true; }
        if(on_f1 && on_f3) { where = ON_EDGE_3; return true; }
        if(on_f1 && on_f2) { where = ON_EDGE_4; return true; }
        if(on_f2 && on_f3) { where = ON_EDGE_5; return true; }

        if(on_f0) { where = ON_FACE_0; return true; }
        if(on_f1) { where = ON_FACE_0; return true; }
        if(on_f2) { where = ON_FACE_1; return true; }
        if(on_f3) { where = ON_FACE_2; return true; }

        where = STRICTLY_INSIDE;
        return true;
    }

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//bool segment_segment_intersect_exact(const vec2d   s0[],
//                                     const vec2d   s1[],
//                                           int   & where_on_s0,
//                                           int   & where_on_s1)
//{
///*
//    // https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
//    double det_u_v0 = orient2d(u0,u1,v0);
//    double det_u_v1 = orient2d(u0,u1,v1);
//    double det_v_u0 = orient2d(v0,v1,u0);
//    double det_v_u1 = orient2d(v0,v1,u1);

//    // Shewchuck's orient predicates return a rough approximation of the determinant.
//    // I am converting values to { -1, 0, 1 } for a simpler check of intersection cases
//    int v0_wrt_u = (det_u_v0>0) ? 1 : ((det_u_v0<0) ? -1 : 0);
//    int v1_wrt_u = (det_u_v1>0) ? 1 : ((det_u_v1<0) ? -1 : 0);
//    int u0_wrt_v = (det_v_u0>0) ? 1 : ((det_v_u0<0) ? -1 : 0);
//    int u1_wrt_v = (det_v_u1>0) ? 1 : ((det_v_u1<0) ? -1 : 0);

//    if(v0_wrt_u!=v1_wrt_u && u0_wrt_v!=u1_wrt_v) // segments intersect
//    {
//        if(allow_shared_endpoints && (u0==v0 || u0==v1 || u1==v0 || u1==v1)) return false;
//        return true;
//    }

//    if(v0_wrt_u==0 && v1_wrt_u==0 && u0_wrt_v==0 && u1_wrt_v==0) // colinear
//    {
//        if(allow_shared_endpoints)
//        {
//            if(u0==v0 && u1==v1) return false;
//            if(u0==v1 && u1==v0) return false;
//        }
//        if(u0.x() > std::min(v0.x(),v1.x()) && u0.x() < std::max(v0.x(),v1.x())) return true;
//        if(u0.y() > std::min(v0.y(),v1.y()) && u0.y() < std::max(v0.y(),v1.y())) return true;
//        if(u1.x() > std::min(v0.x(),v1.x()) && u1.x() < std::max(v0.x(),v1.x())) return true;
//        if(u1.y() > std::min(v0.y(),v1.y()) && u1.y() < std::max(v0.y(),v1.y())) return true;
//    }

//    return false;
//*/
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//bool segment_segment_intersect_exact(const vec3d   s0[],
//                                     const vec3d   s1[],
//                                           int   & where_on_s0,
//                                           int   & where_on_s1)
//{
///*
//    // project on X,Y,Z and, if the check is never false in any of the
//    // projections, then it must be true

//    vec2d u0_x(u0,DROP_X);
//    vec2d u1_x(u1,DROP_X);
//    vec2d v0_x(v0,DROP_X);
//    vec2d v1_x(v1,DROP_X);
//    if(!segments_intersect_exact(u0_x, u1_x, v0_x, v1_x, allow_shared_endpoints)) return false;

//    vec2d u0_y(u0,DROP_Y);
//    vec2d u1_y(u1,DROP_Y);
//    vec2d v0_y(v0,DROP_Y);
//    vec2d v1_y(v1,DROP_Y);
//    if(!segments_intersect_exact(u0_y, u1_y, v0_y, v1_y, allow_shared_endpoints)) return false;

//    vec2d u0_z(u0,DROP_Z);
//    vec2d u1_z(u1,DROP_Z);
//    vec2d v0_z(v0,DROP_Z);
//    vec2d v1_z(v1,DROP_Z);
//    if(!segments_intersect_exact(u0_z, u1_z, v0_z, v1_z, allow_shared_endpoints)) return false;

//    return true;
//*/
//}

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//bool segment_triangle_intersect_exact(const vec2d   s[],
//                                      const vec2d   t[],
//                                            int   & where_on_s,
//                                            int   & where_on_t);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//bool segment_triangle_intersect_exact(const vec3d   s[],
//                                      const vec3d   t[],
//                                            int   & where_on_s,
//                                            int   & where_on_t);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//bool triangle_triangle_intersect_exact(const vec2d   t0[],
//                                       const vec2d   t1[],
//                                             int   & where_on_t0,
//                                             int   & where_on_t1);

////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//bool triangle_triangle_intersect_exact(const vec3d   t0[],
//                                       const vec3d   t1[],
//                                             int   & where_on_t0,
//                                             int   & where_on_t1);

}
