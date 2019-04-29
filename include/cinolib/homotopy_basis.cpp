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
#include <cinolib/homotopy_basis.h>
#include <cinolib/dijkstra.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void extend_loop_through_umbrella(Trimesh<M,V,E,P> & m, const uint v_in, const uint v_mid, const uint v_out, const uint valence)
{
    int e0 = m.edge_id(v_in,  v_mid); assert(e0>=0);
    int e1 = m.edge_id(v_mid, v_out); assert(e1>=0);
    m.edge_data(e0).label+= valence;
    m.edge_data(e1).label+= valence;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void refine_mesh_to_detach_loops(Trimesh<M,V,E,P> & m, const uint root)
{
    // enque vertices with more than 2 incident loop edges
    std::queue<uint> q;
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        if(vid == root) continue;
        uint count = 0;
        for(uint eid : m.adj_v2e(vid)) if(m.edge_data(eid).label>0) ++count;
        if(count>2) q.push(vid);
    }

    // iteratively duplicate edges traversed by more than one loop
    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        std::vector<ipair> edges_on_loop;
        for(uint eid : m.adj_v2e(vid))
        {
            int val = m.edge_data(eid).label;
            if (val>0) edges_on_loop.push_back(std::make_pair(val, m.vert_opposite_to(eid, vid)));
        }
        SORT_VEC(edges_on_loop, false); // the last one is the "outgoing edge" (the one that takes to the root)
        // sanity check: the outgoing edge should condensate all the loops coming into the umbrella
        uint sum = 0;
        for(uint i=0; i<edges_on_loop.size()-1; ++i) sum += edges_on_loop.at(i).first;
        assert(sum == edges_on_loop.back().first);

        if(edges_on_loop.size()>2)
        {
            uint v_in  = edges_on_loop.front().second;
            uint v_out = edges_on_loop.back().second;
            uint v_new = m.vert_split(m.edge_id(vid,v_in), m.edge_id(vid,v_out));

            for(uint eid : m.adj_v2e( vid )) m.edge_data(eid).label = 0;
            for(uint eid : m.adj_v2e(v_new)) m.edge_data(eid).label = 0;

            // connect the ones for which there is only one way possible first
            std::vector<ipair> to_redo;
            uint count[2] = {0,0};
            for(uint i=0; i<edges_on_loop.size()-1; ++i)
            {
                uint v_in = edges_on_loop.at(i).second;
                uint val  = edges_on_loop.at(i).first;
                int  e0   = m.edge_id(v_in, v_new);
                int  e1   = m.edge_id(v_in, vid);
                if(e0==-1 && e1>=0)
                {
                    count[0]++;
                    extend_loop_through_umbrella(m, v_in, vid, v_out, val);
                }
                else if(e0>=0  && e1==-1)
                {
                    count[1]++;
                    extend_loop_through_umbrella(m, v_in, v_new, v_out, val);
                }
                else
                {
                    to_redo.push_back(std::make_pair(v_in,val));
                }
            }
            // then connect the others (in order to chose the right path within the umbrella)
            for(auto v : to_redo)
            {
                uint v_mid = (count[0] < count[1]) ? vid : v_new;
                extend_loop_through_umbrella(m, v.first, v_mid, v_out, v.second);
            }

            if(v_out!=root) q.push(v_out);
            bool add_vid  = false;
            bool add_vnew = false;
            for(uint eid : m.adj_v2e( vid )) if(m.edge_data(eid).label>1) add_vid  = true;
            for(uint eid : m.adj_v2e(v_new)) if(m.edge_data(eid).label>1) add_vnew = true;
            if(add_vid)  q.push(vid);
            if(add_vnew) q.push(v_new);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void refine_mesh_to_detach_loops(Trimesh<M,V,E,P> & m, const uint root, std::vector<std::vector<uint>> & basis)
{
    // PRE-PROCESSING:
    // mark edges on basis loops, and count loops per edge (using edge labels)
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        m.edge_data(eid).label = 0;
    }
    for(auto loop : basis)
    {
        for(uint i=0; i<loop.size(); ++i)
        {
            uint v0  = loop.at(i);
            uint v1  = loop.at((i+1)%loop.size());
             int eid = m.edge_id(v0, v1);
            m.edge_data(eid).label++;
        }
    }

    refine_mesh_to_detach_loops(m, root);

    // POST-PROCESSING: update basis
    basis.clear();
    m.edge_unmark_all();
    for(uint eid: m.adj_v2e(root))
    {
        if(m.edge_data(eid).marked) continue;
        if(m.edge_data(eid).label>0)
        {
            std::vector<uint> loop;
            loop.push_back(root);
            loop.push_back(m.vert_opposite_to(eid, root));
            m.edge_data(eid).marked = true;

            uint curr = loop.back();
            do
            {
                int next = -1;
                for(uint eid : m.adj_v2e(curr))
                {
                    if(m.edge_data(eid).label>0 && !m.edge_data(eid).marked)
                    {
                        assert(next==-1);
                        next = m.vert_opposite_to(eid, curr);
                        m.edge_data(eid).marked = true;
                    }
                }
                assert(next>=0);
                if((uint)next != root) loop.push_back(next);
                curr = next;
            }
            while(curr != root);
            basis.push_back(loop);
        }
    }
    assert((int)basis.size() == m.genus()*2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis(Trimesh<M,V,E,P>               & m,
                    const uint                       root,
                    std::vector<std::vector<uint>> & basis,
                    const bool                       detach_loops)
{
    std::vector<bool> tree, cotree;
    homotopy_basis(m, root, basis, tree, cotree);

    // refine the mesh so that each edge participates in at most a single loop.
    // This is achieved via a sequence of vertex_split operations. An alternative
    // approach, based on edge_splits, is described in:
    //
    // Globally Optimal Surface Mapping for Surfaces with Arbitrary Topology
    // Xin Li, Yunfan Bao, Xiaohu Guo, Miao Jin, Xianfeng Gu and Hong Qin
    // IEEE Transactions on Visualization and Computer Graphics, 2008
    //
    if(detach_loops) refine_mesh_to_detach_loops(m, root, basis);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis(AbstractPolygonMesh<M,V,E,P>   & m,
                    const uint                      root,
                    std::vector<std::vector<uint>> & basis,
                    std::vector<bool>              & tree,
                    std::vector<bool>              & cotree)
{
    assert(root<m.num_verts());

    basis.clear();
    tree   = std::vector<bool>(m.num_edges(), false);
    cotree = std::vector<bool>(m.num_edges(), false);

    m.vert_unmark_all();
    m.poly_unmark_all();

    std::queue<uint> q;
    q.push(root);
    m.vert_data(root).marked = true;

    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        for(uint nbr : m.adj_v2v(vid))
        {
            if(!m.vert_data(nbr).marked)
            {
                m.vert_data(nbr).marked = true;
                q.push(nbr);
                int eid = m.edge_id(vid, nbr);
                assert(eid>=0);
                tree.at(eid) = true;
            }
        }
    }

    assert(q.empty());
    uint pid = m.adj_v2p(root).front();
    q.push(pid);
    m.poly_data(pid).marked = true;

    while(!q.empty())
    {
        uint pid = q.front();
        q.pop();

        for(uint nbr : m.adj_p2p(pid))
        {
            if(!m.poly_data(nbr).marked)
            {
                int eid = m.edge_shared(pid, nbr);
                assert(eid>=0);
                if(!tree.at(eid))
                {
                    q.push(nbr);
                    m.poly_data(nbr).marked = true;
                    cotree.at(eid) = true;
                }
            }
        }
    }

    // find edges neither in tree, nor in cotree
    std::vector<uint> generators;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(!tree.at(eid) && !cotree.at(eid)) generators.push_back(eid);
    }
    assert(m.genus()*2 == (int)generators.size());

    // to restrict Dijkstra to the edges in tree only
    std::vector<bool> edge_mask(m.num_edges());
    for(uint eid=0; eid<m.num_edges(); ++eid) edge_mask.at(eid) = !tree.at(eid);

    // start from each edge in loop generator and close a loop with its two endpoints
    for(uint eid : generators)
    {
        std::vector<uint> e0_to_root, e1_to_root;
        dijkstra_mask_on_edges(m, m.edge_vert_id(eid,0), root, edge_mask, e0_to_root);
        dijkstra_mask_on_edges(m, m.edge_vert_id(eid,1), root, edge_mask, e1_to_root);
        e1_to_root.pop_back();
        std::reverse(e1_to_root.begin(), e1_to_root.end());
        std::copy(e1_to_root.begin(), e1_to_root.end(), std::back_inserter(e0_to_root));
        basis.push_back(e0_to_root);
    }
}

}
