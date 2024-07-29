/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2024: Marco Livesu                                              *
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
#include <cinolib/subdivision_1_to_4.h>

namespace cinolib
{

// Splits each mesh triangle into 4 subtriangles, splitting mesh edges at their midpoint
//
template<class M, class V, class E, class P>
CINO_INLINE
void subdivision_1_to_4(Trimesh<M,V,E,P> & m)
{
    uint nv = m.num_verts();

    // add edge midpoints
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        m.vert_add(m.edge_sample_at(eid,0.5));
    }

    // create subtriangles
    uint np = m.num_polys();
    for(uint pid=0; pid<np; ++pid)
    {
        auto v = m.poly_verts_id(pid);
        if(!m.poly_verts_are_CCW(pid,v[1],v[0])) std::swap(v[1],v[0]);

        //       v2
        //      /   \
        //   e02 -- e12
        //   /  \   /  \
        // v0 -- e01 -- v1

        int e01 = m.edge_id(v[0],v[1]); assert(e01>=0);
        int e12 = m.edge_id(v[1],v[2]); assert(e12>=0);
        int e02 = m.edge_id(v[0],v[2]); assert(e02>=0);

        uint v01 = nv + e01;
        uint v12 = nv + e12;
        uint v02 = nv + e02;

        m.poly_add(v[0], v01, v02);
        m.poly_add( v01, v12, v02);
        m.poly_add( v01,v[1], v12);
        m.poly_add( v02, v12,v[2]);
    }

    // remove old triangles
    //
    // NOTE: since I am deleting all original triangles, it would probably be
    // more efficient to just rebuild the mesh from scratch. However, I may have
    // custom data attached to the mesh elements that I want to save....
    std::vector<uint> del(np);
    std::iota(del.begin(),del.end(),0);
    m.polys_remove(del);
}

}
