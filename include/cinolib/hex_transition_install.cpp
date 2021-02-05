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
#include <cinolib/hex_transition_install.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_transition_install(Polyhedralmesh<M,V,E,F,P> & m,
                            const HexTransition         type,
                            const vec3d               & center,
                            const double                scale,
                            const int                   orientation)
{
    double eps = m.edge_min_length()*0.1;
    Octree octree;
    for(uint vid=0; vid<m.num_verts(); ++vid) octree.push_sphere(vid, m.vert(vid), eps);
    octree.build();

    hex_transition_install(m, octree, type, center, scale, orientation);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// version that takes a pre-built octree (to avoid making one octree for each installation)
template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_transition_install(Polyhedralmesh<M,V,E,F,P> & m,
                            const Octree              & octree,
                            const HexTransition         type,
                            const vec3d               & center,
                            const double                scale,
                            const int                   orientation)
{
    std::vector<double>            verts;
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    std::vector<std::vector<bool>> winding;
    hex_transition(type, verts, faces, polys, winding, center, scale, orientation);

    // merge vertices
    uint nv = verts.size()/3;
    std::map<uint,uint> vmap;
    for(uint vid=0; vid<nv; ++vid)
    {
        uint  ptr = 3*vid;
        vec3d p(verts.at(ptr  ),
                verts.at(ptr+1),
                verts.at(ptr+2));

        std::unordered_set<uint> ids;
        if(octree.contains(p, false, ids))
        {
            // WARNING: I am assuming that the mapping is one to one at most
            assert(ids.size()==1);
            vmap[vid] = *ids.begin();
        }
        else
        {
            uint fresh_id = m.vert_add(p);
            vmap[vid] = fresh_id;
        }
    }

    // merge faces
    std::map<uint,uint> fmap;
    for(uint fid=0; fid<faces.size(); ++fid)
    {
        auto f = faces.at(fid);
        for(auto & vid : f) vid = vmap.at(vid);

        int test_id = m.face_id(f);
        if(test_id>=0)
        {
            fmap[fid] = test_id;
        }
        else
        {
            uint fresh_id = m.face_add(f);
            fmap[fid] = fresh_id;
        }
    }

    // merge polys
    for(uint pid=0; pid<polys.size(); ++pid)
    {
        auto p = polys.at(pid);
        for(auto & fid : p) fid = fmap.at(fid);

        int test_id = m.poly_id(p);
        if(test_id==-1)
        {
            m.poly_add(p, winding.at(pid));
        }
    }
}

}
