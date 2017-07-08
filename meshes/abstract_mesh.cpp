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

namespace cinolib
{

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::update_bbox()
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

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::update_v_normal(const uint vid)
{
    vec3d n(0,0,0);
    for(uint fid : adj_v2f(vid))
    {
        n += face_data(fid).normal;
    }
    n.normalize();
    vert_data(vid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::update_f_normals()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        update_f_normal(fid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::update_v_normals()
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        update_v_normal(vid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::update_normals()
{
    update_f_normals();
    update_v_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<double> AbstractMesh<M,V,E,F>::vector_coords() const
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

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<float> AbstractMesh<M,V,E,F>::export_uvw_param(const int mode) const
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

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<int> AbstractMesh<M,V,E,F>::export_per_face_labels() const
{
    std::vector<int> labels(num_faces());
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        labels.at(face_data(fid).label);
    }
    return labels;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<Color> AbstractMesh<M,V,E,F>::export_per_face_colors() const
{
    std::vector<Color> colors(num_faces());
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        colors.at(fid) = face_data(fid).color;
    }
    return colors;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::set<uint> AbstractMesh<M,V,E,F>::vert_n_ring(const uint vid, const uint n) const
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

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractMesh<M,V,E,F>::verts_are_adjacent(const uint vid0, const uint vid1) const
{
    for(uint nbr : adj_v2v(vid0)) if (vid1==nbr) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractMesh<M,V,E,F>::vert_is_local_min(const uint vid, const int tex_coord) const
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

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractMesh<M,V,E,F>::vert_is_local_max(const uint vid, const int tex_coord) const
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

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractMesh<M,V,E,F>::vert_valence(const uint vid) const
{
    return adj_v2v(vid).size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractMesh<M,V,E,F>::vert_shared(const uint eid0, const uint eid1) const
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

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::vert_set_color(const Color & c)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::vert_set_alpha(const float alpha)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractMesh<M,V,E,F>::edge_vert_id(const uint eid, const uint offset) const
{
    uint   eid_ptr = eid * 2;
    return edges.at(eid_ptr + offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d AbstractMesh<M,V,E,F>::edge_vert(const uint eid, const uint offset) const
{
    return vert(edge_vert_id(eid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractMesh<M,V,E,F>::edge_contains_vert(const uint eid, const uint vid) const
{
    if (edge_vert_id(eid,0) == vid) return true;
    if (edge_vert_id(eid,1) == vid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double AbstractMesh<M,V,E,F>::edge_length(const uint eid) const
{
    return (edge_vert(eid,0) - edge_vert(eid,1)).length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double AbstractMesh<M,V,E,F>::edge_avg_length() const
{
    double avg = 0;
    for(uint eid=0; eid<num_edges(); ++eid) avg += edge_length(eid);
    if (num_edges() > 0) avg/=static_cast<double>(num_edges());
    return avg;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double AbstractMesh<M,V,E,F>::edge_max_length() const
{
    double max = 0;
    for(uint eid=0; eid<num_edges(); ++eid) max = std::max(max, edge_length(eid));
    return max;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double AbstractMesh<M,V,E,F>::edge_min_length() const
{
    double min = 0;
    for(uint eid=0; eid<num_edges(); ++eid) min = std::min(min,edge_length(eid));
    return min;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::edge_set_color(const Color & c)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::edge_set_alpha(const float alpha)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractMesh<M,V,E,F>::face_vert_id(const uint fid, const uint offset) const
{
    return faces.at(fid).at(offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d AbstractMesh<M,V,E,F>::face_vert(const uint fid, const uint offset) const
{
    return vert(face_vert_id(fid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractMesh<M,V,E,F>::face_edge_id(const uint fid, const uint vid0, const uint vid1) const
{
    assert(face_contains_vert(fid,vid0));
    assert(face_contains_vert(fid,vid1));

    for(uint eid : adj_f2e(fid))
    {
        if (edge_contains_vert(eid,vid0) && edge_contains_vert(eid,vid1)) return eid;
    }

    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractMesh<M,V,E,F>::face_vert_offset(const uint fid, const uint vid) const
{
    for(uint offset=0; offset<verts_per_face(fid); ++offset)
    {
        if (face_vert_id(fid,offset) == vid) return offset;
    }
    assert(false && "Something is off here...");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d AbstractMesh<M,V,E,F>::face_centroid(const uint fid) const
{
    vec3d c(0,0,0);
    for(uint off=0; off<verts_per_face(fid); ++off)
    {
        c += face_vert(fid,off);
    }
    c /= static_cast<double>(verts_per_face(fid));
    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double AbstractMesh<M,V,E,F>::face_angle_at_vert(const uint fid, const uint vid, const int unit) const
{
    assert(face_contains_vert(fid,vid));

    uint offset = 0;
    for(uint i=0; i<verts_per_face(fid); ++i) if (face_vert_id(fid,i) == vid) offset = i;
    assert(face_vert_id(fid,offset) == vid);
    //
    // the code above substitutes this one (which was specific for AbstractMeshes...)
    //
    //     if (face_vert_id(fid,0) == vid) offset = 0;
    //else if (face_vert_id(fid,1) == vid) offset = 1;
    //else if (face_vert_id(fid,2) == vid) offset = 2;
    //else { assert(false); offset=0; } // offset=0 kills uninitialized warning message

    vec3d P = vert(vid);
    vec3d u = face_vert(fid,(offset+1)%verts_per_face(fid)) - P;
    vec3d v = face_vert(fid,(offset+2)%verts_per_face(fid)) - P;

    switch (unit)
    {
        case RAD : return u.angle_rad(v);
        case DEG : return u.angle_deg(v);
        default  : assert(false);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractMesh<M,V,E,F>::face_contains_vert(const uint fid, const uint vid) const
{
    for(uint off=0; off<verts_per_face(fid); ++off)
    {
        if (face_vert_id(fid,off) == vid) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::face_flip_winding_order(const uint fid)
{
    std::reverse(faces.at(fid).begin(), faces.at(fid).end());

    update_f_normal(fid);
    for(uint off=0; off<verts_per_face(fid); ++off) update_v_normal(face_vert_id(fid,off));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::face_set_color(const Color & c)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::face_set_alpha(const float alpha)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractMesh<M,V,E,F>::center_bbox()
{
    vec3d center = bb.center();
    for(uint vid=0; vid<num_verts(); ++vid) vert(vid) -= center;
    bb.min -= center;
    bb.max -= center;
    update_bbox();
}



}
