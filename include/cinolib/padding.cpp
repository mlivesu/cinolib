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
#include <cinolib/padding.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void padding(AbstractPolyhedralMesh<M,V,E,F,P> & m, const bool inwards)
{
    uint nv = m.num_verts();
    uint ne = m.num_edges();
    uint nf = m.num_faces();

    double l = m.edge_min_length()*0.1;

    // add copies of surface vertices
    std::unordered_map<uint,uint> vmap;
    for(uint vid=0; vid<nv; ++vid)
    {
        if(m.vert_is_on_srf(vid))
        {
            uint  new_vid = m.vert_add(m.vert(vid));       // update position;
            vec3d off     = m.vert_data(vid).normal*l*0.5;
            if(inwards) m.vert(  vid  ) -= off;
            else        m.vert(new_vid) += off;
            vmap[vid] = new_vid;
        }
    }

    // add copies of surface faces
    std::unordered_map<uint,uint> fmap;
    for(uint fid=0; fid<nf; ++fid)
    {
        if(m.face_is_on_srf(fid))
        {
            auto f = m.face_verts_id(fid);
            for(auto & vid : f) vid = vmap.at(vid);
            uint new_fid = m.face_add(f);
            fmap[fid] = new_fid;
        }
    }

    // add inner faces for each surface edge
    std::unordered_map<uint,uint> emap;
    for(uint eid=0; eid<ne; ++eid)
    {
        if(m.edge_is_on_srf(eid))
        {
            auto e = m.edge_vert_ids(eid);
            std::vector<uint> f =
            {
                e.at(0),
                e.at(1),
                vmap.at(e.at(1)),
                vmap.at(e.at(0))
            };
            uint new_fid = m.face_add(f);
            emap[eid] = new_fid;
        }
    }

    // eventually add padding cells
    for(auto srf : fmap)
    {
        uint fid     = srf.first;
        uint new_fid = srf.second;

        std::vector<uint> p; // cell face list
        std::vector<bool> w; // cell face winding

        // add srf face and the copy of it
        p.push_back(fid);
        p.push_back(new_fid);
        // assign opposite winding to the original srf face
        assert(m.adj_f2p(fid).size()==1);
        uint pid = m.adj_f2p(fid).front();
        w.push_back(!m.poly_face_is_CCW(pid,fid));
        w.push_back( m.poly_face_is_CCW(pid,fid));

        // add lateral faces
        for(uint eid : m.adj_f2e(fid))
        {
            uint new_fid = emap.at(eid);
            auto e = m.edge_vert_ids(eid);
            p.push_back(new_fid);
            w.push_back(!m.face_verts_are_CCW(fid, e.at(1), e.at(0)));
        }

        // create cell and order its vertices if it is a haxahedron
        uint new_pid = m.poly_add(p,w);
        if(m.poly_is_hexahedron(new_pid))
        {
            m.poly_reorder_p2v(new_pid);
            m.update_p_quality(new_pid);
        }
    }
}

}
