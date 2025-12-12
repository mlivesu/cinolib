/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2025: Marco Livesu                                              *
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
#include <cinolib/subdivision_loop.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void subdivision_Loop(Tetmesh<M,V,E,F,P> & m)
{
    std::vector<uint> e_splits(m.num_edges());
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        e_splits.at(eid) = m.vert_add(m.edge_sample_at(eid,0.5));
    }

    uint np = m.num_polys();
    for(uint pid=0; pid<np; ++pid)
    {
        uint v0 = m.poly_vert_id(pid,0);
        uint v1 = m.poly_vert_id(pid,1);
        uint v2 = m.poly_vert_id(pid,2);
        uint v3 = m.poly_vert_id(pid,3);

        uint v01 = e_splits.at(m.edge_id(v0,v1));
        uint v12 = e_splits.at(m.edge_id(v1,v2));
        uint v20 = e_splits.at(m.edge_id(v2,v0));
        uint v03 = e_splits.at(m.edge_id(v0,v3));
        uint v13 = e_splits.at(m.edge_id(v1,v3));
        uint v23 = e_splits.at(m.edge_id(v2,v3));

        // corners
        m.poly_add({v20, v01, v03, v0});
        m.poly_add({v13, v23, v03, v3});
        m.poly_add({v23, v12, v20, v2});
        m.poly_add({v12, v13, v01, v1});

        // inner octahedron
        m.poly_add({v01, v23, v12, v20});
        m.poly_add({v01, v23, v20, v03});
        m.poly_add({v01, v23, v03, v13});
        m.poly_add({v01, v23, v13, v12});

        // TODO: I should tetrahedralize the inner octahedron
        // by always considering the longest inner diagonal
        // connecting pairs of splitpoints associated to
        // opposite tet edges
    }

    // remove old elements
    for(int pid=np-1; pid>=0; --pid) m.poly_remove(pid);
}

}
