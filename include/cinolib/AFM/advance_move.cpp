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
#include <cinolib/AFM/advance_move.h>
#include <cinolib/AFM/flip_checks.h>
#include <cinolib/AFM/convexification.h>
#include <cinolib/AFM/concavification.h>
#include <cinolib/AFM/snap_rounding.h>

namespace cinolib
{

// advance front edge v0-v1, adding triangle pid with a vertex split
CINO_INLINE
bool advance_by_triangle_split(      AFM_data & data,
                               const uint       pid, // w.r.t. m0
                                     uint       v0,  // w.r.t. both m0 and m1
                                     uint       v1,  // w.r.t. both m0 and m1
                               const bool       update_split_point_coords)
{
    if(data.m0.poly_verts_are_CCW(pid,v0,v1)) std::swap(v0,v1);

    /////// PRECONDITIONS ///////

    if(data.enable_sanity_checks) assert(!flipped(data,v0,v1,data.origin));
    // pick the vertex in pid that is opposite to the front edge
    uint v2 = data.m0.vert_opposite_to(pid, v0, v1);
    // front locally converged, nothing to do here
    if(v2==data.origin) return false;
    // conquering this triangle would make the front non simply connected
    if(data.m0.vert_data(v2).flags[MARKED]) return false;

    /////// ADVANCE FRONT IN M1 ///////

    if(update_split_point_coords)
    {
        CGAL_Q * O  = & data.exact_coords[3*data.origin];
        CGAL_Q * V0 = & data.exact_coords[3*v0];
        CGAL_Q * V1 = & data.exact_coords[3*v1];
        CGAL_Q * V2 = & data.exact_coords[3*v2];
        //midpoint(V0,V1,O,V2);
        V2[0] = (V0[0]*99 + V1[0]*99 + O[0]*2)/200;
        V2[1] = (V0[1]*99 + V1[1]*99 + O[1]*2)/200;
        V2[2] = (V0[2]*99 + V1[2]*99 + O[2]*2)/200;
        if(data.enable_sanity_checks)
        {
            assert(orient2d(V0,V1,V2)>0);
            assert(orient2d(V1,O,V2 )>0);
            assert(orient2d(V0,V2,O )>0);
        }
        data.m1.vert(v2) = vec3d(CGAL::to_double(V2[0]),
                                 CGAL::to_double(V2[1]),
                                 CGAL::to_double(V2[2]));
    }
    uint new_pid = data.m1.poly_add(v0,v1,v2);
    data.m1.poly_data(new_pid).color = data.conquered_color;
    data.m1.poly_add(v0,v2,data.origin);
    data.m1.poly_add(v1,data.origin,v2);
    if(update_split_point_coords) snap_rounding(data,v2); // do not snap of the poit was already CAREFULY placed!
    int rem = data.m1.poly_id({data.origin,v0,v1});
    assert(rem>=0);
    data.m1.poly_remove(rem);

    /////// UPDATE FRONT FLAGS ///////

    // update m0 flags
    data.m0.vert_data(v2).flags[MARKED]  = true;
    data.m0.poly_data(pid).flags[MARKED] = true;
    data.m0.poly_data(pid).color = data.conquered_color;
    for(uint eid : data.m0.adj_p2e(pid))
    {
        data.m0.edge_data(eid).flags[MARKED] = (!data.m0.edge_contains_vert(eid,v0) ||
                                                !data.m0.edge_contains_vert(eid,v1));
    }
    // update m1 flags
    int e0 = data.m1.edge_id(v0,v1); assert(e0>=0);
    int e1 = data.m1.edge_id(v1,v2); assert(e1>=0);
    int e2 = data.m1.edge_id(v2,v0); assert(e2>=0);
    data.m1.edge_data(e0).flags[MARKED] = false;
    data.m1.edge_data(e1).flags[MARKED] = true;
    data.m1.edge_data(e2).flags[MARKED] = true;
    // add edges v0-v2 and v1-v2 to the front
    data.front.push_back(v0);
    data.front.push_back(v2);
    data.front.push_back(v1);
    data.front.push_back(v2);

    /////// POSTCONDITIONS ///////

    if(data.enable_sanity_checks)
    {
        assert(!flipped(data,v0,v1,v2));
        assert(!flipped(data,v0,v2,data.origin));
        assert(!flipped(data,v2,v1,data.origin));
    }

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// advance the front by adding triangle pid with an edge flip
CINO_INLINE
bool advance_by_edge_flip(AFM_data & data, const uint pid)
{
    /////// PRECONDITIONS ///////

    // find the edge not currently in the front (to be inserted with an edge flip)
    uint e_front = 0;
    if(!data.m0.edge_data(data.m0.poly_edge_id(pid,0)).flags[MARKED]) e_front = data.m0.poly_edge_id(pid,0); else
    if(!data.m0.edge_data(data.m0.poly_edge_id(pid,1)).flags[MARKED]) e_front = data.m0.poly_edge_id(pid,1); else
    if(!data.m0.edge_data(data.m0.poly_edge_id(pid,2)).flags[MARKED]) e_front = data.m0.poly_edge_id(pid,2); else
    throw("This is not supposed to happen");

    // flip edge cd such that triangles acd and dcb become abc and cbd
    uint v0 = data.m0.edge_vert_id(e_front,0);
    uint v1 = data.m0.edge_vert_id(e_front,1);
    uint v2 = data.m0.vert_opposite_to(pid,v0,v1);
    if(!data.m0.poly_verts_are_CCW(pid,v1,v0)) std::swap(v0,v1);

    // if the front is locally concave force convexification prior to edge flipping
    bool had_convexified = false;
    if(flipped(data,v0,v1,v2))
    {
        ++data.convexifications;
        if(data.refinement_enabled)
        {
            convexify_front(data,v0,v1,v2);
            had_convexified = true; // to remind me to attemp a snap rounding at the end
        }
        else return false;
    }

    // if the convex pocket contains the origin, add a new vertex and make it concave
    // to make sure the origin of the front remains outside of the new triangles
    if(flipped(data,v1,v0,data.origin))
    {
        ++data.concavifications;
        if(data.refinement_enabled)
        {
            // utility function to find explore the front locally
            auto next_front_vert = [&](const uint vid) -> uint
            {
                for(uint eid : data.m0.adj_v2e(vid))
                {
                    uint opp = data.m0.vert_opposite_to(eid,vid);
                    if(data.m0.edge_data(eid).flags[MARKED] && opp!=v2) return opp;
                }
                throw("This is not supposed to happen");
                return 0;
            };

            // trying to understand whether the next move will be an edge
            // flip again, and at what side of the ear it will happen
            uint next0  = next_front_vert(v0);
            uint next1  = next_front_vert(v1);
            bool side0  = (data.m0.poly_id({v0,next0,v1})>=0);
            bool side1  = (data.m0.poly_id({v1,next1,v0})>=0);
            bool origin = (data.m0.poly_id({v1,data.origin,v0})>=0);

            // use this move only to unlock a deadlock configuration
            if(!side0 && !side1 && !origin) return false;

            if(side1)
            {
                std::swap(v0,v1);
                std::swap(next0,next1);
            }
            else if(origin) next0 = data.origin;

            uint new_vid = concavify_front(data,v0,v1,v2,next0);
            int pid = data.m0.poly_id({new_vid,v0,v2});
            assert(pid>=0);
            bool b0 = advance_by_triangle_split(data, pid,v0,v2,false);
            pid = data.m0.poly_id({v2,v1,new_vid});
            assert(pid>=0);
            bool b1 = advance_by_edge_flip(data, pid);
            assert(b0 && b1);
            if(next0!=data.origin)
            {
                bool b2 = advance_by_edge_flip(data, data.m0.poly_id({new_vid,next0,v0}));
                assert(b2);
            }
            snap_rounding(data,new_vid);
            return true;
        }
        else return false;
    }

    /////// ADVANCE FRONT IN M1 ///////

    int e_flip = data.m1.edge_id(data.origin,v2);
    assert(e_flip>=0);
    int err = data.m1.edge_flip(e_flip,false);
    assert(err>=0);
    int new_pid = data.m1.poly_id({v0,v1,v2});
    assert(new_pid>=0);
    data.m1.poly_data(new_pid).color = data.conquered_color;

    /////// UPDATE FRONT FLAGS ///////

    // update m0 flags
    for(uint eid : data.m0.adj_p2e(pid)) data.m0.edge_data(eid).flags[MARKED] = false;
    data.m0.edge_data(e_front).flags[MARKED] = true;
    data.m0.poly_data(pid).flags[MARKED] = true;
    data.m0.poly_data(pid).color = data.conquered_color;
    // update m1 flags
    int e0 = data.m1.edge_id(v0,v1); assert(e0>=0);
    int e1 = data.m1.edge_id(v1,v2); assert(e1>=0);
    int e2 = data.m1.edge_id(v2,v0); assert(e2>=0);
    data.m1.edge_data(e0).flags[MARKED] = true;
    data.m1.edge_data(e1).flags[MARKED] = false;
    data.m1.edge_data(e2).flags[MARKED] = false;
    // add new front edge
    data.front.push_back(v0);
    data.front.push_back(v1);

    if(had_convexified)
    {
        snap_rounding(data,v0);
        snap_rounding(data,v1);
    }

    /////// POSTCONDITIONS ///////

    if(data.enable_sanity_checks)
    {
        for(uint pid : data.m1.adj_v2p(v0)) assert(!flipped(data,pid));
        for(uint pid : data.m1.adj_v2p(v1)) assert(!flipped(data,pid));
    }
    return true;
}

}
