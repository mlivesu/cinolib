/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/meshes/abstract_polygonmesh.h>
#include <assert.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void check_topology(const AbstractPolygonMesh<M,V,E,P> & m)
{
    std::cout << "TOPOLOGY CHECK...";

    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        for(uint nbr : m.adj_v2v(vid))
        {
            assert(nbr < m.num_verts());
            assert(CONTAINS_VEC(m.adj_v2v(nbr), vid));
            assert(m.edge_id(vid, nbr) >= 0); // there exists an edge (vid, nbr)
            assert(std::count(m.adj_v2v(vid).begin(), m.adj_v2v(vid).end(), nbr) == 1);
        }
        for(uint eid : m.adj_v2e(vid))
        {
            assert(eid < m.num_edges());
            assert(m.edge_contains_vert(eid,vid));
            assert(std::count(m.adj_v2e(vid).begin(), m.adj_v2e(vid).end(), eid) == 1);
        }
        for(uint pid : m.adj_v2p(vid))
        {
            assert(pid < m.num_polys());
            assert(CONTAINS_VEC(m.adj_p2v(pid), vid));
            assert(std::count(m.adj_v2p(vid).begin(), m.adj_v2p(vid).end(), pid) == 1);
        }
    }

    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        uint vid0 = m.edge_vert_id(eid,0);
        uint vid1 = m.edge_vert_id(eid,1);

        assert(vid0 < m.num_verts());
        assert(vid1 < m.num_verts());

        assert(CONTAINS_VEC(m.adj_v2v(vid0), vid1));
        assert(CONTAINS_VEC(m.adj_v2v(vid1), vid0));

        for(uint pid : m.adj_e2p(eid))
        {
            assert(pid < m.num_polys());
            assert(CONTAINS_VEC(m.adj_p2e(pid), eid));
            assert(std::count(m.adj_e2p(eid).begin(), m.adj_e2p(eid).end(), pid) == 1);
        }

        for(uint pid0 : m.adj_e2p(eid))
        for(uint pid1 : m.adj_e2p(eid))
        {
            if (pid0 == pid1) continue;
            assert(CONTAINS_VEC(m.adj_p2p(pid0), pid1));
            assert(CONTAINS_VEC(m.adj_p2p(pid1), pid0));
        }
    }

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        for(uint i=0; i<m.adj_p2v(pid).size(); ++i)
        {
            uint vid0 = m.adj_p2v(pid).at(i);
            uint vid1 = m.adj_p2v(pid).at((i+1)%m.verts_per_poly(pid));
            assert(CONTAINS_VEC(m.adj_v2v(vid0), vid1));
            assert(CONTAINS_VEC(m.adj_v2v(vid1), vid0));
            assert(m.edge_id(vid0, vid1) >= 0); // there exist an edge (vid, nbr)
            assert(m.edge_id(vid1, vid0) >= 0); // there exist an edge (vid, nbr)
        }
        for(uint vid : m.adj_p2v(pid))
        {
            assert(vid < m.num_verts());
            assert(CONTAINS_VEC(m.adj_v2p(vid), pid));
            assert(std::count(m.adj_p2v(pid).begin(), m.adj_p2v(pid).end(), vid) == 1);
        }
        for(uint eid : m.adj_p2e(pid))
        {
            assert(eid < m.num_edges());
            assert(CONTAINS_VEC(m.adj_e2p(eid), pid));
            assert(std::count(m.adj_p2e(pid).begin(), m.adj_p2e(pid).end(), eid) == 1);
        }
        for(uint nbr : m.adj_p2p(pid))
        {
            assert(nbr < m.num_polys());
            assert(CONTAINS_VEC(m.adj_p2p(nbr), pid));
            assert(std::count(m.adj_p2p(pid).begin(), m.adj_p2p(pid).end(), nbr) == 1);
        }
        for(uint vid : m.poly_tessellation(pid))
        {
            assert(vid < m.num_verts());
            assert(CONTAINS_VEC(m.adj_v2p(vid), pid));
            assert(CONTAINS_VEC(m.adj_p2v(pid), vid));
        }
    }
    std::cout << "passed!" << std::endl;
}


}
