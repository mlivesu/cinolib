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
#include <cinolib/shortest_path_tree.h>
#include <cinolib/mst.h>
#include <cinolib/stl_container_utilities.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const HomotopyBasisData & data)
{
    in << ":::::::::::::::::::: HOMOTOPY BASIS INFO ::::::::::::::::;::\n";
    in << "Root              : " << data.root                      << "\n";
    in << "Globally shortest : " << data.globally_shortest         << "\n";
    in << "Length            : " << data.length                    << "\n";
    in << "Detach basis loops: " << data.detach_loops              << "\n";
    if(data.detach_loops)
    {
        in << "Split Strategy    : " << ref_txt[data.split_strategy]   << "\n";
        in << "Coplanarity Thresh: " << data.coplanarity_thresh    << " deg\n";
        in << data.refinement_stats;
    }
    in << "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const RefinementStats & stats)
{
    in << "::::::::::::::::::: REFINEMENT STATISTICS ::::::::::::::::::\n";
    in << "Total splits: " << stats.splits_tot << "\n";
    in << "Vert  splits: " << stats.splits_vert << " (" << 100.0*stats.splits_vert/stats.splits_tot << "%)\n";
    in << "Edge  splits: " << stats.splits_edge << " (" << 100.0*stats.splits_edge/stats.splits_tot << "%)\n";
    in << "Poly  splits: " << stats.splits_poly << " (" << 100.0*stats.splits_poly/stats.splits_tot << "%)\n";
    in << "Vert valence: " << stats.vert_val_max << "(max), " << stats.vert_val_avg/stats.splits_tot << "(avg)\n";
    in << "Vert   count: " << stats.num_verts_now  << " (" << 100.0*(stats.num_verts_now-stats.num_verts_bef)/stats.num_verts_bef << "% more than in the input mesh)\n";
    in << "Poly   count: " << stats.num_polys_now  << " (" << 100.0*(stats.num_polys_now-stats.num_polys_bef)/stats.num_polys_bef << "% more than in the input mesh)\n";
    in << "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n";
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double homotopy_basis(AbstractPolygonMesh<M,V,E,P>   & m,
                      const uint                       root,
                      std::vector<std::vector<uint>> & basis,
                      std::vector<bool>              & tree,
                      std::vector<bool>              & cotree)
{
    assert(root<m.num_verts());

    shortest_path_tree(m, root, tree);

    // Compute the cotree as the Maximum Spanning Tree of the dual of M,
    // without considering dual edges that cross edges of primal tree.
    //
    // I'm using a classical Minimum Spanning Tree algorithm (Prim's) with negative weights
    std::vector<float> edge_weights(m.num_edges(),0);
    std::vector<bool>  edge_mask(m.num_edges()); // restrict Dijkstra to the edges in tree only
    for(uint eid=0; eid<m.num_edges(); ++eid) edge_mask.at(eid) = !tree.at(eid);
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(tree.at(eid)) continue;
        std::vector<uint> tmp;
        edge_weights.at(eid) -= m.edge_length(eid);
        edge_weights.at(eid) -= dijkstra_mask_on_edges(m, m.edge_vert_id(eid,0), root, edge_mask, tmp);
        edge_weights.at(eid) -= dijkstra_mask_on_edges(m, m.edge_vert_id(eid,1), root, edge_mask, tmp);
    }
    MST_on_dual_mask_on_edges(m, edge_weights, tree, cotree); // use tree as edge mask

    // Find the edges neither in tree, nor in cotree
    std::vector<uint> generators;
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        if(!tree.at(eid) && !cotree.at(eid)) generators.push_back(eid);
    }
    assert(m.genus()*2 == (int)generators.size());

    // Start from each such edge, and close a loop with its two endpoints
    basis.clear();
    double length = 0.0;
    for(uint eid : generators)
    {
        std::vector<uint> e0_to_root, e1_to_root;
        length += m.edge_length(eid);
        length += dijkstra_mask_on_edges(m, m.edge_vert_id(eid,0), root, edge_mask, e0_to_root);
        length += dijkstra_mask_on_edges(m, m.edge_vert_id(eid,1), root, edge_mask, e1_to_root);
        e1_to_root.pop_back();
        std::reverse(e1_to_root.begin(), e1_to_root.end());
        std::copy(e1_to_root.begin(), e1_to_root.end(), std::back_inserter(e0_to_root));
        basis.push_back(e0_to_root);
    }
    return length;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis(AbstractPolygonMesh<M,V,E,P> & m,
                    HomotopyBasisData            & data)
{
    // BASIS COMPUTATION: either run tree-cotree once on a given root in O(n log n), or try
    // computing a homotopy basis for each mesh vertex, findng the shortest in O(n^2 log n)
    //
    if(data.globally_shortest)
    {
        double best_length = inf_double;
        uint   best_root;
        std::vector<std::vector<uint>> best_basis;
        std::vector<bool>              best_tree;
        std::vector<bool>              best_cotree;

        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            double length = homotopy_basis(m, vid, data.loops, data.tree, data.cotree);

            if(length < best_length)
            {
                best_root   = vid;
                best_length = length;
                best_basis  = data.loops;
                best_tree   = data.tree;
                best_cotree = data.cotree;
            }
        }
        data.loops  = best_basis;
        data.root   = best_root;
        data.length = best_length;
        data.tree   = best_tree;
        data.cotree = best_cotree;
    }
    else
    {
        data.length = homotopy_basis(m, data.root, data.loops, data.tree, data.cotree);
    }

    if(data.detach_loops) detach_loops(dynamic_cast<Trimesh<M,V,E,P>&>(m), data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// globally detaches loops in the homotopy basis
template<class M, class V, class E, class P>
CINO_INLINE
void detach_loops(Trimesh<M,V,E,P>  & m,
                  HomotopyBasisData & data)
{
    detach_loops_preproc(m, data);

    data.refinement_stats.num_verts_bef = m.num_verts();
    data.refinement_stats.num_polys_bef = m.num_polys();

    std::queue<uint> q;
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        if(vid == data.root) continue;
        uint val_1 = 0;
        uint val_2 = 0; // 2 or more
        for(uint eid : m.adj_v2e(vid))
        {
            switch(m.edge_data(eid).label)
            {
                case -1: assert(false); break;
                case  0:                break;
                case  1: ++val_1;       break;
                default: ++val_2;       break;
            }
        }
        if(val_1>=2 && val_2==1) q.push(vid);
    }

    m.vert_unmark_all();
    m.vert_data(data.root).flags[MARKED] = true;
    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        m.vert_data(vid).flags[MARKED] = false;
        uint next = detach_loops(m, data, vid);

        if(!m.vert_data(next).flags[MARKED])
        {
            q.push(next);
            m.vert_data(next).flags[MARKED] = true;
        }
    }

    data.refinement_stats.num_verts_now = m.num_verts();
    data.refinement_stats.num_polys_now = m.num_polys();

    detach_loops_postproc(m, data);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// locally detaches loops in the homotopy basis around vertex vid
template<class M, class V, class E, class P>
CINO_INLINE
uint detach_loops(Trimesh<M,V,E,P>  & m,
                  HomotopyBasisData & data,
                  const uint          vid)
{
    // compute star ring (with the edge pointing to the root of the basis in first position)
    auto e_star = m.vert_ordered_edges_star(vid);
    std::vector<std::pair<int,uint>> loop_edges; // { #loops , eid }
    for(uint eid : e_star)
    {
        int n_loops = m.edge_data(eid).label;
        if(n_loops>0) loop_edges.push_back(std::make_pair(n_loops,eid));
    }
    assert(loop_edges.size()>=3);

    auto it = std::max_element(loop_edges.begin(), loop_edges.end());
    CIRCULAR_SHIFT_VEC(e_star, it->second);

    // rotate CCW to find the triangle strips that surround the edge pointing to the root
    std::vector<uint> edges_ccw;
    auto i = e_star.begin();
    auto j = i+1;
    edges_ccw.push_back(*i);
    for(; j!=e_star.end(); ++j)
    {
        edges_ccw.push_back(*j);
        if(m.edge_data(*j).flags[MARKED]) break;
    }
    assert(j!=e_star.end());

    std::vector<uint> edges_cw;
    auto k = e_star.rbegin();
    edges_cw.push_back(*i);
    for(; k!=e_star.rend(); ++k)
    {
        edges_cw.push_back(*k);
        if(m.edge_data(*k).flags[MARKED]) break;
    }
    assert(k!=e_star.rend());

    switch(data.split_strategy)
    {
        case EDGE_SPLIT_STRATEGY  :
        {
            // always chose the size with the smallest triangle fan
            if(edges_ccw.size()>edges_cw.size()) return detach_loops_by_edge_split(m, data, edges_cw);
            else                                 return detach_loops_by_edge_split(m, data, edges_ccw);
        }

        case VERT_SPLIT_STRATEGY  :
        {
            vec3d new_pos;
            if(find_position_within_fan(m, edges_ccw, vid, new_pos)) return detach_loops_by_vert_split(m, data, *j, *i, new_pos);
            if(find_position_within_fan(m, edges_cw,  vid, new_pos)) return detach_loops_by_vert_split(m, data, *k, *i, new_pos);
            return detach_loops_by_vert_split(m, data, *j, *i); // this uses cinolib's default vert relocation for the vertex split operator
        }

        case HYBRID_SPLIT_STRATEGY :
        {
            // try triangle split first (though these should never be applied in a locally shortest basis...)
            if(edges_cw.size()==2)  return detach_loops_by_poly_split(m, data, *k, *i);
            if(edges_ccw.size()==2) return detach_loops_by_poly_split(m, data, *j, *i);

            // otherwise try vert split
            vec3d new_pos;
            if(polys_are_planar(m, edges_cw, data.coplanarity_thresh) && find_position_within_fan(m, edges_cw, vid, new_pos))
            {
                return detach_loops_by_vert_split(m, data, *k, *i, new_pos);
            }
            if(polys_are_planar(m, edges_ccw, data.coplanarity_thresh) && find_position_within_fan(m, edges_ccw, vid, new_pos))
            {
                return detach_loops_by_vert_split(m, data, *j, *i, new_pos);
            }

            // at the very least, do edge split along the smallest triangle fan
            if(edges_ccw.size()>edges_cw.size())  return detach_loops_by_edge_split(m, data, edges_cw);
            return detach_loops_by_edge_split(m, data, edges_ccw);
        }

        default : assert(false && "unknown split strategy");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// locally detaches loops in the homotopy basis around vertex vid using a vertex split
template<class M, class V, class E, class P>
CINO_INLINE
uint detach_loops_by_vert_split(Trimesh<M,V,E,P>  & m,
                                HomotopyBasisData & data,
                                const uint          e_in,
                                const uint          e_out,
                                const vec3d         new_pos)
{
    ++data.refinement_stats.splits_tot;
    ++data.refinement_stats.splits_vert;

    uint v_mid  = m.vert_shared(e_out, e_in);
    uint v_in   = m.vert_opposite_to(e_in, v_mid);
    uint v_out  = m.vert_opposite_to(e_out, v_mid);
    int  val_in = m.edge_data(e_in).label;

    uint star_size = m.vert_valence(v_mid);
    data.refinement_stats.vert_val_max  = std::max(data.refinement_stats.vert_val_max, star_size);
    data.refinement_stats.vert_val_avg += star_size;

    // find other edges traversed by some loop
    std::vector<std::pair<uint,int>> others;
    for(uint eid : m.adj_v2e(v_mid))
    {
        if(eid==e_in || eid==e_out) continue;
        int val = m.edge_data(eid).label;
        if(val>0) others.push_back(std::make_pair(m.vert_opposite_to(eid,v_mid),val));
    }

    vec3d og_pos = m.vert(v_mid);
    uint  v_new  = m.vert_split(e_in, e_out);
    m.vert(v_mid) = og_pos;
    m.vert(v_new) = new_pos;

    // reset loop topology inside the refined umbrella
    for(uint eid : m.adj_v2e(v_mid)) m.edge_data(eid).label = 0;
    for(uint eid : m.adj_v2e(v_new)) m.edge_data(eid).label = 0;

    int e_in_new  = m.edge_id(v_in,  v_new);
    int e_out_new = m.edge_id(v_out, v_new);
    assert(e_in_new>=0);
    assert(e_out_new>=0);

    // restore loop info on other edges
    for(auto obj : others)
    {
        uint vid   = obj.first;
        int  val   = obj.second;
        int  e_old = m.edge_id(vid, v_mid);
        int  e_new = m.edge_id(vid, v_new);

        if(e_old>=0)
        {
            assert(e_new==-1);
            m.edge_data(e_old).label  = val;
            m.edge_data(e_out).label += val;
        }
        else
        {
            assert(e_new>=0);
            assert(e_old==-1);
            m.edge_data(e_new).label  = val;
            m.edge_data(e_out_new).label += val;
        }
    }

    // restore loop info on { e_in , e_out }
    if(m.edge_data(e_out_new).label==0)
    {
        m.edge_data(e_in_new).label  = val_in;
        m.edge_data(e_out_new).label = val_in;
    }
    else
    {
        // the method will work even if the assertion fails.
        // The only difference is that if both v_mid and v_new
        // are merging points, I should iterate on both and not
        // only one of them, as I am doing right now. The guarantee
        // that only one of the two is still a merging point comes
        // from the fact that e_in and e_out were chosen rotating
        // around v_mid starting from e_out, therefore at least on
        // side of { e_in, e_out } there can't be another edge traversed
        // by a loop
        assert(m.edge_data(e_out).label==0);
        m.edge_data(e_in).label  = val_in;
        m.edge_data(e_out).label = val_in;

        // v_mid is at the same side of the triangle strip. switch vertex positions...
        std::swap(m.vert(v_mid),m.vert(v_new));
    }

    // update normals
    for(uint pid : m.adj_v2p(v_mid)) m.update_p_normal(pid);
    for(uint pid : m.adj_v2p(v_new)) m.update_p_normal(pid);
    m.update_v_normal(v_mid);
    m.update_v_normal(v_new);

    // update flags
    for(uint eid : m.adj_v2e(v_mid)) m.edge_data(eid).flags[MARKED] = m.edge_data(eid).label>0;
    for(uint eid : m.adj_v2e(v_new)) m.edge_data(eid).flags[MARKED] = m.edge_data(eid).label>0;

    // next one ring to process...
    uint count = 0;
    for(uint eid : m.adj_v2e(v_mid)) if(m.edge_data(eid).label>0) ++count;
    if(count>2) return v_mid;
    count = 0;
    for(uint eid : m.adj_v2e(v_new)) if(m.edge_data(eid).label>0) ++count;
    if(count>2) return v_new;
    return v_out;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// locally detaches loops in the homotopy basis around vertex vid using a sequence of edge splits
template<class M, class V, class E, class P>
CINO_INLINE
uint detach_loops_by_edge_split(Trimesh<M,V,E,P>        & m,
                                HomotopyBasisData       & data,
                                const std::vector<uint> & edge_fan)
{
    ++data.refinement_stats.splits_tot;
    ++data.refinement_stats.splits_edge;

    assert(edge_fan.size()>2 || "there's detach_loops_by_poly_split() otherwise");

    uint e_in    = edge_fan.back();
    uint e_out   = edge_fan.front();
    uint v_mid   = m.vert_shared(e_in, e_out);
    uint v_in    = m.vert_opposite_to(e_in, v_mid);
    uint v_out   = m.vert_opposite_to(e_out, v_mid);
    int  val_in  = m.edge_data(e_in).label;
    int  val_out = m.edge_data(e_out).label;
    assert(val_in>0);
    assert(val_out > val_in);

    uint star_size = m.vert_valence(v_mid);
    data.refinement_stats.vert_val_max  = std::max(data.refinement_stats.vert_val_max, star_size);
    data.refinement_stats.vert_val_avg += star_size;

    std::vector<uint> new_chain = { v_out };
    for(uint i=1; i<edge_fan.size()-1; ++i)
    {
        uint eid = edge_fan.at(i);
        assert(m.edge_data(eid).label==0);
        // optimally place newly inserted vertex according to the valence balance between the two disjoint paths
        vec3d A = m.vert(v_mid);
        vec3d B = m.vert(m.vert_opposite_to(eid, v_mid));
        float t = static_cast<float>(val_out - val_in)/static_cast<float>(val_out);
        vec3d C = A*(1.f-t) + B*t;
        assert(t>0.f && t<1.f);
        //std::cout << t << std::endl;
        uint v_new = m.edge_split(eid, C);
        new_chain.push_back(v_new);
        for(uint nbr : m.adj_v2e(v_new)) m.edge_data(nbr).label = 0;
    }
    new_chain.push_back(v_in);

    for(auto i=new_chain.begin(),j=i+1; j<new_chain.end(); ++i,++j)
    {
        int eid = m.edge_id(*i,*j);
        assert(eid>=0);
        assert(m.edge_data(eid).label==0);
        m.edge_data(eid).label=val_in;
    }

    m.edge_data(e_in).label   = 0;
    m.edge_data(e_out).label -= val_in;
    assert(m.edge_data(e_out).label>0);

    // update flags
    for(uint eid : m.adj_v2e(v_mid)) m.edge_data(eid).flags[MARKED] = m.edge_data(eid).label>0;
    for(uint v_new : new_chain) for(uint eid : m.adj_v2e(v_new)) m.edge_data(eid).flags[MARKED] = m.edge_data(eid).label>0;

    // next one ring to process...
    uint count = 0;
    for(uint eid : m.adj_v2e(v_mid)) if(m.edge_data(eid).label>0) ++count;
    if(count>2) return v_mid;
    return v_out;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// locally detaches loops in the homotopy basis around vertex vid using a triangle split
template<class M, class V, class E, class P>
CINO_INLINE
uint detach_loops_by_poly_split(Trimesh<M,V,E,P>  & /*m*/,
                                HomotopyBasisData & /*data*/,
                                const uint          /*e_in*/,
                                const uint          /*e_out*/)
{
    assert(false && "WARNING: this homotopy basis must not be locally shortest!");
    return 0; // warning killer

    // In a locally shortest homotopy basis it is impossible that there
    // exists only one triangle in between in and out edges. If the basis
    // is shortest, the triangular inequality;
    //
    //        |v_in - v_mid| + |v_mid - v_out| >= | v_in - v_out|
    //
    // should always hold. Hence the loops should have traverse the perimeter
    // of the umbrella, never entering into it.

    // For this reason THE CODE BELOW WAS NEVER TESTED!!!!
    // I am keeping just in case there will ever be a non
    // locally shortest basis that needs it....

    //++data.refinement_stats.splits_tot;
    //++data.refinement_stats.splits_poly;

    //uint v_mid     = m.vert_shared(e_out, e_in);
    //uint v_in      = m.vert_opposite_to(e_in, v_mid);
    //uint v_out     = m.vert_opposite_to(e_out, v_mid);
    // int pid       = m.poly_id(e_in, e_out);     assert(pid>=0);
    // int val_in    = m.edge_data(e_in).label;    assert(val_in>0);
    // int val_out   = m.edge_data(e_out).label;   assert(val_out>val_in);
    //uint v_new     = m.poly_split(pid);
    // int e_in_new  = m.edge_id(v_in, v_new);     assert(e_in_new>=0);
    // int e_out_new = m.edge_id(v_out, v_new);    assert(e_out_new>=0);
    // int e_dummy   = m.edge_id(v_mid, v_new);    assert(e_dummy>=0);

    //uint star_size = m.vert_valence(v_mid);
    //data.refinement_stats.vert_val_max  = std::max(data.refinement_stats.vert_val_max, star_size);
    //data.refinement_stats.vert_val_avg += star_size;

    //m.edge_data(e_in).label      = 0;
    //m.edge_data(e_out).label    -= val_in;
    //m.edge_data(e_in_new).label  = val_in;
    //m.edge_data(e_out_new).label = val_in;
    //m.edge_data(e_dummy).label   = 0;

    //// update flags
    //for(uint eid : m.adj_v2e(v_mid)) m.edge_data(eid).flags[MARKED] = m.edge_data(eid).label>0;
    //for(uint eid : m.adj_v2e(v_new)) m.edge_data(eid).flags[MARKED] = m.edge_data(eid).label>0;

    //return v_out;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void detach_loops_preproc(Trimesh<M,V,E,P>  & m,
                          HomotopyBasisData & data)
{
    // mark edges on basis loops, and count loops per edge (using edge labels)
    m.edge_unmark_all();
    m.edge_apply_label(0);
    for(auto loop : data.loops)
    {
        for(uint i=0; i<loop.size(); ++i)
        {
            uint v0  = loop.at(i);
            uint v1  = loop.at((i+1)%loop.size());
             int eid = m.edge_id(v0, v1);
            m.edge_data(eid).label++;
            m.edge_data(eid).flags[MARKED] = true;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void detach_loops_postproc(Trimesh<M,V,E,P>  & m,
                           HomotopyBasisData & data)
{
    // recompute basis
    data.loops.clear();
    m.edge_unmark_all();
    for(uint eid: m.adj_v2e(data.root))
    {
        if(m.edge_data(eid).flags[MARKED]) continue;
        if(m.edge_data(eid).label>0)
        {
            std::vector<uint> loop;
            loop.push_back(data.root);
            loop.push_back(m.vert_opposite_to(eid, data.root));
            m.edge_data(eid).flags[MARKED] = true;

            uint curr = loop.back();
            do
            {
                int next = -1;
                for(uint eid : m.adj_v2e(curr))
                {
                    if(m.edge_data(eid).label>0 && !m.edge_data(eid).flags[MARKED])
                    {
                        assert(next==-1);
                        next = m.vert_opposite_to(eid, curr);
                        m.edge_data(eid).flags[MARKED] = true;
                    }
                }
                assert(next>=0);
                if((uint)next != data.root) loop.push_back(next);
                curr = next;
            }
            while(curr != data.root);
            data.loops.push_back(loop);
        }
    }
    assert((int)data.loops.size() == m.genus()*2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// planarity test for the vert split operator
template<class M, class V, class E, class P>
CINO_INLINE
bool polys_are_planar(const Trimesh<M,V,E,P>  & m,
                      const std::vector<uint> & edge_fan,
                      const float               coplanarity_tresh)
{
    assert(edge_fan.size()>2);

    std::vector<uint> poly_fan;
    for(auto i=edge_fan.begin(),j=i+1; j<edge_fan.end(); ++i,++j)
    {
        int pid = m.poly_id(*i,*j);
        assert(pid>=0);
        poly_fan.push_back(pid);
    }

    std::vector<vec3d> n;
    n.reserve(poly_fan.size());
    for(uint pid : poly_fan) n.push_back(m.poly_data(pid).normal);

    for(auto i=n.begin(); i<n.end(); ++i)
    for(auto j=i+1;       j<n.end(); ++j)
    {
        if((*i).angle_deg(*j)>coplanarity_tresh) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// point positioning for the vert split operator
template<class M, class V, class E, class P>
CINO_INLINE
bool find_position_within_fan(const Trimesh<M,V,E,P>  & m,
                              const std::vector<uint> & edge_fan,
                              const uint                v_mid,
                                    vec3d             & pos)
{
    std::vector<uint> poly_fan;
    for(auto i=edge_fan.begin(),j=i+1; j<edge_fan.end(); ++i,++j)
    {
        int pid = m.poly_id(*i,*j);
        assert(pid>=0);
        poly_fan.push_back(pid);
    }

    assert(edge_fan.size()>2);
    uint eid = edge_fan.at(edge_fan.size()*0.5);
    uint v_opp = m.vert_opposite_to(eid, v_mid);

    vec3d A = m.vert(v_opp);
    vec3d B = m.vert(v_mid);
    uint  i = 0;
    float t = 0.75;
    pos = t*A + (1-t)*B;
    while(true)
    {
        bool reject = false;
        for(uint pid : poly_fan)
        {
            vec3d n1 = m.poly_data(pid).normal;
            auto  v  = m.poly_verts(pid);
            if(m.poly_vert_id(pid,0)==v_mid) v.at(0) = pos; else
            if(m.poly_vert_id(pid,1)==v_mid) v.at(1) = pos; else
            if(m.poly_vert_id(pid,2)==v_mid) v.at(2) = pos; else
            assert(false);
            vec3d n2 = triangle_normal(v.at(0), v.at(1), v.at(2));
            if(n2.is_degenerate() || n1.dot(n2) <= 0)
            {
                reject = true;
                break;
            }
        }
        // test first newly generated triangle
        if(!reject)
        {
            uint v_tmp = m.vert_opposite_to(*(edge_fan.begin()+1), v_mid);
            uint pid = poly_fan.front();
            auto v = m.poly_verts(pid);
            if(m.poly_vert_id(pid,0)==v_tmp) v.at(0) = pos; else
            if(m.poly_vert_id(pid,1)==v_tmp) v.at(1) = pos; else
            if(m.poly_vert_id(pid,2)==v_tmp) v.at(2) = pos; else
            assert(false);
            vec3d n1 = m.poly_data(pid).normal;
            vec3d n2 = triangle_normal(v.at(0), v.at(1), v.at(2));
            if(n2.is_degenerate() || n1.dot(n2) <= 0) reject = true;
        }
        // test second newly generated triangle
        if(!reject)
        {
            uint v_tmp = m.vert_opposite_to(*(edge_fan.rbegin()+1), v_mid);
            uint pid = poly_fan.back();
            auto v = m.poly_verts(pid);
            if(m.poly_vert_id(pid,0)==v_tmp) v.at(0) = pos; else
            if(m.poly_vert_id(pid,1)==v_tmp) v.at(1) = pos; else
            if(m.poly_vert_id(pid,2)==v_tmp) v.at(2) = pos; else
            assert(false);
            vec3d n1 = m.poly_data(pid).normal;
            vec3d n2 = triangle_normal(v.at(0), v.at(1), v.at(2));
            if(n2.is_degenerate() || n1.dot(n2) <= 0) reject = true;
        }
        if(!reject) return true;
        if(++i>5)   return false; // stop after 5 attempts
        t *= 0.5;
        pos  = t*A + (1-t)*B;
    }
    assert(false); // you should never end up executing this line
}

}
