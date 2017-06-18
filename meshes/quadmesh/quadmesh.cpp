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
#include <cinolib/meshes/quadmesh/quadmesh.h>
#include <cinolib/bfs.h>
#include <cinolib/timer.h>
#include <cinolib/io/read_write.h>
#include <cinolib/geometry/plane.h>

#include <queue>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Quadmesh<M,V,E,F>::Quadmesh(const char * filename)
{
    load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Quadmesh<M,V,E,F>::Quadmesh(const std::vector<vec3d> & verts,
                            const std::vector<uint>  & faces)
: verts(verts)
, faces(faces)
{
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Quadmesh<M,V,E,F>::Quadmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->faces = faces;

    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::load(const char * filename)
{
    timer_start("Load Quadmesh");

    clear();
    std::vector<double> coords;
    std::vector<uint>   tris; // unused

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare(".off") == 0 ||
        filetype.compare(".OFF") == 0)
    {
        read_OFF(filename, coords, tris, faces);
    }
    else if (filetype.compare(".obj") == 0 ||
             filetype.compare(".OBJ") == 0)
    {
        read_OBJ(filename, coords, tris, faces);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    verts = vec3d_from_serialized_xyz(coords);

    logger << num_faces() << " quads read" << endl;
    logger << num_verts() << " verts read" << endl;

    this->mesh_data().filename = std::string(filename);

    timer_stop("Load Quadmesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::save(const char * filename) const
{
    timer_start("Save Quadmesh");

    std::vector<double> coords = serialized_xyz_from_vec3d(verts);
    std::vector<uint>   tris; // unused

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        write_OFF(filename, coords, tris, faces);
    }
    else if (filetype.compare(".obj") == 0 ||
             filetype.compare(".OBJ") == 0)
    {
        write_OBJ(filename, coords, tris, faces);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << endl;
        exit(-1);
    }

    timer_stop("Save Quadmesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::clear()
{
    bb.reset();
    //
    verts.clear();
    edges.clear();
    faces.clear();
    //
    M std_M_data;
    m_data = std_M_data;
    v_data.clear();
    e_data.clear();
    f_data.clear();
    //
    v2v.clear();
    v2e.clear();
    v2f.clear();
    e2f.clear();
    f2e.clear();
    f2f.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::init()
{
    update_face_tessellation();
    update_adjacency();
    update_bbox();

    v_data.resize(num_verts());
    e_data.resize(num_edges());
    f_data.resize(num_faces());

    update_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::update_face_tessellation()
{
    tessellated_faces.resize(num_faces());

    for(uint fid=0; fid<num_faces(); ++fid)
    {
        uint vid0 = face_vert_id(fid,0);
        uint vid1 = face_vert_id(fid,1);
        uint vid2 = face_vert_id(fid,2);
        uint vid3 = face_vert_id(fid,3);

        vec3d n1 = (vert(vid1)-vert(vid0)).cross(vert(vid2)-vert(vid0));
        vec3d n2 = (vert(vid2)-vert(vid0)).cross(vert(vid3)-vert(vid0));

        bool flip = (n1.dot(n2) < 0); // flip diag: t(0,1,2) t(0,2,3) => t(0,1,3) t(1,2,3)

        tessellated_faces.at(fid).push_back(vid0);
        tessellated_faces.at(fid).push_back(vid1);
        tessellated_faces.at(fid).push_back(flip ? vid3 : vid2);
        tessellated_faces.at(fid).push_back(flip ? vid1 : vid0);
        tessellated_faces.at(fid).push_back(vid2);
        tessellated_faces.at(fid).push_back(vid3);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::update_adjacency()
{
    timer_start("Build adjacency");

    v2v.clear(); v2v.resize(num_verts());
    v2e.clear(); v2e.resize(num_verts());
    v2f.clear(); v2f.resize(num_verts());
    f2f.clear(); f2f.resize(num_faces());
    f2e.clear(); f2e.resize(num_faces());

    std::map<ipair,std::vector<uint>> e2f_map;
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        for(uint off=0; off<verts_per_face(); ++off)
        {
            uint vid0 = face_vert_id(fid,off);
            uint vid1 = face_vert_id(fid,(off+1)%verts_per_face());
            v2f.at(vid0).push_back(fid);
            e2f_map[unique_pair(vid0,vid1)].push_back(fid);
        }
    }

    edges.clear();
    e2f.clear();
    e2f.resize(e2f_map.size());

    uint fresh_id = 0;
    for(auto e2f_it : e2f_map)
    {
        ipair e    = e2f_it.first;
        uint  eid  = fresh_id++;
        uint  vid0 = e.first;
        uint  vid1 = e.second;

        edges.push_back(vid0);
        edges.push_back(vid1);

        v2v.at(vid0).push_back(vid1);
        v2v.at(vid1).push_back(vid0);

        v2e.at(vid0).push_back(eid);
        v2e.at(vid1).push_back(eid);

        std::vector<uint> fids = e2f_it.second;
        for(uint fid : fids)
        {
            f2e.at(fid).push_back(eid);
            e2f.at(eid).push_back(fid);
            for(uint adj_fid : fids) if (fid != adj_fid) f2f.at(fid).push_back(adj_fid);
        }

        // MANIFOLDNESS CHECKS
        //
        bool is_manifold = (fids.size() > 2 || fids.size() < 1);
        if (is_manifold && !support_non_manifold_edges)
        {
            std::cerr << "Non manifold edge found! To support non manifoldness,";
            std::cerr << "enable the 'support_non_manifold_edges' flag in cinolib.h" << endl;
            assert(false);
        }
        if (is_manifold && print_non_manifold_edges)
        {
            std::cerr << "Non manifold edge! (" << vid0 << "," << vid1 << ")" << endl;
        }
    }

    logger << num_verts() << "\tverts" << endl;
    logger << num_faces() << "\tfaces" << endl;
    logger << num_edges() << "\tedges" << endl;

    timer_stop("Build adjacency");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::update_bbox()
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
std::vector<double> Quadmesh<M,V,E,F>::vector_coords() const
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
void Quadmesh<M,V,E,F>::update_f_normals()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        // compute the best fitting plane
        std::vector<vec3d> points;
        for(uint off=0; off<verts_per_face(); ++off) points.push_back(face_vert(fid,off));
        Plane best_fit(points);

        // adjust orientation (n or -n?)
        assert(tessellated_faces.at(fid).size()>2);
        vec3d v0 = vert(tessellated_faces.at(fid).at(0));
        vec3d v1 = vert(tessellated_faces.at(fid).at(1));
        vec3d v2 = vert(tessellated_faces.at(fid).at(2));
        vec3d n  = (v1-v0).cross(v2-v0);

        face_data(fid).normal = (best_fit.n.dot(n) < 0) ? -best_fit.n : best_fit.n;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::update_v_normals()
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vec3d n(0,0,0);
        for(uint fid : adj_v2f(vid))
        {
            n += face_data(fid).normal;
        }
        n.normalize();
        vert_data(vid).normal = n;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::update_normals()
{
    update_f_normals();
    update_v_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Quadmesh<M,V,E,F>::face_vert_id(const uint fid, const uint offset) const
{
    uint fid_ptr = fid * verts_per_face();
    return faces.at(fid_ptr + offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Quadmesh<M,V,E,F>::face_vert(const uint fid, const uint offset) const
{
    return vert(face_vert_id(fid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Quadmesh<M,V,E,F>::face_centroid(const uint fid) const
{
    vec3d c(0,0,0);
    for(uint off=0; off<verts_per_face(); ++off)
    {
        c += face_vert(fid,off);
    }
    c /= static_cast<double>(verts_per_face());
    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Quadmesh<M,V,E,F>::face_edge_id(const uint fid, const uint vid0, const uint vid1) const
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
vec3d Quadmesh<M,V,E,F>::elem_centroid(const uint fid) const
{
    return face_centroid(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Quadmesh<M,V,E,F>::edge_vert_id(const uint eid, const uint offset) const
{
    uint   eid_ptr = eid * 2;
    return edges.at(eid_ptr + offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::edge_contains_vert(const uint eid, const uint vid) const
{
    if (edge_vert_id(eid,0) == vid) return true;
    if (edge_vert_id(eid,1) == vid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Quadmesh<M,V,E,F>::edge_vert(const uint eid, const uint offset) const
{
    return vert(edge_vert_id(eid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::edge_is_manifold(const uint eid) const
{
    return (adj_e2f(eid).size() <= 2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::edge_is_boundary(const uint eid) const
{
    return (adj_e2f(eid).size() < 2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::elem_show_all()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).visible = true;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::face_contains_vert(const uint fid, const uint vid) const
{
    for(uint off=0; off<verts_per_face(); ++off)
    {
        if (face_vert_id(fid,off) == vid) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::verts_are_adjacent(const uint vid0, const uint vid1) const
{
    for(uint nbr : adj_v2v(vid0)) if (nbr==vid1) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::vert_is_boundary(const uint vid) const
{
    for(uint eid : adj_v2e(vid)) if (edge_is_boundary(eid)) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Quadmesh<M,V,E,F>::vert_valence(const uint vid) const
{
    return adj_v2v(vid).size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::vert_is_singular(const uint vid) const
{
    return (vert_valence(vid)!=4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::vert_is_regular(const uint vid) const
{
    return (vert_valence(vid)==4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::vert_is_local_min(const uint vid, const int tex_coord) const
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
bool Quadmesh<M,V,E,F>::vert_is_local_max(const uint vid, const int tex_coord) const
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
bool Quadmesh<M,V,E,F>::vert_is_saddle(const uint vid, const int tex_coord) const
{
    std::vector<bool> signs;
    for(uint nbr : vert_ordered_vert_ring(vid))
    {
        // Discard == signs. For references, see:
        // Decomposing Polygon Meshes by Means of Critical Points
        // Yinan Zhou and Zhiyong Huang
        //
        switch (tex_coord)
        {
            case U_param : if (vert_data(nbr).uvw[0] != vert_data(vid).uvw[0]) signs.push_back(vert_data(nbr).uvw[0] > vert_data(vid).uvw[0]); break;
            case V_param : if (vert_data(nbr).uvw[1] != vert_data(vid).uvw[1]) signs.push_back(vert_data(nbr).uvw[1] > vert_data(vid).uvw[1]); break;
            case W_param : if (vert_data(nbr).uvw[2] != vert_data(vid).uvw[2]) signs.push_back(vert_data(nbr).uvw[2] > vert_data(vid).uvw[2]); break;
            default: assert(false);
        }
    }

    uint sign_switch = 0;
    for(uint i=0; i<signs.size(); ++i)
    {
        if (signs.at(i) != signs.at((i+1)%signs.size())) ++sign_switch;
    }

    if (sign_switch > 2) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::vert_is_critical_p(const uint vid, const int tex_coord) const
{
    return (vert_is_local_max(vid,tex_coord) ||
            vert_is_local_min(vid,tex_coord) ||
            vert_is_saddle   (vid, tex_coord));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::vert_set_color(const Color & c)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::vert_set_alpha(const float alpha)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,F>::vert_boundary_edges(const uint vid) const
{
    std::vector<uint> b_edges;
    for(uint eid : adj_v2e(vid)) if (edge_is_boundary(eid)) b_edges.push_back(eid);
    return b_edges;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::verts_are_ordered_CCW(const uint fid, const uint curr, const uint prev) const
{
    uint prev_offset = face_vert_offset(fid, prev);
    uint curr_offset = face_vert_offset(fid, curr);
    if (curr_offset == (prev_offset+1)%verts_per_face()) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::vert_ordered_one_ring(const uint vid,
                                              std::vector<uint> & v_ring,       // sorted list of adjacent vertices
                                              std::vector<uint> & f_ring,       // sorted list of adjacent quads
                                              std::vector<uint> & e_ring,       // sorted list of edges incident to vid
                                              std::vector<uint> & e_link) const // sorted list of edges opposite to vid
{
    v_ring.clear();
    f_ring.clear();
    e_ring.clear();
    e_link.clear();

    if (adj_v2e(vid).empty()) return;
    uint curr_e  = adj_v2e(vid).front(); assert(edge_is_manifold(curr_e));
    uint curr_v  = vert_opposite_to(curr_e, vid);
    uint curr_f  = adj_e2f(curr_e).front();
    // impose CCW winding...
    if (!verts_are_ordered_CCW(curr_f, curr_v, vid)) curr_f = adj_e2f(curr_e).back();

    // If there are boundary edges it is important to start from the right triangle (i.e. right-most),
    // otherwise it will be impossible to cover the entire umbrella
    std::vector<uint> b_edges = vert_boundary_edges(vid);
    if (b_edges.size()  > 0)
    {
        assert(b_edges.size() == 2); // otherwise there is no way to cover the whole umbrella walking through adjacent triangles!!!

        uint e = b_edges.front();
        uint f = adj_e2f(e).front();
        uint v = vert_opposite_to(e, vid);

        if (!verts_are_ordered_CCW(f, v, vid))
        {
            e = b_edges.back();
            f = adj_e2f(e).front();
            v = vert_opposite_to(e, vid);
            assert(verts_are_ordered_CCW(f, v, vid));
        }

        curr_e = e;
        curr_f = f;
        curr_v = v;
    }

    do
    {
        e_ring.push_back(curr_e);
        f_ring.push_back(curr_f);

        uint off = face_vert_offset(curr_f, curr_v);
        for(uint i=0; i<verts_per_face()-1; ++i)
        {
            curr_v = face_vert_id(curr_f,(off+i)%verts_per_face());
            if (i>0) e_link.push_back( face_edge_id(curr_f, curr_v, v_ring.back()) );
            v_ring.push_back(curr_v);
        }

        curr_e = face_edge_id(curr_f, vid, v_ring.back()); assert(edge_is_manifold(curr_e));
        curr_f = (adj_e2f(curr_e).front() == curr_f) ? adj_e2f(curr_e).back() : adj_e2f(curr_e).front();

        v_ring.pop_back();

        if (edge_is_boundary(curr_e)) e_ring.push_back(curr_e);
    }
    while(e_ring.size() < adj_v2e(vid).size());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,F>::vert_ordered_vert_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent vertices
    std::vector<uint> f_ring; // sorted list of adjacent triangles
    std::vector<uint> e_ring; // sorted list of edges incident to vid
    std::vector<uint> e_link; // sorted list of edges opposite to vid
    vert_ordered_one_ring(vid, v_ring, f_ring, e_ring, e_link);
    return v_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,F>::vert_ordered_face_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent vertices
    std::vector<uint> f_ring; // sorted list of adjacent triangles
    std::vector<uint> e_ring; // sorted list of edges incident to vid
    std::vector<uint> e_link; // sorted list of edges opposite to vid
    vert_ordered_one_ring(vid, v_ring, f_ring, e_ring, e_link);
    return f_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,F>::vert_ordered_edge_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent vertices
    std::vector<uint> f_ring; // sorted list of adjacent triangles
    std::vector<uint> e_ring; // sorted list of edges incident to vid
    std::vector<uint> e_link; // sorted list of edges opposite to vid
    vert_ordered_one_ring(vid, v_ring, f_ring, e_ring, e_link);
    return e_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,F>::vert_ordered_edge_link(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent vertices
    std::vector<uint> f_ring; // sorted list of adjacent triangles
    std::vector<uint> e_ring; // sorted list of edges incident to vid
    std::vector<uint> e_link; // sorted list of edges opposite to vid
    vert_ordered_one_ring(vid, v_ring, f_ring, e_ring, e_link);
    return e_link;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,F>::vert_loop(const uint start, const uint next) const
{
    assert(verts_are_adjacent(start,next));
    assert(vert_is_regular(start)); // if there is a singularity along there will be no loop!

    uint curr = next;
    uint prev = start;

    std::vector<uint> loop;
    loop.push_back(prev);

    do
    {
        assert(vert_is_regular(curr));
        loop.push_back(curr);

        std::vector<uint> v_ring = vert_ordered_vert_ring(curr);
        auto it  = std::find(v_ring.begin(), v_ring.end(), prev); assert(it != v_ring.end());
        uint pos = std::distance(v_ring.begin(),it);

        prev = curr;
        curr = v_ring.at((pos+4)%v_ring.size());

        assert(loop.size() < num_verts()); // sanity check for inifnite loops...
    }
    while(curr != start);

    return loop;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::edge_set_color(const Color & c)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::edge_set_alpha(const float alpha)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Quadmesh<M,V,E,F>::face_vert_offset(const uint fid, const uint vid) const
{
    for(uint off=0; off<verts_per_face(); ++off)
    {
        if (face_vert_id(fid,off) == vid) return off;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::face_set_color(const Color & c)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::face_set_alpha(const float alpha)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<float> Quadmesh<M,V,E,F>::export_uvw_param(const int mode) const
{
    std::vector<float> uvw;
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
uint Quadmesh<M,V,E,F>::vert_opposite_to(const uint eid, const uint vid) const
{
    assert(edge_contains_vert(eid, vid));
    if (edge_vert_id(eid,0) != vid) return edge_vert_id(eid,0);
    else                            return edge_vert_id(eid,1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


}
