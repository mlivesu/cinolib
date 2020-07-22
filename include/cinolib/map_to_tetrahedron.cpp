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
#include <cinolib/map_to_tetrahedron.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/predicates.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void map_to_tetrahedron(const Trimesh<M,V,E,P> & m,
                              Trimesh<M,V,E,P> & m_out)
{
    m_out = m;
    std::vector<vec3d> verts;
    map_to_tetrahedron(m, verts);
    for(uint vid=0; vid<m_out.num_verts(); ++vid) m_out.vert(vid) = verts.at(vid);
    m_out.update_bbox();
    m_out.update_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void map_to_tetrahedron(const Trimesh<M,V,E,P>   & m,
                              std::vector<vec3d> & verts) // verts mapped to tet
{
    uint v0 = 0;
    while(m.vert_valence(v0)<4 && v0<m.num_verts()) ++v0;
    assert(v0<m.num_verts());
    assert(m.vert_valence(v0)>=4);
    std::vector<uint> v_link = m.vert_ordered_verts_link(v0);
    uint v1 = v_link.at(0);
    uint v2 = v_link.at(1);
    uint v3 = v_link.at(2);
    std::vector<uint> v1v3_chain(v_link.begin()+3, v_link.end());

    // make sure you don't flip elements in the map
    if(orient3d(m.vert(v0), m.vert(v1), m.vert(v2), m.vert(v3))>0)
    {
        std::swap(v1,v3);
        std::reverse(v1v3_chain.begin(), v1v3_chain.end());
    }

    // assign tet corners
    std::map<uint,vec3d> bcs;
    bcs[v0] = REFERENCE_TET_VERTS[0];
    bcs[v1] = REFERENCE_TET_VERTS[1];
    bcs[v2] = REFERENCE_TET_VERTS[2];
    bcs[v3] = REFERENCE_TET_VERTS[3];

    // linearly interpolate the rest of verts in the link along chain connecting v1 to v3
    double step = (REFERENCE_TET_VERTS[1].dist(REFERENCE_TET_VERTS[3]))/(v_link.size()-2.0);
    vec3d  dir  = REFERENCE_TET_VERTS[1] - REFERENCE_TET_VERTS[3]; dir.normalize();
    for(uint i=0; i<v1v3_chain.size(); ++i)
    {
        bcs[v1v3_chain.at(i)] = REFERENCE_TET_VERTS[3] + (i+1)*step*dir;
    }

    // map the other vertices inside face v1-v0-v3
    verts = harmonic_map_3d(m, bcs, 1, UNIFORM);
}

}
