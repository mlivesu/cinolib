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
#include <cinolib/extrude_mesh.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void extrude_mesh(AbstractPolygonMesh<M,V,E,P> & m, const vec3d & dir) // encode amount of extrusion in dir magnitude
{
    uint nv = m.num_verts();
    uint np = m.num_polys();
    uint ne = m.num_edges();

    std::unordered_map<uint,uint> v_map;
    for(uint vid=0; vid<nv; ++vid)
    {
        v_map[vid] = m.vert_add(m.vert(vid) + dir);
    }

    for(uint pid=0; pid<np; ++pid)
    {
        std::vector<uint> p;
        for(uint vid : m.adj_p2v(pid)) p.push_back(v_map.at(vid));
        if(dir.dot(m.poly_data(pid).normal)<0)
        {
            m.poly_flip_winding_order(pid);   // if extruding along the normal direction, flip winding order
        }
        else
        {
            std::reverse(p.begin(), p.end()); // otherwise flip the new face
        }
        m.poly_add(p);
    }

    for(uint eid=0; eid<ne; ++eid)
    {
        if(m.edge_is_boundary(eid))
        {
            uint A = m.edge_vert_id(eid, 0);
            uint B = m.edge_vert_id(eid, 1);
            uint pid = m.adj_e2p(eid).front();
            if(!m.poly_verts_are_CCW(pid, A, B)) std::swap(A, B);
            uint Ap = v_map.at(A);
            uint Bp = v_map.at(B);
            m.poly_add({A, B,  Bp});
            m.poly_add({A, Bp, Ap});
        }
    }
}

}

