/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
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
#include <cinolib/AFM/convexification.h>
#include <cinolib/AFM/flip_checks.h>
#include <cinolib/AFM/snap_rounding.h>
#include <cinolib/geometry/segment_utils.h>

namespace cinolib
{

CINO_INLINE
void update_vertex_pos(AFM_data & data, const uint vid, const CGAL_Q * p)
{
    vertex_unlock(data,vid,p);
    copy(p,&data.exact_coords[3*vid]);
    //snap_rounding(data,vid); // it is not safe to round it here, because there will be a flip after
                          // returning from convexify_front, hence the new triangles will not be tested
    data.m1.vert(vid) = vec3d(CGAL::to_double(p[0]),
                              CGAL::to_double(p[1]),
                              CGAL::to_double(p[2]));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void convexify_front(AFM_data & data,
                     const uint v0,
                     const uint v1,
                     const uint v2)
{
    // I assume I can move at least one point
    assert(!data.m1.vert_is_boundary(v0) || !data.m1.vert_is_boundary(v1));

    CGAL_Q v2_lift[3] = { 0, 0, 0 };
    v2_lift[0] = (data.exact_coords[3*v2+0]*999 + data.exact_coords[3*data.origin+0])/1000;
    v2_lift[1] = (data.exact_coords[3*v2+1]*999 + data.exact_coords[3*data.origin+1])/1000;
    v2_lift[2] = (data.exact_coords[3*v2+2]*999 + data.exact_coords[3*data.origin+2])/1000;
    //midpoint(&data.exact_coords[3*v2], &data.exact_coords[3*data.origin], v2_lift);

    CGAL_Q v0_new[3] = { 0, 0, 0 };
    CGAL_Q v1_new[3] = { 0, 0, 0 };
    CGAL_Q d0,d1; // distance from the origin

    if(!data.m1.vert_is_boundary(v0))
    {
        line_intersection2d(&data.exact_coords[3*v0],
                            &data.exact_coords[3*data.origin],
                            &data.exact_coords[3*v1],
                            v2_lift, v0_new);
        d0 = sqrd_distance2d(v0_new, &data.exact_coords[3*data.origin]);
    }

    if(!data.m1.vert_is_boundary(v1))
    {
        line_intersection2d(&data.exact_coords[3*v1],
                            &data.exact_coords[3*data.origin],
                            &data.exact_coords[3*v0],
                            v2_lift, v1_new);
        d1 = sqrd_distance2d(v1_new, &data.exact_coords[3*data.origin]);
    }

    if(data.m1.vert_is_boundary(v1) || (d0>d1 && !data.m1.vert_is_boundary(v0))) update_vertex_pos(data,v0,v0_new);
    else update_vertex_pos(data,v1,v1_new);

    if(data.enable_sanity_checks)
    {
        for(uint pid : data.m1.adj_v2p(v0)) assert(!flipped(data,pid));
        for(uint pid : data.m1.adj_v2p(v1)) assert(!flipped(data,pid));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_is_blocking(AFM_data & data,
                          const uint     vid,
                          const uint     pid,
                          const CGAL_Q * p)
{
    assert(data.m1.poly_contains_vert(pid,vid));

    // if the trignale is beyond the front, it's not blocking
    if(data.m1.poly_contains_vert(pid,data.origin)) return false;

    uint e_opp = data.m1.edge_opposite_to(pid,vid);
    uint v0 = data.m1.edge_vert_id(e_opp,0);
    uint v1 = data.m1.edge_vert_id(e_opp,1);
    if(data.m1.poly_verts_are_CCW(pid,v0,v1)) std::swap(v0,v1);

    // it the positive half space of the edge opposite to front_vert
    // does not contain the new_pos, the triangle is blocking
    if(orient2d(&data.exact_coords[3*v0],
                &data.exact_coords[3*v1],
                p)<=0) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint vertex_locks(AFM_data & data,
                  const uint          vid,
                  std::vector<uint> & locks,
                  const CGAL_Q      * p)
{
    assert(data.m1.verts_are_adjacent(vid,data.origin));

    locks.clear();
    for(uint pid : data.m1.adj_v2p(vid))
    {
        if(triangle_is_blocking(data,vid,pid,p))
        {
            locks.push_back(pid);
        }
    }
    return locks.size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void unlock_by_edge_split(AFM_data & data,
                          const uint pid,
                          const uint vid)
{
    assert(data.m1.poly_contains_vert(pid,vid));

    // retrieve CCW ordered edge vertices
    uint eid = data.m1.edge_opposite_to(pid,vid);
    uint e[2] = { data.m1.edge_vert_id(eid,0), data.m1.edge_vert_id(eid,1) };
    if(!data.m1.poly_verts_are_CCW(pid,e[1],e[0])) std::swap(e[0],e[1]);
    if(data.enable_sanity_checks) assert(!flipped(data,vid,e[0],e[1]));

    // should I split edge vid-e[0] or edgevid-e[1]?
    // The correct edge is the one from which I can "see" the front origin
    uint off=0;
    if(flipped(data,vid,e[0],data.origin))
    {
        off = 1;
        if(data.enable_sanity_checks) assert(!flipped(data,e[1],vid,data.origin));
    }

    // split point is at the intersection between lines l0 and l1
    uint l0[2] = { e[off],       vid         };
    uint l1[2] = { e[(off+1)%2], data.origin };

    vec3d p = segment_intersection(data.m1.vert(l0[0]),
                                   data.m1.vert(l0[1]),
                                   data.m1.vert(l1[0]),
                                   data.m1.vert(l1[1]));
    p = (p + data.m1.vert(vid))/2;

    CGAL_Q pp[3] = { 0, 0, 0 };
    line_intersection2d(&data.exact_coords[3*l0[0]],
                        &data.exact_coords[3*l0[1]],
                        &data.exact_coords[3*l1[0]],
                        &data.exact_coords[3*l1[1]], pp);

    midpoint(&data.exact_coords[3*vid],pp,pp);
    data.exact_coords.push_back(pp[0]);
    data.exact_coords.push_back(pp[1]);
    data.exact_coords.push_back(pp[2]);

    // this is a better version of p
    p = vec3d(CGAL::to_double(pp[0]),
              CGAL::to_double(pp[1]),
              CGAL::to_double(pp[2]));

    data.m0.edge_split(data.m0.edge_id(vid,e[off]), 0.5); // just split at the midpoint in the input mesh...
    data.m1.edge_split(data.m1.edge_id(vid,e[off]), p);
    snap_rounding(data,data.exact_coords.size()/3-1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void vertex_unlock(AFM_data & data,
                   const uint     vid,
                   const CGAL_Q * p)
{
    assert(data.m1.verts_are_adjacent(vid,data.origin));

    uint count = 0;
    std::vector<uint> locks;
    while(vertex_locks(data,vid,locks, p))
    {
        unlock_by_edge_split(data,locks.front(), vid);
        if(++count%100==0) std::cout << count << " iters of vertex unlock refinement. vert valence: " << data.m1.vert_valence(vid) << std::endl;
    }
    if(data.enable_sanity_checks) assert(vertex_locks(data,vid,locks,p)==0);
}

}

