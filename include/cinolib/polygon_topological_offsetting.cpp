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
#include <cinolib/polygon_topological_offsetting.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/earcut.h>

namespace cinolib
{

CINO_INLINE
void polygon_topological_offsetting(std::vector<vec3d> & poly,
                                    std::vector<uint>  & tris,
                                    std::vector<uint>  & vmap)
{
    // step 0: triangulate the polygon and make a mesh out of it
    // (the better the triangulation the more numerically stable...)
    earcut(poly,tris, EarSorting::PRIORITIZED);
    Trimesh<> m(poly,tris);

    // step 1: refine the mesh so that each boundary
    // vertex has at least three incoming edges (two buoundaries and one interior)
    uint nv = m.num_verts();
    for(uint vid=0; vid<nv; ++vid)
    {
        assert(m.vert_is_boundary(vid));
        if(m.vert_valence(vid)<3)
        {
            uint pid = m.adj_v2p(vid).front();
            uint eid = m.edge_opposite_to(pid, vid);
            m.edge_split(eid);
        }
    }

    // step 2: walk along the boundary, visiting the ordered set of incident edges
    // and adding offset points along them
    std::vector<std::vector<uint>> inner_verts(nv);
    vmap.resize(nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        // pick the ordered list of incoming edges
        auto e_star = m.vert_ordered_edges_star(vid);
        uint nbr = m.vert_opposite_to(e_star.front(), vid);
        if(nbr==(vid+1)%nv) std::reverse(e_star.begin(), e_star.end());
        nbr = m.vert_opposite_to(e_star.front(), vid);
        assert(nbr!=(vid+1)%nv);

        for(auto it=e_star.begin()+1; it+1!=e_star.end(); ++it)
        {
            uint  eid = *it;
            uint  nbr = m.vert_opposite_to(eid, vid);
            vec3d p   = m.vert(vid)*0.6 + m.vert(nbr)*0.4;
            inner_verts.at(vid).push_back(poly.size());
            poly.push_back(p);
        }
        vmap.at(vid) = inner_verts.at(vid).size();
    }

    // step 3: triangulate offsetted polygon
    tris.clear();
    for(uint vid=0; vid<nv; ++vid)
    {
        for(uint i=1; i<inner_verts.at(vid).size(); ++i)
        {
            tris.push_back(vid);
            tris.push_back(inner_verts.at(vid).at(i));
            tris.push_back(inner_verts.at(vid).at(i-1));
        }

        if(vid>0)
        {
            tris.push_back(vid-1);
            tris.push_back(vid);
            tris.push_back(inner_verts.at(vid-1).back());
            //
            tris.push_back(inner_verts.at(vid).front());
            tris.push_back(inner_verts.at(vid-1).back());
            tris.push_back(vid);
        }
        else
        {
            tris.push_back(nv-1);
            tris.push_back(vid);
            tris.push_back(inner_verts.at(nv-1).back());
            //
            tris.push_back(inner_verts.at(vid).front());
            tris.push_back(inner_verts.at(nv-1).back());
            tris.push_back(vid);
        }
    }
}

}
