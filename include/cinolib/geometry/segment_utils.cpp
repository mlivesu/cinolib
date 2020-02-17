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
#include <cinolib/geometry/segment_utils.h>
#include <cinolib/geometry/point_utils.h>

namespace cinolib
{

// true if point p lies (strictly) inside segment v0-v1
CINO_INLINE
bool segment_contains_point_exact(const vec2d & v0,
                                  const vec2d & v1,
                                  const vec2d & p,
                                  const bool    strict,
                                  const bool    skip_colinearity_test)
{
    if(!skip_colinearity_test && !points_are_colinear_exact(v0, v1, p)) return false;

    if(!strict && p==v0) return true;
    if(!strict && p==v1) return true;

    if(p.x() > std::min(v0.x(),v1.x()) && p.x() < std::max(v0.x(),v1.x())) return true;
    if(p.y() > std::min(v0.y(),v1.y()) && p.y() < std::max(v0.y(),v1.y())) return true;

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if point p lies (strictly) inside segment v0-v1
CINO_INLINE
bool segment_contains_point_exact(const vec3d & v0,
                                  const vec3d & v1,
                                  const vec3d & p,
                                  const bool    strict,
                                  const bool    skip_colinearity_test)
{
    if(!skip_colinearity_test && !points_are_colinear_exact(v0, v1, p)) return false;

    if(!strict && p==v0) return true;
    if(!strict && p==v1) return true;

    if(p.x() > std::min(v0.x(),v1.x()) && p.x() < std::max(v0.x(),v1.x())) return true;
    if(p.y() > std::min(v0.y(),v1.y()) && p.y() < std::max(v0.y(),v1.y())) return true;
    if(p.z() > std::min(v0.z(),v1.z()) && p.z() < std::max(v0.z(),v1.z())) return true;

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if point segments u and v intersect. Shared endpoints are not
// counted as intersections if allow_shared_endpoints is true
CINO_INLINE
bool segments_intersect_exact(const vec2d & u0,
                              const vec2d & u1,
                              const vec2d & v0,
                              const vec2d & v1,
                              const bool    allow_shared_endpoints)
{
    // https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    double det_u_v0 = orient2d(u0,u1,v0);
    double det_u_v1 = orient2d(u0,u1,v1);
    double det_v_u0 = orient2d(v0,v1,u0);
    double det_v_u1 = orient2d(v0,v1,u1);

    // Shewchuck's orient predicates return a rough approximation of the determinant.
    // I am converting values to { -1, 0, 1 } for a simpler check of intersection cases
    int v0_wrt_u = (det_u_v0>0) ? 1 : ((det_u_v0<0) ? -1 : 0);
    int v1_wrt_u = (det_u_v1>0) ? 1 : ((det_u_v1<0) ? -1 : 0);
    int u0_wrt_v = (det_v_u0>0) ? 1 : ((det_v_u0<0) ? -1 : 0);
    int u1_wrt_v = (det_v_u1>0) ? 1 : ((det_v_u1<0) ? -1 : 0);

    if(v0_wrt_u!=v1_wrt_u && u0_wrt_v!=u1_wrt_v) // segments intersect
    {
        if(allow_shared_endpoints && (u0==v0 || u0==v1 || u1==v0 || u1==v1)) return false;
        return true;
    }

    if(v0_wrt_u==0 && v1_wrt_u==0 && u0_wrt_v==0 && u1_wrt_v==0) // colinear
    {
        if(allow_shared_endpoints)
        {
            if(u0==v0 && u1==v1) return false;
            if(u0==v1 && u1==v0) return false;
        }
        if(u0.x() > std::min(v0.x(),v1.x()) && u0.x() < std::max(v0.x(),v1.x())) return true;
        if(u0.y() > std::min(v0.y(),v1.y()) && u0.y() < std::max(v0.y(),v1.y())) return true;
        if(u1.x() > std::min(v0.x(),v1.x()) && u1.x() < std::max(v0.x(),v1.x())) return true;
        if(u1.y() > std::min(v0.y(),v1.y()) && u1.y() < std::max(v0.y(),v1.y())) return true;
    }

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if point segments u and v intersect. Shared endpoints are not
// counted as intersections if allow_shared_endpoints is true
CINO_INLINE
bool segments_intersect_exact(const vec3d & u0,
                              const vec3d & u1,
                              const vec3d & v0,
                              const vec3d & v1,
                              const bool    allow_shared_endpoints)
{
    // project on X,Y,Z and, if the check is never false in any of the
    // projections, then it must be true

    vec2d u0_x(u0,DROP_X);
    vec2d u1_x(u1,DROP_X);
    vec2d v0_x(v0,DROP_X);
    vec2d v1_x(v1,DROP_X);
    if(!segments_intersect_exact(u0_x, u1_x, v0_x, v1_x, allow_shared_endpoints)) return false;

    vec2d u0_y(u0,DROP_Y);
    vec2d u1_y(u1,DROP_Y);
    vec2d v0_y(v0,DROP_Y);
    vec2d v1_y(v1,DROP_Y);
    if(!segments_intersect_exact(u0_y, u1_y, v0_y, v1_y, allow_shared_endpoints)) return false;

    vec2d u0_z(u0,DROP_Z);
    vec2d u1_z(u1,DROP_Z);
    vec2d v0_z(v0,DROP_Z);
    vec2d v1_z(v1,DROP_Z);
    if(!segments_intersect_exact(u0_z, u1_z, v0_z, v1_z, allow_shared_endpoints)) return false;

    return true;
}

}
