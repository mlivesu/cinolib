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
#include <cinolib/canonical_polygonal_schema.h>
#include <cinolib/cut_mesh.h>
#include <cinolib/geometry/n_sided_poygon.h>
#include <cinolib/harmonic_map.h>
#include <cinolib/sampling.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void canonical_polygonal_schema(      Trimesh<M,V,E,P>  & m_in,
                                const HomotopyBasisData & basis,
                                      Trimesh<M,V,E,P>  & m_out,
                                const int                 laplacian_mode)
{
    std::cout << "Canonical Polygonal Schema" << std::endl;

    // In order to Basis loop
    assert(basis.detach_loops);

    int genus = m_in.genus();
    assert(genus>0);
    std::cout << "genus: " << genus << std::endl;

    // label vertices according to the basis loop they belong to.
    // this serves to identify the edges of the 4g-gon
    m_in.vert_apply_label(-1);
    for(uint lid=0; lid<basis.loops.size(); ++lid)
    {
        for(uint vid : basis.loops.at(lid))
        {
            m_in.vert_data(vid).label = lid;
        }
    }
    std::unordered_map<uint,std::vector<uint>> v_map;
    cut_mesh_along_marked_edges(m_in, v_map);

    // detect all the 4g copies of the basis' root
    std::vector<uint> copies_of_root = v_map.at(basis.root);
    copies_of_root.push_back(basis.root);
    assert(!copies_of_root.empty());
    m_in.vert_unmark_all();
    for(uint vid : copies_of_root)
    {
        assert(m_in.vert_is_boundary(vid));
        m_in.vert_data(vid).flags[MARKED] = true;
    }

    // topological checks: make sure no element has all its vertices on the border,
    // otherwise it will map to a degenerate triangle
    std::vector<uint> split_list;
    for(uint eid=0; eid<m_in.num_edges(); ++eid)
    {
        if(m_in.edge_is_boundary(eid)) continue;
        uint v0 = m_in.edge_vert_id(eid,0);
        uint v1 = m_in.edge_vert_id(eid,1);
        if(m_in.vert_is_boundary(v0) && m_in.vert_is_boundary(v1) &&
          (m_in.vert_data(v0).label==m_in.vert_data(v1).label))
        {
            split_list.push_back(eid);
        }
    }
    std::cout << "splitting " << split_list.size() << " edges to avoid degenerate elements along the boundary" << std::endl;
    for(uint eid : split_list) m_in.edge_split(eid);

    std::vector<uint> border = m_in.get_ordered_boundary_vertices();
    // rotate the list so as to have a polygon corner at the beginning of it
    CIRCULAR_SHIFT_VEC(border, copies_of_root.front());
    assert(m_in.vert_data(border.front()).flags[MARKED]);

    // split the boundary into 4g edges
    std::vector<std::vector<uint>> edges;
    for(uint i=0; i<border.size(); ++i)
    {
        std::vector<uint> e = { border.at(i) };
        for(uint j=i+1; j<border.size() && !m_in.vert_data(border.at(j)).flags[MARKED]; ++j,++i)
        {
            e.push_back(border.at(j));
        }
        e.push_back(border.at((i+1)%border.size()));
        edges.push_back(e);
    }

    std::vector<vec3d> poly = n_sided_polygon(vec3d(0,0,0), genus*4, 1.0);
    for(auto & p : poly) rotate(p, vec3d(0,0,1), M_PI*0.25); // if g=1, align square with uv frame
    std::map<uint,vec3d> dirichlet_bcs;
    for(uint i=0; i<poly.size(); ++i)
    {
        std::vector<vec3d> e_bcs = sample_within_interval(poly.at(i), poly.at((i+1)%poly.size()), edges.at(i).size());
        for(uint j=0; j<e_bcs.size(); ++j) dirichlet_bcs[edges.at(i).at(j)] = e_bcs.at(j);
    }

    // map the interior vertices
    m_out = m_in;
    m_out.vector_verts() = harmonic_map_3d(m_in, dirichlet_bcs, 1, laplacian_mode);
    m_out.update_bbox();
    m_out.update_normals();    
}

}
