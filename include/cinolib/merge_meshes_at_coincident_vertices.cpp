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
#include <cinolib/merge_meshes_at_coincident_vertices.h>
#include <cinolib/octree.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void merge_meshes_at_coincident_vertices(const AbstractPolyhedralMesh<M,V,E,F,P> & m1,
                                         const AbstractPolyhedralMesh<M,V,E,F,P> & m2,
                                               AbstractPolyhedralMesh<M,V,E,F,P> & res,
                                         const double                              proximity_thresh)
{
    Octree o;
    for(uint vid=0; vid<m1.num_verts(); ++vid) o.push_sphere(vid, m1.vert(vid), proximity_thresh);
    o.build();

    res = m1;

    std::map<uint,uint> vmap;
    for(uint vid=0; vid<m2.num_verts(); ++vid)
    {
        vec3d p = m2.vert(vid);
        std::unordered_set<uint> ids;
        if(o.contains(p, false, ids))
        {
            // WARNING: I am assuming that the mapping is one to one at most
            assert(ids.size()==1);
            vmap[vid] = *ids.begin();
        }
        else
        {
            uint fresh_id = res.vert_add(p);
            vmap[vid] = fresh_id;
        }
    }

    std::map<uint,uint> fmap;
    for(uint fid=0; fid<m2.num_faces(); ++fid)
    {
        auto f = m2.face_verts_id(fid);
        for(auto & vid : f) vid = vmap.at(vid);

        int test_id = res.face_id(f);
        if(test_id>=0)
        {
            fmap[fid] = test_id;
        }
        else
        {
            uint fresh_id = res.face_add(f);
            fmap[fid] = fresh_id;
        }
    }

    for(uint pid=0; pid<m2.num_polys(); ++pid)
    {
        auto p = m2.poly_faces_id(pid);
        for(auto & fid : p) fid = fmap.at(fid);

        int test_id = res.poly_id(p);
        if(test_id==-1)
        {
            res.poly_add(p, m2.poly_faces_winding(pid));
        }
    }
}

}
