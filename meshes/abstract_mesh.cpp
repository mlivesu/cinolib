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
#include <cinolib/meshes/abstract_mesh.h>
#include <cinolib/bfs.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::clear()
{
    bb.reset();
    //
    verts.clear();
    edges.clear();
    polys.clear();
    //
    M std_M_data;
    m_data = std_M_data;
    v_data.clear();
    e_data.clear();
    p_data.clear();
    //
    v2v.clear();
    v2e.clear();
    v2p.clear();
    e2p.clear();
    p2e.clear();
    p2p.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d AbstractMesh<M,V,E,P>::centroid() const
{
    vec3d bary(0,0,0);
    for(auto p : this->verts) bary += p;
    if (this->num_verts() > 0) bary/=static_cast<double>(this->num_verts());
    return bary;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::translate(const vec3d & delta)
{
    for(uint vid=0; vid<this->num_verts(); ++vid) this->vert(vid) += delta;
    this->update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::rotate(const vec3d & axis, const double angle)
{
    double R[3][3];
    bake_rotation_matrix(axis, angle, R);
    //
    vec3d c = centroid();
    //
    for(uint vid=0; vid<this->num_verts(); ++vid)
    {
        this->vert(vid) -= c;
        transform(this->vert(vid), R);
        this->vert(vid) += c;
    }
    //
    this->update_bbox();
    this->update_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::scale(const double scale_factor)
{
    vec3d c = centroid();
    translate(-c);
    for(uint vid=0; vid<this->num_verts(); ++vid) this->vert(vid) *= scale_factor;
    translate(c);
    this->update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::connected_components() const
{
    std::vector<std::set<uint>> ccs;
    return connected_components(ccs);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::connected_components(std::vector<std::set<uint>> & ccs) const
{
    ccs.clear();
    uint seed = 0;
    std::vector<bool> visited(this->num_verts(), false);

    do
    {
        std::set<uint> cc;
        bfs_exahustive<AbstractMesh<M,V,E,P>>(*this, seed, cc);

        ccs.push_back(cc);
        for(uint vid : cc) visited.at(vid) = true;

        seed = 0;
        while (seed < this->num_verts() && visited.at(seed)) ++seed;
    }
    while (seed < this->num_verts());

    return ccs.size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::update_bbox()
{
    bb.reset();
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vec3d v = vert(vid);
        bb.min = bb.min.min(v);
        bb.max = bb.max.max(v);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<double> AbstractMesh<M,V,E,P>::vector_coords() const
{
    std::vector<double> coords;
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        coords.push_back(vert(vid).x());
        coords.push_back(vert(vid).y());
        coords.push_back(vert(vid).z());
    }
    return coords;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<float> AbstractMesh<M,V,E,P>::export_uvw_param(const int mode) const
{
    std::vector<float> uvw;
    uvw.reserve(num_verts());
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        switch (mode)
        {
            case U_param  : uvw.push_back(vert_data(vid).uvw[0]); break;
            case V_param  : uvw.push_back(vert_data(vid).uvw[1]); break;
            case W_param  : uvw.push_back(vert_data(vid).uvw[2]); break;
            case UV_param : uvw.push_back(vert_data(vid).uvw[0]);
                            uvw.push_back(vert_data(vid).uvw[1]); break;
            case UW_param : uvw.push_back(vert_data(vid).uvw[0]);
                            uvw.push_back(vert_data(vid).uvw[2]); break;
            case VW_param : uvw.push_back(vert_data(vid).uvw[1]);
                            uvw.push_back(vert_data(vid).uvw[2]); break;
            case UVW_param: uvw.push_back(vert_data(vid).uvw[0]);
                            uvw.push_back(vert_data(vid).uvw[1]);
                            uvw.push_back(vert_data(vid).uvw[2]); break;
            default: assert(false);
        }
    }
    return uvw;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::set<uint> AbstractMesh<M,V,E,P>::vert_n_ring(const uint vid, const uint n) const
{
    std::set<uint> active_set;
    std::set<uint> ring;

    active_set.insert(vid);
    for(uint i=0; i<n; ++i)
    {
        std::set<uint> next_active_set;

        for(uint curr : active_set)
        for(uint nbr  : adj_v2v(curr))
        {
            if (DOES_NOT_CONTAIN(ring,nbr) && nbr != vid) next_active_set.insert(nbr);
            ring.insert(nbr);
        }

        active_set = next_active_set;
    }
    return ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::verts_are_adjacent(const uint vid0, const uint vid1) const
{
    for(uint nbr : adj_v2v(vid0)) if (vid1==nbr) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::vert_is_local_min(const uint vid, const int tex_coord) const
{
    for(uint nbr : adj_v2v(vid))
    {
        switch (tex_coord)
        {
            case U_param : if (vert_data(nbr).uvw[0] < vert_data(vid).uvw[0]) return false; break;
            case V_param : if (vert_data(nbr).uvw[1] < vert_data(vid).uvw[1]) return false; break;
            case W_param : if (vert_data(nbr).uvw[2] < vert_data(vid).uvw[2]) return false; break;
            default: assert(false);
        }
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::vert_is_local_max(const uint vid, const int tex_coord) const
{
    for(uint nbr : adj_v2v(vid))
    {
        switch (tex_coord)
        {
            case U_param : if (vert_data(nbr).uvw[0] > vert_data(vid).uvw[0]) return false; break;
            case V_param : if (vert_data(nbr).uvw[1] > vert_data(vid).uvw[1]) return false; break;
            case W_param : if (vert_data(nbr).uvw[2] > vert_data(vid).uvw[2]) return false; break;
            default: assert(false);
        }
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::vert_valence(const uint vid) const
{
    return adj_v2v(vid).size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::vert_shared(const uint eid0, const uint eid1) const
{
    uint e00 = edge_vert_id(eid0,0);
    uint e01 = edge_vert_id(eid0,1);
    uint e10 = edge_vert_id(eid1,0);
    uint e11 = edge_vert_id(eid1,1);
    if (e00 == e10 || e00 == e11) return e00;
    if (e01 == e10 || e01 == e11) return e01;
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_set_color(const Color & c)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_set_alpha(const float alpha)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::edge_vert_id(const uint eid, const uint offset) const
{
    uint   eid_ptr = eid * 2;
    return edges.at(eid_ptr + offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d AbstractMesh<M,V,E,P>::edge_vert(const uint eid, const uint offset) const
{
    return vert(edge_vert_id(eid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::edge_contains_vert(const uint eid, const uint vid) const
{
    if (edge_vert_id(eid,0) == vid) return true;
    if (edge_vert_id(eid,1) == vid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractMesh<M,V,E,P>::edge_length(const uint eid) const
{
    return (edge_vert(eid,0) - edge_vert(eid,1)).length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractMesh<M,V,E,P>::edge_avg_length() const
{
    double avg = 0;
    for(uint eid=0; eid<num_edges(); ++eid) avg += edge_length(eid);
    if (num_edges() > 0) avg/=static_cast<double>(num_edges());
    return avg;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractMesh<M,V,E,P>::edge_max_length() const
{
    double max = 0;
    for(uint eid=0; eid<num_edges(); ++eid) max = std::max(max, edge_length(eid));
    return max;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractMesh<M,V,E,P>::edge_min_length() const
{
    double min = 0;
    for(uint eid=0; eid<num_edges(); ++eid) min = std::min(min,edge_length(eid));
    return min;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::edge_set_color(const Color & c)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::edge_set_alpha(const float alpha)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::poly_edge_id(const uint fid, const uint vid0, const uint vid1) const
{
    assert(poly_contains_vert(fid,vid0));
    assert(poly_contains_vert(fid,vid1));

    for(uint eid : adj_p2e(fid))
    {
        if (edge_contains_vert(eid,vid0) && edge_contains_vert(eid,vid1)) return eid;
    }

    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::poly_contains_edge(const uint fid, const uint eid) const
{
    for(uint e : adj_p2e(fid)) if (e == eid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_show_all()
{
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        poly_data(pid).visible = true;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::center_bbox()
{
    vec3d center = bb.center();
    for(uint vid=0; vid<num_verts(); ++vid) vert(vid) -= center;
    bb.min -= center;
    bb.max -= center;
    update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_set_color(const Color & c)
{
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        this->poly_data(pid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_set_alpha(const float alpha)
{
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        this->poly_data(pid).color.a = alpha;
    }
}


}
