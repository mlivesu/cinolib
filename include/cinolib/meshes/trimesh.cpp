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
#include <cinolib/meshes/trimesh.h>
#include <unordered_set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const char * filename)
{
    this->load(filename);
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<vec3d>              & verts,
                          const std::vector<std::vector<uint>>  & faces)

{
    this->verts = verts;
    this->polys = faces;
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<double>             & coords,
                          const std::vector<std::vector<uint>>  & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->polys = faces;
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<vec3d> & verts,
                          const std::vector<uint>  & faces)
{
    this->verts = verts;
    this->polys = faces_from_serialized_vids(faces,3);
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<double> & coords,
                          const std::vector<uint>   & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->polys = faces_from_serialized_vids(faces,3);
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::update_p_normal(const uint pid)
{
    vec3d A = this->poly_vert(pid,0);
    vec3d B = this->poly_vert(pid,1);
    vec3d C = this->poly_vert(pid,2);
    vec3d n = (B-A).cross(C-A);
    n.normalize();
    this->poly_data(pid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int Trimesh<M,V,E,P>::edge_opposite_to(const uint pid, const uint vid) const
{
    assert(this->poly_contains_vert(pid, vid));
    for(uint eid : this->adj_p2e(pid))
    {
        if (this->edge_vert_id(eid,0) != vid &&
            this->edge_vert_id(eid,1) != vid) return eid;
    }
    assert(false && "This is not supposed to happen!");
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_is_collapsible(const uint eid, const double lambda) const
{
    // HYPER-CONSERVATIVE CHECKS THAT SHOULD BE RELAXED SOMEDAY...
    {
        if (!this->edge_is_manifold(eid)) return false;

        uint vid0 = this->edge_vert_id(eid,0);
        uint vid1 = this->edge_vert_id(eid,1);
        if (this->vert_is_boundary(vid0)) return false;
        if (this->vert_is_boundary(vid1)) return false;
        for(uint nbr : this->adj_v2v(vid0)) if (this->vert_is_boundary(nbr)) return false;
        for(uint nbr : this->adj_v2v(vid1)) if (this->vert_is_boundary(nbr)) return false;
    }

    if (!edge_is_topologically_collapsible(eid)) return false;
    if (!edge_is_geometrically_collapsible(eid, lambda)) return false;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_is_topologically_collapsible(const uint eid) const
{
    // https://stackoverflow.com/questions/27049163/mesh-simplification-edge-collapse-conditions

    uint vid0 = this->edge_vert_id(eid,0);
    uint vid1 = this->edge_vert_id(eid,1);

    std::set<uint> v0_ring(this->adj_v2v(vid0).begin(), this->adj_v2v(vid0).end());
    std::set<uint> v1_ring(this->adj_v2v(vid1).begin(), this->adj_v2v(vid1).end());

    // http://en.cppreference.com/w/cpp/algorithm/set_intersection
    std::vector<uint> shared_verts;
    std::set_intersection(v0_ring.begin(), v0_ring.end(),
                          v1_ring.begin(), v1_ring.end(),
                          std::back_inserter(shared_verts));

    if ( this->edge_is_boundary(eid) && shared_verts.size() > 1) return false;
    if (!this->edge_is_boundary(eid) && shared_verts.size() > 2) return false;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_is_geometrically_collapsible(const uint eid, const double lambda) const
{
    // GEOMETRIC CONDITIONS: no triangle should flip
    //
    vec3d new_vert = this->edge_sample_at(eid, lambda);
    uint  vid0     = this->edge_vert_id(eid,0);
    uint  vid1     = this->edge_vert_id(eid,1);

    std::unordered_set<uint> polys_to_test;
    for(uint pid : this->adj_v2p(vid0)) if (!this->poly_contains_edge(pid, eid)) polys_to_test.insert(pid);
    for(uint pid : this->adj_v2p(vid1)) if (!this->poly_contains_edge(pid, eid)) polys_to_test.insert(pid);

    for(uint pid : polys_to_test)
    {
        int e_opp = -1;
        if (this->poly_contains_vert(pid, vid0)) e_opp = this->edge_opposite_to(pid, vid0); else
        if (this->poly_contains_vert(pid, vid1)) e_opp = this->edge_opposite_to(pid, vid1); else
        assert(false);

        uint A = this->edge_vert_id(e_opp,0);
        uint B = this->edge_vert_id(e_opp,1);
        if (this->poly_verts_are_CCW(pid,A,B)) std::swap(A, B);
        vec3d new_n = triangle_normal(this->vert(A), this->vert(B), new_vert);

        if (this->poly_data(pid).normal.dot(new_n) < 0) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_collapse(const uint eid, const double lambda)
{
    if (!edge_is_collapsible(eid, lambda)) return false;

    uint vert_to_keep   = this->edge_vert_id(eid,0);
    uint vert_to_remove = this->edge_vert_id(eid,1);
    if (vert_to_remove < vert_to_keep) std::swap(vert_to_keep, vert_to_remove); // remove vert with highest ID

    this->vert(vert_to_keep) = this->edge_sample_at(eid, lambda); // reposition vertex

    for(uint pid : this->adj_v2p(vert_to_remove))
    {
        if (this->poly_contains_edge(pid, eid)) continue; // no need to update. will collapse

        int  e_opp = this->edge_opposite_to(pid, vert_to_remove);
        uint A     = this->edge_vert_id(e_opp,0);
        uint B     = this->edge_vert_id(e_opp,1);

        if (this->poly_verts_are_CCW(pid,A,B)) std::swap(A, B);
        uint new_pid = this->poly_add(A, B, vert_to_keep);

        this->poly_data(new_pid) = this->poly_data(pid);
        this->update_p_normal(new_pid);
    }
    this->update_v_normal(vert_to_keep);

    this->vert_remove(vert_to_remove);
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::edge_split(const uint eid, const double lambda)
{
    uint new_vid = this->vert_add(this->edge_sample_at(eid,lambda));
    uint vid0    = this->edge_vert_id(eid,0);
    uint vid1    = this->edge_vert_id(eid,1);

    for(uint pid : this->adj_e2p(eid))
    {
        uint v_opp = this->vert_opposite_to(pid, vid0, vid1);
        if (this->poly_verts_are_CCW(pid, vid0, vid1)) std::swap(vid0, vid1);
        this->poly_add(v_opp, vid0, new_vid);
        this->poly_add(v_opp, new_vid, vid1);
    }
    this->polys_remove(this->adj_e2p(eid));

    return new_vid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_bary_coords(const uint pid, const vec3d & p, std::vector<double> & wgts) const
{
    return triangle_barycentric_coords(this->poly_vert(pid,0),
                                       this->poly_vert(pid,1),
                                       this->poly_vert(pid,2),
                                       p, wgts);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_bary_is_vert(const uint pid, const std::vector<double> & wgts, uint & vid, const double tol) const
{
    uint off;
    if (triangle_bary_is_vertex(wgts, off, tol))
    {
        vid = this->poly_vert_id(pid, off);
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_bary_is_edge(const uint pid, const std::vector<double> & wgts, uint & eid, const double tol) const
{
    uint off;
    if (triangle_bary_is_edge(wgts, off, tol))
    {
        eid = poly_edge_id(pid, off);
        return true;
    }
    return false;
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
void Trimesh<M,V,E,P>::poly_set(const uint pid, const uint vid0, const uint vid1, const uint vid2)
{
    /* WARNING!!!! This completely screws up edge connectivity!!!!!! */

    assert(vid0 < this->num_verts());
    assert(vid1 < this->num_verts());
    assert(vid2 < this->num_verts());

    this->polys.at(pid).clear();
    this->polys.at(pid).push_back(vid0);
    this->polys.at(pid).push_back(vid1);
    this->polys.at(pid).push_back(vid2);

    this->update_p_normal(pid);
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
    assert(this->poly_contains_vert(pid, vid0));
    assert(this->poly_contains_vert(pid, vid1));
    for(uint off=0; off<this->verts_per_poly(pid); ++off)
    {
        uint vid = this->poly_vert_id(pid,off);
        if (vid != vid0 && vid != vid1) return vid;
    }
    assert(false);
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
        assert(this->edge_is_manifold(eid));
        uint   nbr   = this->vert_opposite_to(eid, vid);
        double wgt   = 0.0;
        uint   count = 0;
        for(uint pid : this->adj_e2p(eid))
        {
            double alpha = this->poly_angle_at_vert(pid, vert_opposite_to(pid, vid, nbr));
            if (!std::isnan(alpha))
            {
                wgt += cot(alpha);
                ++count;
            }
        }
        wgt = (count > 0) ? wgt/static_cast<double>(count) : 0.0;
        wgts.push_back(std::make_pair(nbr,wgt));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,P>::get_ordered_boundary_vertices() const
{
    // NOTE: assumes the mesh contains exactly ONE simply connected boundary!

    std::vector<uint>  faces;
    std::vector<vec3d> verts = this->vector_verts();
    verts.push_back(this->centroid());

    uint pid = this->num_verts();
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (this->edge_is_boundary(eid))
        {
            uint pid  = this->adj_e2p(eid).front();
            uint vid0 = this->edge_vert_id(eid,0);
            uint vid1 = this->edge_vert_id(eid,1);
            if (this->poly_vert_offset(pid,vid0) > this->poly_vert_offset(pid,vid1))
            {
                std::swap(vid0,vid1);
            }
            faces.push_back(vid0);
            faces.push_back(vid1);
            faces.push_back(pid);
        }
    }

    Trimesh<> tmp(verts,faces);
    return tmp.vert_ordered_vert_ring(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
const std::vector<uint> & Trimesh<M,V,E,P>::poly_tessellation(const uint pid) const
{
    return this->polys.at(pid);
}

}
