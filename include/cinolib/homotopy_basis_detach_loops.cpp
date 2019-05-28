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
#include <cinolib/homotopy_basis_detach_loops.h>

namespace cinolib
{


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis_detach_loops_preproc(Trimesh<M,V,E,P>               & m,
                                         std::vector<std::vector<uint>> & basis)
{
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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis_detach_loops_postproc(Trimesh<M,V,E,P>               & m,
                                          const uint                       root,
                                          std::vector<std::vector<uint>> & basis)
{
    // recompute basis
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
void homotopy_basis_detach_loops_by_vert_split(Trimesh<M,V,E,P>               & m,
                                               const uint                       root,
                                               std::vector<std::vector<uint>> & basis)
{
    homotopy_basis_detach_loops_preproc(m, basis);

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

    homotopy_basis_detach_loops_postproc(m, root, basis);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int refine_umbrella(Trimesh<M,V,E,P> & m, const uint vid)
{
    auto  e_star    = m.vert_ordered_edges_star(vid); assert(!e_star.empty());
    uint  e_out     = e_star.front(); // will contain the ID of the edge pointing towards the root of the homotopy basis
     int  val_check = 0;
    uint count      = 0;
    for(uint eid : e_star)
    {
        int val = m.edge_data(eid).label;
        if(m.edge_data(e_out).label < val) e_out = eid;
        val_check += val;
        if(val>0) ++count;
    }
    if(count<3) return -1;
    assert(val_check == 2*m.edge_data(e_out).label);
    CIRCULAR_SHIFT_VEC(e_star, e_out);
    assert(e_star.front() == e_out);

    //std::cout << "handle vert: " << vid << std::endl;
    //for(uint eid : e_star) std::cout << eid << "\t" << m.edge_data(eid).label << std::endl;

    // due to mesh refinement, it's safer to store edges as pairs of vertex ids and retrieve the ID each time is needed
    std::vector<ipair> e_star_as_vpairs;
    for(uint eid : e_star) e_star_as_vpairs.push_back(m.edge_vert_ids(eid));

    // rotate around the vertex until an incoming edge participating in a basis loop is found
    std::vector<ipair> split_list;
    int val;
    int e_in;
    for(uint i=1; i<e_star.size(); ++i)
    {
        e_in = e_star.at(i);
        val  = m.edge_data(e_in).label;
        if(val==0) split_list.push_back(e_star_as_vpairs.at(i));
        else break;
    }
    assert(val>0);
    assert(m.edge_data(e_in).label==val);
    assert(split_list.size()<e_star.size()-1);
    int v_in = m.vert_opposite_to(e_in, vid);
    assert(v_in>=0);

    // detach the path, iteratively splitting all edges in between e_in and e_out
    uint v_out  = m.vert_opposite_to(e_out, vid);
    uint v_prev = v_out;
    if(!split_list.empty())
    {
        for(ipair & e : split_list)
        {
             int eid   = m.edge_id(e);
            assert(eid>=0);
            assert(m.edge_data(eid).label==0);
            uint v_new = m.edge_split(eid);
            for(uint e : m.adj_v2e(v_new)) m.edge_data(e).label = 0;
            int e_new = m.edge_id(v_prev, v_new);
            m.edge_data(e_new).label = val;
            v_prev = v_new;
            assert((int)e_out == m.edge_id(vid, v_out)); // after the edge splits can the original e_out become inconsistent?
        }
        // close the path
        int e_last = m.edge_id(v_prev, v_in);
        assert(m.edge_data(e_last).label==0);
        assert(m.edge_data(e_in  ).label==val);
        assert(m.edge_data(e_out ).label >val);
        m.edge_data(e_last).label = val;
        m.edge_data(e_in  ).label-= val; // reduce valence of previous path
        m.edge_data(e_out ).label-= val; // reduce valence of outgoing edge
    }
    else // e_in and e_out share the same face. To detach them I just split the face
    {
        int pid    = m.poly_shared(e_out, e_in);
        uint v_new = m.poly_split(pid);
        for(uint e : m.adj_v2e(v_new)) m.edge_data(e).label = 0;
        int e0     = m.edge_id(v_new, v_out);
        int e1     = m.edge_id(v_new, v_in);
        assert(m.edge_data(e_in  ).label==val);
        assert(m.edge_data(e_out ).label >val);
        m.edge_data(e0).label     = val;
        m.edge_data(e1).label     = val;
        m.edge_data(e_in).label  -= val; // reduce valence of previous path
        m.edge_data(e_out).label -= val; // reduce valence of outgoing edge
    }

    count = 0;
    for(uint eid : m.adj_v2e(vid)) if(m.edge_data(eid).label>0) ++count;
    if(count>2) return vid;
    return v_out;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis_detach_loops_by_edge_split(Trimesh<M,V,E,P>               & m,
                                               const uint                       root,
                                               std::vector<std::vector<uint>> & basis)
{
    homotopy_basis_detach_loops_preproc(m, basis);

    // enque vertices with more than 2 incident loop edges
    std::queue<uint> q;
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        if(vid == root) continue;
        uint count = 0;
        for(uint eid : m.adj_v2e(vid)) if(m.edge_data(eid).label>0) ++count;
        if(count>2) q.push(vid);
    }

    // PROCESSING: mesh refinement
    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        int next = refine_umbrella(m, vid);
        if(next>=0 && next!=(int)root) q.push(next);
    }

    homotopy_basis_detach_loops_postproc(m, root, basis);
}

}
