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
#include <cinolib/cut_mesh.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void cut_mesh_along_marked_edges(AbstractPolygonMesh<M,V,E,P> & m)
{
    std::unordered_map<uint,std::vector<uint>> v_map;
    cut_mesh_along_marked_edges(m, v_map);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void cut_mesh_along_marked_edges(AbstractPolygonMesh<M,V,E,P>               & m,
                                 std::unordered_map<uint,std::vector<uint>> & v_map)
{
    m.vert_unmark_all();
    v_map.clear();

    uint nv = m.num_verts();
    for(uint vid=0; vid<nv; ++vid)
    {
        if(m.vert_data(vid).flags[MARKED]) continue;
        m.vert_data(vid).flags[MARKED] = true;

        std::vector<std::vector<uint>> clusters;
        m.vert_cluster_one_ring(vid, clusters, true);

        std::vector<uint> to_remove;
        for(uint i=1; i<clusters.size(); ++i)
        {
            uint new_vid = m.vert_add(m.vert(vid));
            m.vert_data(new_vid) = m.vert_data(vid);
            v_map[vid].push_back(new_vid);

            for(uint pid : clusters.at(i))
            {
                auto verts = m.poly_verts_id(pid);
                for(uint & id : verts) if(id==vid) id = new_vid;
                m.poly_add(verts);
                to_remove.push_back(pid);
            }
        }
        m.polys_remove(to_remove);
    }
}

}
