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
#include <cinolib/meshes/trimesh.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/symbols.h>
#include <cinolib/cot.h>
#include <cinolib/vector_serialization.h>

#include <unordered_set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const char * filename)
{
    this->load(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<vec3d>              & verts,
                          const std::vector<std::vector<uint>>  & polys)

{
    this->init(verts, polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<double>             & coords,
                          const std::vector<std::vector<uint>>  & polys)
{
    this->init(vec3d_from_serialized_xyz(coords), polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<vec3d> & verts,
                          const std::vector<uint>  & polys)
{
    this->init(verts, polys_from_serialized_vids(polys,3));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<double> & coords,
                          const std::vector<uint>   & polys)
{
    this->init(vec3d_from_serialized_xyz(coords), polys_from_serialized_vids(polys,3));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::update_p_normal(const uint pid)
{
    vec3d v0 = this->poly_vert(pid,0);
    vec3d v1 = this->poly_vert(pid,1);
    vec3d v2 = this->poly_vert(pid,2);

    vec3d u = v1 - v0;    u.normalize();
    vec3d v = v2 - v0;    v.normalize();
    vec3d n = u.cross(v); n.normalize();

    this->poly_data(pid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::edge_opposite_to(const uint pid, const uint vid) const
{
    assert(this->poly_contains_vert(pid, vid));
    for(uint eid : this->adj_p2e(pid))
    {
        if(!this->edge_contains_vert(eid,vid)) return eid;
    }
    assert(false && "This is not supposed to happen!");
    return 0; // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_is_collapsible(const uint eid, const double lambda) const
{
    if(!edge_is_topologically_collapsible(eid)) return false;
    if(!edge_is_geometrically_collapsible(eid, lambda)) return false;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_is_topologically_collapsible(const uint eid) const
{
    // implements the "Link Condition" for 2-complexes described in:
    // Topology preserving edge contraction
    // Tamal K. Dey, Herbert Edelsbrunner, Sumanta Guha and Dmitry V. Nekhayev
    // 1999

    uint v0 = this->edge_vert_id(eid,0);
    uint v1 = this->edge_vert_id(eid,1);

    auto v0_e_link = this->vert_edges_link(v0);
    auto v1_e_link = this->vert_edges_link(v1);

    std::vector<uint> inters;
    SET_INTERSECTION(v0_e_link, v1_e_link, inters, true);
    if(!inters.empty()) return false;

    auto v0_v_link = this->vert_verts_link(v0);
    auto v1_v_link = this->vert_verts_link(v1);
    auto e_v_link  = this->edge_verts_link(eid);
    SORT_VEC(e_v_link, false);

    // to avoid topological changes at the border, boundary edges are assumed to form a triangle with an infinite vertex, which thus enters in the various vert links
    uint inf_vert = this->num_verts();
    if(this->vert_is_boundary(v0))  v0_v_link.push_back(inf_vert);
    if(this->vert_is_boundary(v1))  v1_v_link.push_back(inf_vert);
    if(this->edge_is_boundary(eid)) e_v_link.push_back(inf_vert); // (last position in the vector is ok, as inf_vert is the biggest id!)

    SET_INTERSECTION(v0_v_link, v1_v_link, inters, true);
    if(inters!=e_v_link) return false;

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_is_geometrically_collapsible(const uint eid, const double lambda) const
{
    // no triangle should flip or collapse
    vec3d new_vert = this->edge_sample_at(eid, lambda);
    uint  vid0     = this->edge_vert_id(eid,0);
    uint  vid1     = this->edge_vert_id(eid,1);

    std::unordered_set<uint> polys_to_test;
    for(uint pid : this->adj_v2p(vid0)) if(!this->poly_contains_edge(pid, eid)) polys_to_test.insert(pid);
    for(uint pid : this->adj_v2p(vid1)) if(!this->poly_contains_edge(pid, eid)) polys_to_test.insert(pid);

    for(uint pid : polys_to_test)
    {
        vec3d v[3];
        for(int i=0; i<3; ++i)
        {
            bool is_v0 = (this->poly_vert_id(pid,i) == vid0);
            bool is_v1 = (this->poly_vert_id(pid,i) == vid1);
            v[i] = (is_v0 || is_v1) ? new_vert : this->poly_vert(pid,i);
        }
        // avoid tiny triangles
        if(triangle_area(v[0], v[1], v[2]) < 1e-10) return false;
        // avoid flips and collapses
        if(triangle_normal(v[0], v[1], v[2]).dot(this->poly_data(pid).normal) <= 0) return false;
    }

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::vert_split(const uint eid0, const uint eid1)
{    
    uint v0 = this->vert_shared(eid0, eid1);
    uint v1 = this->vert_add(vec3d(0,0,0));

    // bi-partition the umbrella around the two split edges
    std::vector<uint> p_ring = this->vert_ordered_polys_star(v0);
    std::vector<uint> pids0, pids1;
    bool push0 = true;
    for(uint i=0; i<p_ring.size(); ++i)
    {
        uint curr = p_ring.at(i);
        if(push0) pids0.push_back(curr);
        else      pids1.push_back(curr);
        if(i<p_ring.size()-1)
        {
            uint next = p_ring.at(i+1);
             int eid  = this->edge_shared(curr,next); assert(eid>=0);
            if((uint)eid==eid0 || (uint)eid==eid1) push0 = !push0;
        }
    }
    assert(pids0.size() + pids1.size() == p_ring.size());
    assert(!p_ring.empty());

    // put left and right verts at the centroid of their reference polys
    // WARNING: not ideal for highly non planar umbrellas. should rather project on the actual surface
    vec3d xyz0(0,0,0);
    vec3d xyz1(0,0,0);
    for(uint pid : pids0) xyz0 += this->poly_centroid(pid);
    for(uint pid : pids1) xyz1 += this->poly_centroid(pid);
    if(!pids0.empty()) xyz0 /= static_cast<double>(pids0.size()); else xyz0 = this->vert(v0);
    if(!pids1.empty()) xyz1 /= static_cast<double>(pids1.size()); else xyz1 = this->vert(v0);
    this->vert(v0) = xyz0;
    this->vert(v1) = xyz1;

    for(uint pid : pids0) this->update_p_normal(pid);
    for(uint pid : pids1)
    {
        auto v_list = this->poly_verts_id(pid);
        for(uint & v : v_list) if(v==v0) v = v1;
        uint new_pid = this->poly_add(v_list);
        this->poly_data(new_pid) = this->poly_data(pid);
        this->update_p_normal(new_pid);
    }
    this->update_v_normal(v0);
    this->update_v_normal(v1);
    this->polys_remove(pids1);

    // tessellate the quad-like hole
    uint v2 = this->vert_opposite_to(eid0, v0);
    uint v3 = this->vert_opposite_to(eid1, v0);
    assert(this->adj_e2p(eid0).size()==1);
    uint vid_new = v1;
    uint ref_pid = this->adj_e2p(eid0).front();
    if(this->poly_verts_are_CCW(ref_pid, v0, v2)) std::swap(v0,v1);
    this->poly_add({v2, v0, v1});
    this->poly_add({v3, v1, v0});

    return vid_new;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int Trimesh<M,V,E,P>::edge_collapse(const uint eid, const double lambda, const bool topologic_check, const bool geometric_check)
{
    if(topologic_check && !edge_is_topologically_collapsible(eid))         return -1;
    if(geometric_check && !edge_is_geometrically_collapsible(eid, lambda)) return -1;

#ifndef NDEBUG
    int euler_before = this->Euler_characteristic();
#endif

    uint vert_to_keep   = this->edge_vert_id(eid,0);
    uint vert_to_remove = this->edge_vert_id(eid,1);
    if (vert_to_remove < vert_to_keep) std::swap(vert_to_keep, vert_to_remove); // remove vert with highest ID

    this->vert(vert_to_keep) = this->edge_sample_at(eid, lambda); // reposition vertex

    for(uint pid : this->adj_v2p(vert_to_remove))
    {
        if (this->poly_contains_edge(pid, eid)) continue; // no need to update. will collapse

        uint off   = this->poly_vert_offset(pid, vert_to_remove);
        auto vlist = this->poly_verts_id(pid);
        vlist.at(off) = vert_to_keep;
        uint new_pid = this->poly_add(vlist);

        this->poly_data(new_pid) = this->poly_data(pid);
        this->update_p_normal(new_pid);
    }
    this->update_v_normal(vert_to_keep);

    this->vert_remove(vert_to_remove);

    if(topologic_check)
    {
#ifndef NDEBUG
        assert(euler_before == this->Euler_characteristic());
#endif
    }

    return vert_to_keep;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::edge_split(const uint eid, const double lambda)
{
    return edge_split(eid, this->edge_sample_at(eid,lambda));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::edge_split(const uint eid, const vec3d & p)
{
    uint new_vid = this->vert_add(p);
    uint vid0    = this->edge_vert_id(eid,0);
    uint vid1    = this->edge_vert_id(eid,1);

    for(uint pid : this->adj_e2p(eid))
    {
        uint v_opp = this->vert_opposite_to(pid, vid0, vid1);
        if (this->poly_verts_are_CCW(pid, vid0, vid1)) std::swap(vid0, vid1);
        uint new_pid1 = this->poly_add(v_opp, vid0, new_vid);
        uint new_pid2 = this->poly_add(v_opp, new_vid, vid1);
        this->poly_data(new_pid1) = this->poly_data(pid);
        this->poly_data(new_pid2) = this->poly_data(pid);
        this->update_p_normal(new_pid1);
        this->update_p_normal(new_pid2);
    }
    this->update_v_normal(new_vid);

    // copy edge data
    int eid0 = this->edge_id(vid0,new_vid); assert(eid0>=0);
    int eid1 = this->edge_id(vid1,new_vid); assert(eid1>=0);
    this->edge_data(eid0) = this->edge_data(eid);
    this->edge_data(eid1) = this->edge_data(eid);

    this->polys_remove(this->adj_e2p(eid));
    return new_vid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_is_flippable(const uint eid)
{
    if( this->edge_is_boundary(eid)) return false;
    if(!this->edge_is_manifold(eid)) return false;

    // geometric check (if the projected outline is a concave quad, discard the move)
    assert(this->adj_e2p(eid).size()==2);
    uint  pid0 = this->adj_e2p(eid).front();
    uint  pid1 = this->adj_e2p(eid).back();
    uint  vid0 = this->edge_vert_id(eid,0);
    uint  vid1 = this->edge_vert_id(eid,1);
    uint  opp0 = this->vert_opposite_to(pid0,vid0,vid1);
    uint  opp1 = this->vert_opposite_to(pid1,vid0,vid1);
    if(!this->poly_verts_are_CCW(pid0, vid1, vid0)) std::swap(vid0,vid1);
    vec3d n0   = this->poly_data(pid0).normal;
    vec3d n1   = this->poly_data(pid1).normal;
    if(triangle_area(this->vert(opp0),this->vert(vid0),this->vert(opp1))<1e-5) return false;
    if(triangle_area(this->vert(opp1),this->vert(vid1),this->vert(opp0))<1e-5) return false;
    vec3d n2   = triangle_normal(this->vert(opp0),this->vert(vid0),this->vert(opp1));
    vec3d n3   = triangle_normal(this->vert(opp1),this->vert(vid1),this->vert(opp0));
    if(std::fabs(1.f-n2.length())>0.1) return false;
    if(std::fabs(1.f-n3.length())>0.1) return false;
    if(n0.dot(n2)<0) return false;
    if(n0.dot(n3)<0) return false;
    if(n1.dot(n2)<0) return false;
    if(n2.dot(n2)<0) return false;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double Trimesh<M,V,E,P>::edge_cotangent_weight(const uint eid) const
{
    assert(this->edge_is_manifold(eid));
    uint   vid0  = this->edge_vert_id(eid,0);
    uint   vid1  = this->edge_vert_id(eid,1);
    double count = 0.0;
    double sum   = 0.0;
    for(uint pid : this->adj_e2p(eid))
    {
        uint   v_opp = this->vert_opposite_to(pid, vid0, vid1);
        double alpha = this->poly_angle_at_vert(pid, v_opp);
        double c     = cot(alpha);
        if (!std::isnan(c))
        {
            sum   += std::max(1e-10, c); // avoid negative weights
            count += 1.0;
        }
    }
    if(count==0) return 0.0;
    return sum/count;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int Trimesh<M,V,E,P>::edge_flip(const uint eid)
{
    if(!edge_is_flippable(eid)) return -1;

    assert(this->adj_e2p(eid).size()==2);
    uint pid0 = this->adj_e2p(eid).front();
    uint pid1 = this->adj_e2p(eid).back();
    uint vid0 = this->edge_vert_id(eid,0);
    uint vid1 = this->edge_vert_id(eid,1);
    uint opp0 = this->vert_opposite_to(pid0,vid0,vid1);
    uint opp1 = this->vert_opposite_to(pid1,vid0,vid1);
    if(!this->poly_verts_are_CCW(pid0, vid1, vid0)) std::swap(vid0,vid1);
    std::vector<uint> p0 = { opp0, vid0, opp1 };
    std::vector<uint> p1 = { opp1, vid1, opp0 };
    this->poly_add(p0);
    this->poly_add(p1);

    this->edge_remove(eid);

    // copy edge data
    int new_eid = this->edge_id(opp0,opp1); assert(new_eid>=0);
    this->edge_data(new_eid) = this->edge_data(eid);

    return new_eid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::poly_bary_coords(const uint pid, const vec3d & p, double bc[]) const
{
    triangle_barycentric_coords(this->poly_vert(pid,0),
                                this->poly_vert(pid,1),
                                this->poly_vert(pid,2),
                                p, bc);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::poly_edge_id(const uint pid, const uint offset) const
{
    uint vid0 = this->poly_vert_id(pid, TRI_EDGES[offset][0]);
    uint vid1 = this->poly_vert_id(pid, TRI_EDGES[offset][1]);

    for(uint eid : this->adj_p2e(pid))
    {
        if (this->edge_contains_vert(eid,vid0) &&
            this->edge_contains_vert(eid,vid1))
        {
            return eid;
        }
    }
    assert(false && "Something is off here...");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::poly_split(const uint pid)
{
    // uses centroid as default split point
    return this->poly_split(pid, this->poly_centroid(pid));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::poly_split(const uint pid, const vec3d & p)
{
    uint vids[4] =
    {
        this->poly_vert_id(pid, 0),
        this->poly_vert_id(pid, 1),
        this->poly_vert_id(pid, 2),
        this->vert_add(p)
    };
    uint new_pid;
    new_pid = poly_add(vids[0], vids[1], vids[3]); this->poly_data(new_pid) = this->poly_data(pid);
    new_pid = poly_add(vids[1], vids[2], vids[3]); this->poly_data(new_pid) = this->poly_data(pid);
    new_pid = poly_add(vids[2], vids[0], vids[3]); this->poly_data(new_pid) = this->poly_data(pid);
    this->poly_remove(pid);
    return vids[3];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// for a definition of cap and needles, see:
// A Robust Procedure to Eliminate Degenerate Faces from Triangle Meshes
// Mario Botsch, Leif P. Kobbelt
// VMW 2001
//
template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_is_cap(const uint pid, const double angle_thresh_deg) const
{
    for(uint offset=0; offset<this->verts_per_poly(pid); ++offset)
    {
        uint vid = this->poly_vert_id(pid,offset);
        if (this->poly_angle_at_vert(pid,vid,DEG) > angle_thresh_deg)
        {
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// for a definition of cap and needles, see:
// A Robust Procedure to Eliminate Degenerate Faces from Triangle Meshes
// Mario Botsch, Leif P. Kobbelt
// VMW 2001
//
template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_is_needle(const uint pid, const double angle_thresh_deg) const
{
    for(uint offset=0; offset<this->verts_per_poly(pid); ++offset)
    {
        uint vid = this->poly_vert_id(pid,offset);
        if (this->poly_angle_at_vert(pid,vid,DEG) < angle_thresh_deg)
        {
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::poly_add(const uint vid0, const uint vid1, const uint vid2)
{
    std::vector<uint> p = { vid0, vid1, vid2 };
    return this->poly_add(p);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int Trimesh<M,V,E,P>::poly_id(const uint eid0, const uint eid1) const
{
    for(uint pid : this->adj_e2p(eid0))
    {
        if(this->poly_contains_edge(pid, eid1)) return pid;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double Trimesh<M,V,E,P>::poly_area(const uint pid) const
{
    vec3d p = this->poly_vert(pid,0);
    vec3d u = this->poly_vert(pid,1) - p;
    vec3d v = this->poly_vert(pid,2) - p;
    double area = 0.5 * u.cross(v).length();
    return area;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::vert_opposite_to(const uint pid, const uint vid0, const uint vid1) const
{
    assert(vid0!=vid1);
    assert(this->poly_contains_vert(pid, vid0));
    assert(this->poly_contains_vert(pid, vid1));
    for(uint off=0; off<this->verts_per_poly(pid); ++off)
    {
        uint vid = this->poly_vert_id(pid,off);
        if (vid != vid0 && vid != vid1) return vid;
    }
    assert(false);
    return 0; // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,P>::vert_link_edges(const uint vid) const
{
    std::vector<uint> e_link;
    for(uint pid : this->adj_v2p(vid))
    {
        e_link.push_back(this->edge_opposite_to(pid,vid));
    }
    return e_link;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,P>::verts_opposite_to(const uint eid) const
{
    std::vector<uint> vlist;
    for(uint pid : this->adj_e2p(eid))
    {
        vlist.push_back(this->vert_opposite_to(pid, this->edge_vert_id(eid,0), this->edge_vert_id(eid,1)));
    }
    return vlist;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,P>::edge_verts_link(const uint eid) const
{
    return verts_opposite_to(eid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::vert_weights(const uint vid, const int type, std::vector<std::pair<uint,double>> & wgts) const
{
    switch (type)
    {
        case UNIFORM   : this->vert_weights_uniform(vid, wgts); return;
        case COTANGENT : vert_weights_cotangent(vid, wgts); return;
        default        : assert(false && "Vert weights not supported at this level of the hierarchy!");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::vert_weights_cotangent(const uint vid, std::vector<std::pair<uint,double>> & wgts) const
{
    wgts.clear();
    for(uint eid : this->adj_v2e(vid))
    {
        uint   nbr = this->vert_opposite_to(eid, vid);
        double wgt = this->edge_cotangent_weight(eid);
        wgts.push_back(std::make_pair(nbr,wgt));
    }
}

}
