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
#include <cinolib/meshes/abstract_mesh.h>
#include <cinolib/meshes/mesh_attributes.h>
#include <cinolib/stl_container_utilities.h>
#include <cinolib/min_max_inf.h>
#include <map>
#include <unordered_set>
#include <unordered_map>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::mesh_is_surface() const
{
    switch (mesh_type())
    {
        case TRIMESH       : return true;
        case QUADMESH      : return true;
        case POLYGONMESH   : return true;
        case TETMESH       : return false;
        case HEXMESH       : return false;
        case POLYHEDRALMESH: return false;
        default : assert(false);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::mesh_is_volumetric() const
{
    return !mesh_is_surface();
}
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
    for(auto p : verts) bary += p;
    if (num_verts() > 0) bary/=static_cast<double>(num_verts());
    return bary;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::translate(const vec3d & delta)
{
    for(uint vid=0; vid<num_verts(); ++vid) vert(vid) += delta;
    bb.min += delta;
    bb.max += delta;
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
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert(vid) -= c;
        transform(vert(vid), R);
        vert(vid) += c;
    }
    //
    update_bbox();
    update_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::scale(const double scale_factor)
{
    vec3d c = centroid();
    translate(-c);
    for(uint vid=0; vid<num_verts(); ++vid) vert(vid) *= scale_factor;
    translate(c);
    update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::normalize_bbox()
{
    double s = 1.0/bbox().diag();
    for(uint vid=0; vid<num_verts(); ++vid) vert(vid) *= s;
    update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::update_bbox()
{
    bb.update(this->verts);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<vec3d> AbstractMesh<M,V,E,P>::vector_vert_normals() const
{
    std::vector<vec3d> normals;
    normals.reserve(num_verts());
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        normals.push_back(vert_data(vid).normal);
    }
    return normals;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<Color> AbstractMesh<M,V,E,P>::vector_vert_colors() const
{
    std::vector<Color> colors;
    colors.reserve(num_verts());
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        colors.push_back(vert_data(vid).color);
    }
    return colors;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> AbstractMesh<M,V,E,P>::vector_vert_labels() const
{
    std::vector<int> labels;
    labels.reserve(num_verts());
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        labels.push_back(vert_data(vid).label);
    }
    return labels;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<Color> AbstractMesh<M,V,E,P>::vector_edge_colors() const
{
    std::vector<Color> colors;
    colors.reserve(num_edges());
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        colors.push_back(edge_data(eid).color);
    }
    return colors;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> AbstractMesh<M,V,E,P>::vector_edge_labels() const
{
    std::vector<int> labels;
    labels.reserve(num_edges());
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        labels.push_back(edge_data(eid).label);
    }
    return labels;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<vec3d> AbstractMesh<M,V,E,P>::vector_poly_normals() const
{
    std::vector<vec3d> normals;
    normals.reserve(num_polys());
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        normals.push_back(poly_data(pid).normal);
    }
    return normals;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<Color> AbstractMesh<M,V,E,P>::vector_poly_colors() const
{
    std::vector<Color> colors;
    colors.reserve(num_polys());
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        colors.push_back(poly_data(pid).color);
    }
    return colors;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> AbstractMesh<M,V,E,P>::vector_poly_labels() const
{
    std::vector<int> labels;
    labels.reserve(num_polys());
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        labels.push_back(poly_data(pid).label);
    }
    return labels;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<Color> AbstractMesh<M,V,E,P>::vector_poly_unique_colors() const
{
    std::vector<Color> colors = vector_poly_colors();
    REMOVE_DUPLICATES_FROM_VEC(colors);
    return colors;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> AbstractMesh<M,V,E,P>::vector_poly_unique_labels() const
{
    std::vector<int> labels = vector_poly_labels();
    REMOVE_DUPLICATES_FROM_VEC(labels);
    return labels;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<double> AbstractMesh<M,V,E,P>::serialize_uvw(const int mode) const
{
    std::vector<double> uvw;
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
void AbstractMesh<M,V,E,P>::deserialize_uvw(const std::vector<vec3d> & uvw)
{
    assert(uvw.size()==num_verts());
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).uvw = uvw.at(vid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::copy_xyz_to_uvw(const int mode)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        switch (mode)
        {
            case U_param  : vert_data(vid).uvw[0] = vert(vid).x(); break;
            case V_param  : vert_data(vid).uvw[1] = vert(vid).y(); break;
            case W_param  : vert_data(vid).uvw[2] = vert(vid).z(); break;
            case UV_param : vert_data(vid).uvw[0] = vert(vid).x();
                            vert_data(vid).uvw[1] = vert(vid).y(); break;
            case UW_param : vert_data(vid).uvw[0] = vert(vid).x();
                            vert_data(vid).uvw[2] = vert(vid).z(); break;
            case VW_param : vert_data(vid).uvw[1] = vert(vid).y();
                            vert_data(vid).uvw[2] = vert(vid).z(); break;
            case UVW_param: vert_data(vid).uvw[0] = vert(vid).x();
                            vert_data(vid).uvw[1] = vert(vid).y();
                            vert_data(vid).uvw[2] = vert(vid).z(); break;
            default: assert(false);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::copy_uvw_to_xyz(const int mode)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        switch (mode)
        {
            case U_param  : vert(vid).x() = vert_data(vid).uvw[0]; break;
            case V_param  : vert(vid).y() = vert_data(vid).uvw[1]; break;
            case W_param  : vert(vid).z() = vert_data(vid).uvw[2]; break;
            case UV_param : vert(vid).x() = vert_data(vid).uvw[0];
                            vert(vid).y() = vert_data(vid).uvw[1]; break;
            case UW_param : vert(vid).x() = vert_data(vid).uvw[0];
                            vert(vid).z() = vert_data(vid).uvw[2]; break;
            case VW_param : vert(vid).y() = vert_data(vid).uvw[1];
                            vert(vid).z() = vert_data(vid).uvw[2]; break;
            case UVW_param: vert(vid).x() = vert_data(vid).uvw[0];
                            vert(vid).y() = vert_data(vid).uvw[1];
                            vert(vid).z() = vert_data(vid).uvw[2]; break;
            default: assert(false);
        }
    }
    update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::swap_xyz_uvw(const bool normals, const bool bbox)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        std::swap(vert(vid),vert_data(vid).uvw);
    }
    if (normals) update_normals();
    if (bbox)    update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractMesh<M,V,E,P>::adj_e2v(const uint eid) const
{
    return { this->edge_vert_id(eid,0),
             this->edge_vert_id(eid,1) };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractMesh<M,V,E,P>::adj_e2e(const uint eid) const
{
    std::unordered_set<uint> unique_e_list;
    uint v0 = this->edge_vert_id(eid,0);
    uint v1 = this->edge_vert_id(eid,1);
    for(uint nbr : this->adj_v2e(v0)) if(nbr != eid) unique_e_list.insert(nbr);
    for(uint nbr : this->adj_v2e(v1)) if(nbr != eid) unique_e_list.insert(nbr);
    std::vector<uint> e_list(unique_e_list.begin(), unique_e_list.end());
    return e_list;
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
uint AbstractMesh<M,V,E,P>::vert_opposite_to(const uint eid, const uint vid) const
{
    assert(this->edge_contains_vert(eid, vid));
    if (this->edge_vert_id(eid,0) != vid) return this->edge_vert_id(eid,0);
    else                                  return this->edge_vert_id(eid,1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_weights(const uint vid, const int type, std::vector<std::pair<uint,double>> & wgts) const
{
    switch (type)
    {
        case UNIFORM : vert_weights_uniform(vid, wgts); return;
        default      : assert(false && "Vert weights not supported at this level of the hierarchy!");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_weights_uniform(const uint vid, std::vector<std::pair<uint,double>> & wgts) const
{
    wgts.clear();
    double w = 1.0; // / (double)nbrs.size(); // <= WARNING: makes the matrix non-symmetric!!!!!
    for(uint nbr : adj_v2v(vid))
    {
        wgts.push_back(std::make_pair(nbr,w));
    }
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
    assert(adj_v2v(vid).size() == adj_v2e(vid).size());
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
    return 0; // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractMesh<M,V,E,P>::vert_min_uvw_value(const int tex_coord) const
{
    double min = inf_double;
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        switch (tex_coord)
        {
            case U_param : min = std::min(min, vert_data(vid).uvw[0]); break;
            case V_param : min = std::min(min, vert_data(vid).uvw[1]); break;
            case W_param : min = std::min(min, vert_data(vid).uvw[2]); break;
            default: assert(false);
        }
    }
    return min;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractMesh<M,V,E,P>::vert_max_uvw_value(const int tex_coord) const
{
    double max = -inf_double;
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        switch (tex_coord)
        {
            case U_param : max = std::max(max, vert_data(vid).uvw[0]); break;
            case V_param : max = std::max(max, vert_data(vid).uvw[1]); break;
            case W_param : max = std::max(max, vert_data(vid).uvw[2]); break;
            default: assert(false);
        }
    }
    return max;
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
void AbstractMesh<M,V,E,P>::vert_unmark_all()
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).flags[MARKED] = false;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_local_unmark_near_vert(const uint vid)
{
    vert_data(vid).flags[MARKED_LOCAL] = false;
    for(uint nbr : adj_v2v(vid)) vert_data(nbr).flags[MARKED_LOCAL] = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_local_unmark_near_edge(const uint eid)
{
    vert_data(edge_vert_id(eid,0)).flags[MARKED_LOCAL] = false;
    vert_data(edge_vert_id(eid,1)).flags[MARKED_LOCAL] = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_local_unmark_near_poly(const uint pid)
{
    for(uint vid : adj_p2v(pid)) vert_data(vid).flags[MARKED_LOCAL] = false;
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
std::vector<uint> AbstractMesh<M,V,E,P>::edge_vert_ids(const uint eid) const
{
    return { this->edge_vert_id(eid,0), this->edge_vert_id(eid,1) };
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<vec3d> AbstractMesh<M,V,E,P>::edge_verts(const uint eid) const
{
    return {this->edge_vert(eid,0), this->edge_vert(eid,1)};
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int AbstractMesh<M,V,E,P>::edge_id(const uint vid0, const uint vid1) const
{
    assert(vid0 != vid1);
    for(uint eid : adj_v2e(vid0))
    {
        if(edge_contains_vert(eid,vid0) && edge_contains_vert(eid,vid1))
        {
            return eid;
        }
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int AbstractMesh<M,V,E,P>::edge_id(const ipair & vids) const
{
    return edge_id(vids.first, vids.second);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int AbstractMesh<M,V,E,P>::edge_id(const std::vector<uint> & vids) const
{
    assert(vids.size()==2);
    return edge_id(vids.front(), vids.back());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::edge_valence(const uint eid) const
{
    return this->adj_e2p(eid).size();
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
bool AbstractMesh<M,V,E,P>::edges_are_adjacent(const uint eid0, const uint eid1) const
{
    if (edge_vert_id(eid0,0)==edge_vert_id(eid1,0)) return true;
    if (edge_vert_id(eid0,0)==edge_vert_id(eid1,1)) return true;
    if (edge_vert_id(eid0,1)==edge_vert_id(eid1,0)) return true;
    if (edge_vert_id(eid0,1)==edge_vert_id(eid1,1)) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d AbstractMesh<M,V,E,P>::edge_sample_at(const uint eid, const double lambda) const
{
    return ((1.0-lambda)*edge_vert(eid,0) + lambda*edge_vert(eid,1));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractMesh<M,V,E,P>::edge_length(const uint eid) const
{
    return edge_vec(eid).length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d AbstractMesh<M,V,E,P>::edge_vec(const uint eid, const bool normalized) const
{
    vec3d e(edge_vert(eid,1) - edge_vert(eid,0));
    if(normalized) e.normalize();
    return e;
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
void AbstractMesh<M,V,E,P>::edge_unmark_all()
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).flags[MARKED] = false;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::edge_local_unmark_near_vert(const uint vid)
{
    for(uint eid : adj_v2e(vid)) edge_data(eid).flags[MARKED_LOCAL] = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::edge_local_unmark_near_edge(const uint eid)
{
    edge_data(eid).flags[MARKED_LOCAL] = false;
    for(uint nbr : adj_e2e(eid)) edge_data(nbr).flags[MARKED_LOCAL] = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::edge_local_unmark_near_poly(const uint pid)
{
    for(uint eid : adj_p2e(pid)) edge_data(eid).flags[MARKED_LOCAL] = false;
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
uint AbstractMesh<M,V,E,P>::poly_vert_id(const uint pid, const uint offset) const
{
    return adj_p2v(pid).at(offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::poly_vert_offset(const uint pid, const uint vid) const
{
    assert(poly_contains_vert(pid,vid));
    for(uint off=0; off<verts_per_poly(pid); ++off)
    {
        if(poly_vert_id(pid,off) == vid) return off;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d AbstractMesh<M,V,E,P>::poly_centroid(const uint pid) const
{
    vec3d c(0,0,0);
    for(uint vid : adj_p2v(pid)) c += vert(vid);
    c /= static_cast<double>(verts_per_poly(pid));
    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d AbstractMesh<M,V,E,P>::poly_sample_at(const uint pid, const std::vector<double> & bc) const
{
    assert(bc.size() == verts_per_poly(pid));
    vec3d p(0,0,0);
    for(uint off=0; off<verts_per_poly(pid); ++off)
    {
        p += bc.at(off) * poly_vert(pid,off);
    }
    return p;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractMesh<M,V,E,P>::poly_sample_param_at(const uint pid, const std::vector<double> & bc, const int tex_coord) const
{
    assert(bc.size() == verts_per_poly(pid));
    double val = 0;
    for(uint off=0; off<verts_per_poly(pid); ++off)
    {
        switch(tex_coord)
        {
            case U_param : val += bc.at(off) * this->vert_data(this->poly_vert_id(pid,off)).uvw[0]; break;
            case V_param : val += bc.at(off) * this->vert_data(this->poly_vert_id(pid,off)).uvw[1]; break;
            case W_param : val += bc.at(off) * this->vert_data(this->poly_vert_id(pid,off)).uvw[2]; break;
            default: assert(false);
        }
    }
    return val;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
vec3d AbstractMesh<M,V,E,P>::poly_vert(const uint pid, const uint offset) const
{
    return vert(poly_vert_id(pid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<vec3d> AbstractMesh<M,V,E,P>::poly_verts(const uint pid) const
{
    uint nv = this->verts_per_poly(pid);
    std::vector<vec3d> p_list(nv);
    for(uint off=0; off<nv; ++off) p_list.at(off) = this->poly_vert(pid,off);
    return p_list;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractMesh<M,V,E,P>::poly_verts_id(const uint pid, const bool sort_by_vid) const
{
    if(sort_by_vid)
    {
        std::vector<uint> v_list = this->adj_p2v(pid);
        SORT_VEC(v_list);
        return v_list;
    }
    return this->adj_p2v(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractMesh<M,V,E,P>::poly_v2v(const uint pid, const uint vid) const
{
    assert(this->poly_contains_vert(pid,vid));
    std::vector<uint> verts;
    for(uint eid : this->adj_v2e(vid))
    {
        if(this->poly_contains_edge(pid,eid)) verts.push_back(this->vert_opposite_to(eid,vid));
    }
    return verts;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractMesh<M,V,E,P>::poly_v2e(const uint pid, const uint vid) const
{
    assert(this->poly_contains_vert(pid,vid));
    std::vector<uint> edges;
    for(uint eid : this->adj_v2e(vid))
    {
        if(this->poly_contains_edge(pid,eid)) edges.push_back(eid);
    }
    return edges;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::poly_vert_valence(const uint pid, const uint vid) const
{
    return this->poly_v2v(pid,vid).size();
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
    return 0; // warning killer
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::poly_contains_edge(const uint pid, const uint eid) const
{
    for(uint e : adj_p2e(pid)) if (e == eid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::poly_contains_edge(const uint pid, const uint vid0, const uint vid1) const
{
    for(uint eid : adj_p2e(pid))
    {
        if (edge_contains_vert(eid, vid0) &&
            edge_contains_vert(eid, vid1))
        {
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_show_all()
{
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        poly_data(pid).flags[HIDDEN] = false;
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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_set_color(const Color & c)
{
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        poly_data(pid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_set_alpha(const float alpha)
{
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        poly_data(pid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_color_wrt_label(const bool sorted, const float s, const float v) // s => saturation, v => value in HSV color space
{
    std::map<int,uint> l_map;
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        int l = this->poly_data(pid).label;
        if(DOES_NOT_CONTAIN(l_map,l))
        {
            uint fresh_label = l_map.size();
            l_map[l] = fresh_label;
        }
    }
    uint n_labels = l_map.size();
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        if(sorted) this->poly_data(pid).color = Color::hsv_ramp(n_labels, this->poly_data(pid).label);
        else       this->poly_data(pid).color = Color::scatter(n_labels,l_map.at(this->poly_data(pid).label), s, v);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_label_wrt_color()
{
    std::map<Color,int> colormap;
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        const Color & c = this->poly_data(pid).color;
        if (DOES_NOT_CONTAIN(colormap,c)) colormap[c] = colormap.size();
    }
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        this->poly_data(pid).label = colormap.at(this->poly_data(pid).color);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::poly_contains_vert(const uint pid, const uint vid) const
{
    for(uint v : adj_p2v(pid)) if(v == vid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_unmark_all()
{
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        poly_data(pid).flags[MARKED] = false;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_local_unmark_near_vert(const uint vid)
{
    for(uint pid : adj_v2p(vid)) poly_data(pid).flags[MARKED_LOCAL] = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_local_unmark_near_edge(const uint eid)
{
    for(uint pid : adj_e2p(eid)) poly_data(pid).flags[MARKED_LOCAL] = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_local_unmark_near_poly(const uint pid)
{
    poly_data(pid).flags[MARKED_LOCAL] = false;
    for(uint nbr : adj_p2p(pid)) poly_data(nbr).flags[MARKED_LOCAL] = false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::polys_n_unique_colors() const
{
    return vector_poly_unique_colors().size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::polys_n_unique_labels() const
{
    return vector_poly_unique_labels().size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::polys_are_colored() const
{
    return (polys_n_unique_colors()>1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractMesh<M,V,E,P>::polys_are_labeled() const
{
    return (polys_n_unique_labels()>1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_apply_labels(const std::vector<int> & labels)
{
    assert(labels.size() == this->num_polys());
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        poly_data(pid).label = labels.at(pid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::poly_apply_label(const int label)
{
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        poly_data(pid).label = label;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
AABB AbstractMesh<M,V,E,P>::poly_aabb(const uint pid) const
{
    return AABB(poly_verts(pid));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::edge_apply_labels(const std::vector<int> & labels)
{
    assert(labels.size() == this->num_edges());
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).label = labels.at(eid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::edge_apply_label(const int label)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).label = label;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_apply_labels(const std::vector<int> & labels)
{
    assert(labels.size() == this->num_verts());
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).label = labels.at(vid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::vert_apply_label(const int label)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).label = label;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractMesh<M,V,E,P>::edge_mark_sharp_creases(const float thresh)
{
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if(edge_dihedral_angle(eid) >= thresh)
        {
            this->edge_data(eid).flags[CREASE] = true;
            this->edge_data(eid).flags[MARKED] = true;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::pick_vert(const vec3d & p) const
{
    std::vector<std::pair<double,uint>> closest;
    for(uint vid=0; vid<this->num_verts(); ++vid) closest.push_back(std::make_pair(this->vert(vid).dist(p),vid));
    std::sort(closest.begin(), closest.end());
    return closest.front().second;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::pick_edge(const vec3d & p) const
{
    std::vector<std::pair<double,uint>> closest;
    for(uint eid=0; eid<this->num_edges(); ++eid) closest.push_back(std::make_pair(this->edge_sample_at(eid, 0.5).dist(p),eid));
    std::sort(closest.begin(), closest.end());
    return closest.front().second;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractMesh<M,V,E,P>::pick_poly(const vec3d & p) const
{
    std::vector<std::pair<double,uint>> closest;
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        if(!this->poly_data(pid).flags[HIDDEN]) closest.push_back(std::make_pair(this->poly_centroid(pid).dist(p),pid));
    }
    std::sort(closest.begin(), closest.end());
    return closest.front().second;
}

}
