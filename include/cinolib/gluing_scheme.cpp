/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
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
#include <cinolib/gluing_scheme.h>
#include <cinolib/dijkstra.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void gluing_scheme_force_normal_form(Trimesh<M,V,E,P>  & m,
                                     HomotopyBasisData & data)
{
    int fresh_loop_id = m.genus()*2;

    //**********************************************//
    //************** HELPER FUNCTIONS **************//
    //**********************************************//

    // split mesh edges connecting adjacent loops
    auto pad_loops = [&]() -> void
    {
        std::vector<uint> edges_to_split;
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            uint v0 = m.edge_vert_id(eid,0);
            uint v1 = m.edge_vert_id(eid,1);
            if(!m.edge_data(eid).flags[MARKED])
            {
                int l0 = m.vert_data(v0).label;
                int l1 = m.vert_data(v1).label;
                if((l0>=0 && l1>=0)         ||
                   (l0>=0 && v1==data.root) ||
                   (l1>=0 && v0==data.root))
                {
                    edges_to_split.push_back(v0);
                    edges_to_split.push_back(v1);
                }
            }
        }
        for(uint i=0; i<edges_to_split.size(); i+=2)
        {
            int eid = m.edge_id(edges_to_split.at(i),edges_to_split.at(i+1));
            assert(eid>=0);
            m.edge_split(eid);
        }
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // remove a previosuly existing loop (VIRTUAL STITCH)
    auto remove_loop = [&](const uint loop_id)
    {
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            if(m.edge_data(eid).flags[MARKED] && m.edge_data(eid).label==loop_id)
            {
                m.edge_data(eid).flags[MARKED] = false;
                m.edge_data(eid).label = -1;
                uint v0 = m.edge_vert_id(eid,0);
                uint v1 = m.edge_vert_id(eid,1);
                m.vert_data(v0 ).flags[MARKED] = false;
                m.vert_data(v1 ).flags[MARKED] = false;
                m.vert_data(v0 ).label = -1;
                m.vert_data(v1 ).label = -1;
            }
        }
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // trace a new loop (VIRTUAL CUT)
    auto trace_loop = [&](const uint beg, const uint end) -> bool
    {
        std::vector<bool> mask(m.num_verts());
        for(uint vid=0; vid<m.num_verts(); ++vid) mask.at(vid) = (m.vert_data(vid).label>=0);
        mask.at(data.root) = true;

        std::vector<uint> path;
        dijkstra(m, beg, end, mask, path);
        if(path.empty()) std::runtime_error("Tracing failed");
        path.push_back(data.root);
        uint n = path.size();
        for(uint i=0; i<n; ++i)
        {
            uint v0 = path[i];
            uint v1 = path[(i+1)%n];
            int eid = m.edge_id(v0,v1);
            assert(eid>=0);
            m.edge_data(eid).flags[MARKED] = true;
            m.vert_data(v0 ).flags[MARKED] = true;
            m.vert_data(v1 ).flags[MARKED] = true;
            m.edge_data(eid).label = fresh_loop_id;
            m.vert_data(v0 ).label = fresh_loop_id;
            m.vert_data(v1 ).label = fresh_loop_id;
        }
        m.vert_data(data.root).flags[MARKED] = false;
        m.vert_data(data.root).label = -1;
        ++fresh_loop_id;
        return true;
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // updates the loop id for a given loop
    auto rename_loop = [&](const uint old_id, const uint new_id)
    {
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            if(m.vert_data(vid).label==old_id) m.vert_data(vid).label = new_id;
        }
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            if(m.edge_data(eid).label==old_id) m.edge_data(eid).label = new_id;
        }
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // puts loops alpha and beta in normal form
    auto Fulton_step = [&](const uint alpha, const uint beta)
    {
        pad_loops();
        auto gs = gluing_scheme(m,data,true);
        uint n  = gs.size();

        auto next_occurrence_of = [&](const uint loop, const uint index) -> int
        {
            for(uint i=index,check=0; ; i=(i+1)%n)
            {
                if(i%2==1 && gs[i]==loop) return i;
                if(++check>n) std::runtime_error("Infinite loop");
            }
            assert(false);
            return 0;
        };

        uint b0  = next_occurrence_of(beta,0);
        uint b1  = next_occurrence_of(beta,(b0+1)%n);
        uint beg = (b0+1)%n;
        uint end = (b1+n-1)%n;

        trace_loop(gs[beg],gs[end]);
        remove_loop(alpha);
        rename_loop(fresh_loop_id-1,alpha);

        pad_loops();
        gs = gluing_scheme(m,data,true);
        n  = gs.size();

        uint g0 = (next_occurrence_of(beta,0)+2)%n;
        uint g1 = (next_occurrence_of(alpha,(g0+1)%n))%n;
        beg = (g0+1)%n;
        end = (g1+n-1)%n;

        trace_loop(gs[beg],gs[end]);
        remove_loop(beta);
        rename_loop(fresh_loop_id-1,beta);
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    auto find_and_normalize_pair = [&]()
    {
        auto gs = gluing_scheme(m,data,false);
        uint n  = gs.size();

        auto dist_from_next = [&](const uint val, const uint pos) -> int
        {
            uint dist = 1;
            for(uint i=(pos+1)%n,check=0; gs[i]!=val; i=(i+1)%n)
            {
                ++dist;
                if(++check>n) std::runtime_error("Infinite loop");
            }
            return dist;
        };

        for(uint i=0; i<gs.size(); ++i)
        {
            if(!is_normal_form(gs,i))
            {
                for(uint j=(i+1)%n,count=0; count<n; j=(j+1)%n,++count)
                {
                    int d_next_i  = dist_from_next(gs[i],j);
                    int d_next_j  = dist_from_next(gs[j],j);
                    int d_next_ii = dist_from_next(gs[i],j+d_next_i) + d_next_i;
                    if(d_next_i<d_next_j && d_next_j<d_next_ii && !is_normal_form(gs,j))
                    {
                        Fulton_step(gs[i],gs[j]);
                        return true;
                    }
                }
            }
        }
        return false;
    };

    //**********************************************//
    //************** ACTUAL ALGORITHM **************//
    //**********************************************//

    while(find_and_normalize_pair()){}
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> gluing_scheme(Trimesh<M,V,E,P>  & m,
                               HomotopyBasisData & data,
                               const bool full)
{
    std::vector<int> rs = radial_sorting(m,data,full);
    int n    = rs.size();
    int step = (full)?2:1; // next element in vector (because the full version interleaves loop and vertex ids)

    auto next = [&](const int curr) -> int
    {
        for(int i=(curr+step)%n,check=0; ; i=(i+step)%n)
        {
            if(rs.at(i)==rs.at(curr))
            {
                for(int j=(i+step)%n,check=0; ; j=(j+step)%n)
                {
                    if(rs.at(j)>=0) return j;
                    if(check>n) std::runtime_error("Infinite loop");
                }
            }
            if(++check>n) std::runtime_error("Infinite loop");
        }
        assert(false);
        return 0;
    };

    int i=0;
    while(rs.at(i)<0) i+=step;
    assert(rs.at(i)>=0);
    std::vector<int> gs_indices = { i };
    gs_indices.reserve(m.genus()*4);
    do
    {
        gs_indices.push_back(next(gs_indices.back()));
    }
    while(gs_indices.size()<m.genus()*4);
    //
    std::vector<int> gs;
    for(int index : gs_indices)
    {
        if(full) gs.push_back(rs.at((index+n-1)%n));
        gs.push_back(rs.at(index));
    }
    return gs;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> radial_sorting(Trimesh<M,V,E,P>  & m,
                                HomotopyBasisData & data,
                                const bool full)
{
    std::vector<int> rs;
    for(uint eid : m.vert_ordered_edges_star(data.root))
    {
        if(full || m.edge_data(eid).flags[MARKED])
        {
            uint vid = m.vert_opposite_to(eid,data.root);
            rs.push_back(m.vert_data(vid).label);
            if(full) rs.push_back(vid);
        }
    }
    return rs;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool is_normal_form(const std::vector<int> & gs, const uint pos)
{
    uint n = gs.size();
    uint a  = gs[pos];
    uint n1 = gs[(pos  +1)%n];
    uint n2 = gs[(pos  +2)%n];
    uint n3 = gs[(pos  +3)%n];
    uint p1 = gs[(pos+n-1)%n];
    uint p2 = gs[(pos+n-2)%n];
    uint p3 = gs[(pos+n-3)%n];

    if(a==n2 && n1==n3) return true;
    if(a==n2 && p1==n1) return true;
    if(a==p2 && p1==n1) return true;
    if(a==p2 && p1==p3) return true;
    return false;
}

}
