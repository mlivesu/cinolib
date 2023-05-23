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
#include <cinolib/AFM/concavification.h>
#include <cinolib/AFM/flip_checks.h>
#include <cinolib/AFM/snap_rounding.h>
#include <cinolib/geometry/segment_utils.h>

namespace cinolib
{

CINO_INLINE
uint concavify_front(AFM_data & data,
                                const uint v0,
                                const uint v1,
                                const uint v2,
                                const uint v3) // next to v0 along the front, forms a triangle with v0,v1
{
    // takes into account the possibility that v0,v1,v2 are not CCW
    bool CCW = !flipped(data,v0,v2,v1);

    if(data.enable_sanity_checks)
    {
        assert(data.m0.poly_id({v0,v1,v3})>=0);
        assert(data.m0.poly_id({v0,v1,v2})>=0);
        assert(!flipped(data,v0,v2,v1) == CCW);
        assert(!flipped(data,v3,v0,data.origin) == CCW);
        assert(!flipped(data,v0,v2,data.origin) == CCW);
        assert(!flipped(data,v2,v1,data.origin) == CCW);
        //assert(flipped(data,v0,v1,data.origin)  == CCW); // WRONG! This may be zero
    }

    // initialize split point
    uint split_point_id = data.exact_coords.size()/3;
    data.exact_coords.push_back(0);
    data.exact_coords.push_back(0);
    data.exact_coords.push_back(0);

    // if the next flip is concave, just focus on this one
    // (the next will be made valid by the convexification routine)

    auto res = orient2d(&data.exact_coords[3*v0],
                        &data.exact_coords[3*v2],
                        &data.exact_coords[3*v3]);
    if(res==0 || (res<0) == CCW || v3==data.origin)
    {
        CGAL_Q A[3] =
        {
            (data.exact_coords[3*data.origin+0] + data.exact_coords[3*v2+0]*99)/100,
            (data.exact_coords[3*data.origin+1] + data.exact_coords[3*v2+1]*99)/100,
            (data.exact_coords[3*data.origin+2] + data.exact_coords[3*v2+2]*99)/100
        };
        //midpoint(&data.exact_coords[3*data.origin],&data.exact_coords[3*v2],A);
        //
        CGAL_Q B[3] = { 0, 0, 0 };
        line_intersection2d(&data.exact_coords[3*v1], A, &data.exact_coords[3*v0], &data.exact_coords[3*v2], B);
        if(data.enable_sanity_checks)
        {
            assert(orient2d(&data.exact_coords[3*v0],B,&data.exact_coords[3*data.origin]) *
                   orient2d(B,&data.exact_coords[3*v2],&data.exact_coords[3*data.origin])>0);
        }
        //
        midpoint(A,B,&data.exact_coords[3*split_point_id]);
    }
    // conversely, if the next flip is convex, make sure it's already doable,
    // (i.e.) the triangle it generates does not contain the front origin
    else
    {
        CGAL_Q A[3] = { 0, 0, 0 };
        line_intersection2d(&data.exact_coords[3*v1],
                            &data.exact_coords[3*data.origin],
                            &data.exact_coords[3*v0],
                            &data.exact_coords[3*v2], A);
        // sanity checks
        // if O,v0,v1 are aligned, then A==v0, that's why >= and not >
        if(data.enable_sanity_checks)
        {
            assert(orient2d(&data.exact_coords[3*v0],A,&data.exact_coords[3*data.origin]) *
                   orient2d(A,&data.exact_coords[3*v2],&data.exact_coords[3*data.origin])>=0);
        }

        //
        CGAL_Q B[3] = { 0, 0, 0 };
        line_intersection2d(&data.exact_coords[3*v3],
                            &data.exact_coords[3*data.origin],
                            &data.exact_coords[3*v0],
                            &data.exact_coords[3*v2], B);
        // if B does not lie in between v0 and v2, set B as v2
        if(orient2d(&data.exact_coords[3*v0],B,&data.exact_coords[3*data.origin]) *
           orient2d(B,&data.exact_coords[3*v2],&data.exact_coords[3*data.origin])<=0)
        {
            B[0] = data.exact_coords[3*v2+0];
            B[1] = data.exact_coords[3*v2+1];
            B[2] = data.exact_coords[3*v2+2];
        }

        // make sure A comes "before" B in the segment v0-v2
        if(data.enable_sanity_checks)
        {
            assert(orient2d(&data.exact_coords[3*v0],A,&data.exact_coords[3*data.origin]) *
                   orient2d(A,B,&data.exact_coords[3*data.origin])>0);
        }

        data.exact_coords[3*split_point_id+0] = (A[0]*49 + B[0]*49 + data.exact_coords[3*data.origin+0]*2)/100;
        data.exact_coords[3*split_point_id+1] = (A[1]*49 + B[1]*49 + data.exact_coords[3*data.origin+1]*2)/100;
        data.exact_coords[3*split_point_id+2] = (A[2]*49 + B[2]*49 + data.exact_coords[3*data.origin+2]*2)/100;
        //midpoint(A,B,&data.exact_coords[3*data.origin],&data.exact_coords[3*split_point_id]);

        // sanity checks
        if(data.enable_sanity_checks)
        {
            assert(!flipped(data,v0,split_point_id,v3) == CCW);
            assert(!flipped(data,v3,split_point_id,data.origin) == CCW);
        }
    }

    // sanity checks
    if(data.enable_sanity_checks)
    {
        assert(!flipped(data,v0,v2,split_point_id) == CCW);
        assert(!flipped(data,v2,v1,split_point_id) == CCW);
        assert(!flipped(data,v0,split_point_id,data.origin) == CCW);
        assert(!flipped(data,split_point_id,v1,data.origin) == CCW);
        assert(flipped(data,split_point_id,v1,data.origin) == flipped(data,v1,split_point_id,v2));
    }

    data.m1.vert_add(vec3d(CGAL::to_double(data.exact_coords[3*split_point_id+0]),
                           CGAL::to_double(data.exact_coords[3*split_point_id+1]),
                           CGAL::to_double(data.exact_coords[3*split_point_id+2])));

    int eid = data.m0.edge_id(v0,v1);
    assert(eid>=0);
    uint vid = data.m0.edge_split(eid);
    assert(vid==split_point_id);

    return split_point_id;
}

}
