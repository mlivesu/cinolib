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
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/bfs.h>
#include <cinolib/timer.h>
#include <cinolib/io/read_write.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Trimesh<M,V,E,F>::Trimesh(const char * filename)
{
    load(filename);
    std::cout << "loaded" << std::endl;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Trimesh<M,V,E,F>::Trimesh(const std::vector<vec3d> & verts,
                          const std::vector<uint>  & faces)
: verts(verts)
, faces(faces)
{
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Trimesh<M,V,E,F>::Trimesh(const std::vector<double> & coords,
                          const std::vector<uint>   & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->faces = faces;

    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::load(const char * filename)
{
    timer_start("Load Tri");

    clear();
    std::vector<double> coords;
    std::vector<uint>   quads; // unused

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare(".off") == 0 ||
        filetype.compare(".OFF") == 0)
    {
        read_OFF(filename, coords, faces, quads);
    }
    else if (filetype.compare(".obj") == 0 ||
             filetype.compare(".OBJ") == 0)
    {
        read_OBJ(filename, coords, faces, quads);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    verts = vec3d_from_serialized_xyz(coords);

    this->mesh_data().filename = std::string(filename);

    timer_stop("Load Tri");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::save(const char * filename) const
{
    timer_start("Save Tri");

    std::vector<double> coords = serialized_xyz_from_vec3d(verts);
    std::vector<uint>   quads; // unused

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        write_OFF(filename, coords, faces, quads);
    }
    else if (filetype.compare(".obj") == 0 ||
             filetype.compare(".OBJ") == 0)
    {
        write_OBJ(filename, coords, faces, quads);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << endl;
        exit(-1);
    }

    timer_stop("Save Tri");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::clear()
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
void Trimesh<M,V,E,F>::init()
{
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
void Trimesh<M,V,E,F>::update_adjacency()
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
        for(uint offset=0; offset<verts_per_face(); ++offset)
        {
            int  vid0 = face_vert_id(fid,offset);
            int  vid1 = face_vert_id(fid,(offset+1)%verts_per_face());
            v2f.at(vid0).push_back(fid);
            e2f_map[unique_pair(vid0,vid1)].push_back(fid);
        }
    }

    edges.clear();
    e2f.clear();
    e2f.resize(e2f_map.size());

    int fresh_id = 0;
    for(auto e2f_it : e2f_map)
    {
        ipair e    = e2f_it.first;
        int   eid  = fresh_id++;
        int   vid0 = e.first;
        int   vid1 = e.second;

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
void Trimesh<M,V,E,F>::update_bbox()
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
std::vector<double> Trimesh<M,V,E,F>::vector_coords() const
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
void Trimesh<M,V,E,F>::update_f_normal(const uint fid)
{
    vec3d A = face_vert(fid,0);
    vec3d B = face_vert(fid,1);
    vec3d C = face_vert(fid,2);
    vec3d n = (B-A).cross(C-A);
    n.normalize();
    face_data(fid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::update_f_normals()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        update_f_normal(fid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::update_v_normal(const uint vid)
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
void Trimesh<M,V,E,F>::update_v_normals()
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        update_v_normal(vid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::update_normals()
{
    update_f_normals();
    update_v_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::normalize_area()
{
    double area = 0.0;
    for(uint fid=0; fid<num_faces(); ++fid) area += elem_mass(fid);
    area = std::max(1e-4,area); // avoid creating degenerate faces...
    double s = 1.0 / sqrt(area);
    for(uint vid=0; vid<num_verts(); ++vid) vert(vid) *= s;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::center_bbox()
{
    update_bbox();
    vec3d center = bb.center();
    for(uint vid=0; vid<num_verts(); ++vid) vert(vid) -= center;
    bb.min -= center;
    bb.max -= center;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::face_vert_id(const uint fid, const uint offset) const
{
    uint face_ptr = verts_per_face() * fid;
    return faces.at(face_ptr+offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Trimesh<M,V,E,F>::face_vert(const uint fid, const uint offset) const
{
    return vert(face_vert_id(fid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Trimesh<M,V,E,F>::face_centroid(const uint fid) const
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
vec3d Trimesh<M,V,E,F>::elem_centroid(const uint fid) const
{
    return face_centroid(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::edge_vert_id(const uint eid, const uint offset) const
{
    uint   eid_ptr = eid * 2;
    return edges.at(eid_ptr + offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Trimesh<M,V,E,F>::edge_vert(const uint eid, const uint offset) const
{
    return vert(edge_vert_id(eid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::elem_show_all()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).visible = true;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::face_area(const uint fid) const
{
    vec3d P = face_vert(fid, 0);
    vec3d u = face_vert(fid, 1) - P;
    vec3d v = face_vert(fid, 2) - P;
    double area = 0.5 * u.cross(v).length();
    return area;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::face_mass(const uint fid) const
{
    return face_area(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::elem_mass(const uint fid) const
{
    return face_area(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::vert_set_color(const Color & c)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::vert_set_alpha(const float alpha)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::edge_set_color(const Color & c)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::edge_set_alpha(const float alpha)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::face_set_color(const Color & c)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::face_set_alpha(const float alpha)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::operator+=(const Trimesh<M,V,E,F> & m)
{
    uint nv = num_verts();
    uint nf = num_faces();
    uint ne = num_edges();

    std::vector<uint> tmp;
    for(uint fid=0; fid<m.num_faces(); ++fid)
    {
        faces.push_back(nv + m.face_vert_id(fid,0));
        faces.push_back(nv + m.face_vert_id(fid,1));
        faces.push_back(nv + m.face_vert_id(fid,2));

        f_data.push_back(m.face_data(fid));

        tmp.clear();
        for(uint eid : m.f2e.at(fid)) tmp.push_back(ne + eid);
        f2e.push_back(tmp);

        tmp.clear();
        for(uint nbr : m.f2f.at(fid)) tmp.push_back(nf + nbr);
        f2f.push_back(tmp);
    }
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        edges.push_back(nv + m.edge_vert_id(eid,0));
        edges.push_back(nv + m.edge_vert_id(eid,1));

        e_data.push_back(m.edge_data(eid));

        tmp.clear();
        for(uint tid : m.e2f.at(eid)) tmp.push_back(nf + tid);
        e2f.push_back(tmp);
    }
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        verts.push_back(m.vert(vid));
        v_data.push_back(m.vert_data(vid));

        tmp.clear();
        for(uint eid : m.v2e.at(vid)) tmp.push_back(ne + eid);
        v2e.push_back(tmp);

        tmp.clear();
        for(uint tid : m.v2f.at(vid)) tmp.push_back(nf + tid);
        v2f.push_back(tmp);

        tmp.clear();
        for(uint nbr : m.v2v.at(vid)) tmp.push_back(nv + nbr);
        v2v.push_back(tmp);
    }

    update_bbox();

    logger << "Appended " << m.mesh_data().filename << " to mesh " << mesh_data().filename << endl;
    logger << num_faces() << " faces" << endl;
    logger << num_verts() << " verts" << endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Trimesh<M,V,E,F>::centroid() const
{
    vec3d bary(0,0,0);
    for(auto p : verts) bary += p;
    if (num_verts() > 0) bary/=static_cast<double>(num_verts());
    return bary;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::translate(const vec3d & delta)
{
    for(uint vid=0; vid<num_verts(); ++vid) vert(vid) += delta;
    update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::rotate(const vec3d & axis, const double angle)
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

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::connected_components() const
{
    std::vector<std::set<uint>> ccs;
    return connected_components(ccs);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::connected_components(std::vector<std::set<uint>> &) const
{
//    ccs.clear();
//    uint seed = 0;
//    std::vector<bool> visited(num_verts(), false);

//    do
//    {
//        std::set<uint> cc;
//        bfs_exahustive<Trimesh<M,V,E,F>>(*this, seed, cc);

//        ccs.push_back(cc);
//        for(uint vid : cc) visited.at(vid) = true;

//        seed = 0;
//        while (seed < num_verts() && visited.at(seed)) ++seed;
//    }
//    while (seed < num_verts());

//    return ccs.size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
int Trimesh<M,V,E,F>::edge_opposite_to(const uint fid, const uint vid) const
{
    assert(face_contains_vert(fid, vid));
    for(uint eid : adj_f2e(fid))
    {
        if (edge_vert_id(eid,0) != vid &&
            edge_vert_id(eid,1) != vid) return eid;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::edge_contains_vert(const uint eid, const uint vid) const
{
    if (edge_vert_id(eid,0) == vid) return true;
    if (edge_vert_id(eid,1) == vid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::edge_is_manifold(const uint eid) const
{
    return (adj_e2f(eid).size() <= 2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::edge_is_boundary(const uint eid) const
{
    return (adj_e2f(eid).size() < 2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::edges_share_face(const uint eid1, const uint eid2) const
{
    for(uint fid1 : adj_e2f(eid1))
    for(uint fid2 : adj_e2f(eid2))
    {
        if (fid1 == fid2) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
ipair Trimesh<M,V,E,F>::edge_shared(const uint fid0, const uint fid1) const
{
    std::vector<uint> shared_verts;
    uint v0 = face_vert_id(fid0,0);
    uint v1 = face_vert_id(fid0,1);
    uint v2 = face_vert_id(fid0,2);

    if (face_contains_vert(fid1,v0)) shared_verts.push_back(v0);
    if (face_contains_vert(fid1,v1)) shared_verts.push_back(v1);
    if (face_contains_vert(fid1,v2)) shared_verts.push_back(v2);
    assert(shared_verts.size() == 2);

    ipair e;
    e.first  = shared_verts.front();
    e.second = shared_verts.back();
    return e;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::edge_length(const uint eid) const
{
    return (edge_vert(eid,0) - edge_vert(eid,1)).length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::edge_avg_length() const
{
    double avg = 0;
    for(uint eid=0; eid<num_edges(); ++eid) avg += edge_length(eid);
    if (num_edges() > 0) avg/=static_cast<double>(num_edges());
    return avg;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::edge_max_length() const
{
    double max = 0;
    for(uint eid=0; eid<num_edges(); ++eid) max = std::max(max, edge_length(eid));
    return max;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::edge_min_length() const
{
    double min = 0;
    for(uint eid=0; eid<num_edges(); ++eid) min = std::min(min,edge_length(eid));
    return min;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::face_contains_vert(const uint fid, const uint vid) const
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
std::vector<ipair> Trimesh<M,V,E,F>::get_boundary_edges() const
{
    std::vector<ipair> res;
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        if (edge_is_boundary(eid))
        {
            ipair e;
            e.first  = edge_vert_id(eid,0);
            e.second = edge_vert_id(eid,1);
            res.push_back(e);
        }
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,F>::get_boundary_vertices() const
{
    std::vector<uint> res;
    for(uint vid=0; vid<num_verts(); ++vid) if (vert_is_boundary(vid)) res.push_back(vid);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::edge_collapse(const uint eid)
{
    // define what to keep and what to remove
    //
    uint vid_keep   = edge_vert_id(eid,0);
    uint vid_remove = edge_vert_id(eid,1);
    std::set<uint> fid_remove(adj_e2f(eid).begin(), adj_e2f(eid).end());
    std::set<uint> edg_keep, edg_remove;
    for(uint fid: fid_remove)
    {
        edg_keep.insert(edge_opposite_to(fid, vid_remove));
        edg_remove.insert(edge_opposite_to(fid, vid_keep));
    }
    edg_remove.insert(eid);

    // Pre-processing : check edges
    //
    for(uint eid : adj_v2e(vid_remove))
    {
        if (CONTAINS(edg_remove,eid)) continue;

        uint vid0 = edge_vert_id(eid,0);
        uint vid1 = edge_vert_id(eid,1);
        //
        uint vid0_mod = vid0;
        uint vid1_mod = vid1;
        //
        if (vid0 == vid_remove) vid0_mod = vid_keep; else
        if (vid1 == vid_remove) vid1_mod = vid_keep; else
        assert("Something is off here" && false);

        // check edge
        for(uint eid2=0; eid2<num_edges(); ++eid2)
        {
            if (eid2==eid) continue;

            if ((vid0_mod == vid0 && vid1_mod == vid1) ||
                (vid0_mod == vid1 && vid1_mod == vid0))
            {
                logger << "WARNING : duplicared edge " << eid << " : Impossible to perform edge collapse. " << endl;
                return false;
            }
        }
    }

    // Pre-processing : check triangles
    //
    for(uint fid : adj_v2f(vid_remove))
    {
        if (CONTAINS(fid_remove, fid)) continue;

        vec3d n_old = face_data(fid).normal;
        uint vid0   = face_vert_id(fid,0);
        uint vid1   = face_vert_id(fid,1);
        uint vid2   = face_vert_id(fid,2);

        if (vid0 == vid_remove) vid0 = vid_keep; else
        if (vid1 == vid_remove) vid1 = vid_keep; else
        if (vid2 == vid_remove) vid2 = vid_keep; else
        assert("Something is off here" && false);

        // check triangle flip
        vec3d v0 = vert(vid0);
        vec3d v1 = vert(vid1);
        vec3d v2 = vert(vid2);
        //
        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        if (n.length() == 0)
        {
            logger << "WARNING : triangle on colinear points " << fid << " : Impossible to perform edge collapse. " << endl;
            return false;
        }

        if (n.dot(n_old) < 0 ) // triangle inversion
        {
            logger << "WARNING : triangle inversion " << fid << " : Impossible to perform edge collapse. " << std::endl;
            return false;
        }
    }

    // Everything is ok
    // The edge can be collapsed

    for(uint eid : adj_v2e(vid_remove))
    {
        if (CONTAINS(edg_remove, eid)) continue;

        v2e.at(vid_keep).push_back(eid);
        if (edge_vert_id(eid,0) == vid_remove) edges.at(eid*2+0) = vid_keep; else
        if (edge_vert_id(eid,1) == vid_remove) edges.at(eid*2+1) = vid_keep; else
        assert("Something is off here" && false);
    }
    //
    for(uint fid : adj_v2f(vid_remove))
    {
        if (CONTAINS(fid_remove, fid)) continue;

        v2f.at(vid_keep).push_back(fid);
        if (face_vert_id(fid,0) == vid_remove) faces.at(fid*3+0) = vid_keep; else
        if (face_vert_id(fid,0) == vid_remove) faces.at(fid*3+1) = vid_keep; else
        if (face_vert_id(fid,0) == vid_remove) faces.at(fid*3+2) = vid_keep; else
        assert("Something is off here" && false);

        update_f_normal(fid);
    }

    // Migrate references from edge_remove to edge_keep
    //
    for(uint fid : fid_remove)
    {
        int e_take = edge_opposite_to(fid, vid_remove); assert(e_take >= 0);
        int e_give = edge_opposite_to(fid, vid_keep);   assert(e_give >= 0);
        assert(CONTAINS(edg_remove, e_give));

        for(uint inc_f : adj_e2f(e_give))
        {
            if (CONTAINS(fid_remove, inc_f)) continue;

            for (uint adj_f : adj_e2f(e_take))
            {
                if (CONTAINS(fid_remove, adj_f)) continue;

                f2f.at(inc_f).push_back(adj_f);
                f2f.at(adj_f).push_back(inc_f);
            }

            e2f.at(e_take).push_back(inc_f);
            f2e.at(inc_f).push_back(e_take);
        }
    }

    // remove references to vid_remove
    //
    for(uint vid : adj_v2v(vid_remove))
    {
        assert(vid!=vid_remove && vid<num_verts());

        auto beg = v2v.at(vid).begin();
        auto end = v2v.at(vid).end();
        v2v.at(vid).erase(std::remove(beg, end, vid_remove), end); // Erase-Remove idiom

        if (vid == vid_keep) continue;
        if (!verts_are_adjacent(vid_keep,vid))
        {
            v2v.at(vid_keep).push_back(vid);
            v2v.at(vid).push_back(vid_keep);
        }
    }
    //
    // remove references to any edge in edg_remove.
    //
    for(uint edg_rem : edg_remove)
    {
        assert(edg_rem<num_edges());

        for(uint fid : adj_e2f(edg_rem))
        {
            assert(fid<num_faces());
            auto beg = f2e.at(fid).begin();
            auto end = f2e.at(fid).end();
            f2e.at(fid).erase(std::remove(beg, end, edg_rem), end); // Erase-Remove idiom
        }

        for(uint i=0; i<2; ++i)
        {
            uint vid = edge_vert_id(edg_rem, i);
            auto beg = v2e.at(vid).begin();
            auto end = v2e.at(vid).end();
            v2e.at(vid).erase(std::remove(beg, end, edg_rem), end); // Erase-Remove idiom
        }
    }
    //
    // remove references to any triangle in tri_remove.
    //
    for(uint fid_rem : fid_remove)
    {
        for(uint off=0; off<3; ++off)
        {
            uint vid = face_vert_id(fid_rem,off);
            auto beg = v2f.at(vid).begin();
            auto end = v2f.at(vid).end();
            v2f.at(vid).erase(std::remove(beg, end, fid_rem), end); // Erase-Remove idiom
            update_v_normal(vid);
        }

        for(uint eid : adj_f2e(fid_rem))
        {
            auto beg = e2f.at(eid).begin();
            auto end = e2f.at(eid).end();
            e2f.at(eid).erase(std::remove(beg, end, fid_rem), end); // Erase-Remove idiom
        }

        for(uint fid : adj_f2f(fid_rem))
        {
            auto beg = f2f.at(fid).begin();
            auto end = f2f.at(fid).end();
            f2f.at(fid).erase(std::remove(beg, end, fid_rem), end); // Erase-Remove idiom
        }
    }

    // clear
    v2v.at(vid_remove).clear();
    v2e.at(vid_remove).clear();
    v2f.at(vid_remove).clear();

    for(uint eid : edg_remove)
    {
        e2f.at(eid).clear();
        edges.at(eid*2+0) = INT_MAX;
        edges.at(eid*2+1) = INT_MAX;
    }

    for(uint fid : fid_remove)
    {
        f2e.at(fid).clear();
        f2f.at(fid).clear();
        faces.at(fid*3+0) = INT_MAX;
        faces.at(fid*3+1) = INT_MAX;
        faces.at(fid*3+2) = INT_MAX;
    }

    // Finalize

    vert_remove_unreferenced(vid_remove);

    std::vector<uint> edg_remove_vec(edg_remove.begin(), edg_remove.end());
    std::sort(edg_remove_vec.begin(), edg_remove_vec.end());
    std::reverse(edg_remove_vec.begin(), edg_remove_vec.end());
    for(uint eid : edg_remove_vec) edge_remove_unreferenced(eid);

    std::vector<uint> fid_remove_vec(fid_remove.begin(), fid_remove.end());
    std::sort(fid_remove_vec.begin(), fid_remove_vec.end());
    std::reverse(fid_remove_vec.begin(), fid_remove_vec.end());
    for(uint tid : fid_remove_vec) face_remove_unreferenced(tid);

    update_normals();

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::edge_switch_id(const uint eid0, const uint eid1)
{
    for(uint off=0; off<2; ++off) std::swap(edges.at(2*eid0+off), edges.at(2*eid1+off));

    std::swap(e2f.at(eid0), e2f.at(eid1));

    for(std::vector<uint> & nbrs : v2e)
    for(uint & curr : nbrs)
    {
        if (curr == eid0) curr = eid1; else
        if (curr == eid1) curr = eid0;
    }

    for(std::vector<uint> & nbrs : f2e)
    for(uint & curr : nbrs)
    {
        if (curr == eid0) curr = eid1; else
        if (curr == eid1) curr = eid0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::edge_remove_unreferenced(const uint eid)
{
    edge_switch_id(eid, num_edges()-1);
    edges.resize(edges.size()-2);
    e_data.pop_back();
    e2f.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::face_bary_coords(const uint fid, const vec3d & P, std::vector<double> & wgts) const
{
    return triangle_barycentric_coords(face_vert(fid,0),
                                       face_vert(fid,1),
                                       face_vert(fid,2),
                                       P, wgts);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::face_bary_is_vert(const uint fid, const std::vector<double> & wgts, uint & vid, const double tol) const
{
    uint off;
    if (triangle_bary_is_vertex(wgts, off, tol))
    {
        vid = face_vert_id(fid, off);
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::elem_bary_is_vert(const uint fid, const std::vector<double> & wgts, uint & vid, const double tol) const
{
    return face_bary_is_vert(fid, wgts, vid, tol);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::face_bary_is_edge(const uint fid, const std::vector<double> & wgts, uint & eid, const double tol) const
{
    uint off;
    if (triangle_bary_is_edge(wgts, off, tol))
    {
        eid = face_edge_id(fid, off);
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::elem_bary_is_edge(const uint fid, const std::vector<double> & wgts, uint & eid, const double tol) const
{
    return face_bary_is_edge(fid, wgts, eid, tol);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::face_edge_id(const uint fid, const uint offset) const
{
    uint vid0 = face_vert_id(fid, TRI_EDGES[offset][0]);
    uint vid1 = face_vert_id(fid, TRI_EDGES[offset][1]);

    for(uint eid : adj_f2e(fid))
    {
        if (edge_contains_vert(eid,vid0) &&
            edge_contains_vert(eid,vid1))
        {
            return eid;
        }
    }
    assert(false && "Something is off here...");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::face_is_boundary(const uint fid) const
{
    return (adj_f2f(fid).size() < 3);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::face_vert_offset(const uint fid, const uint vid) const
{
    for(uint offset=0; offset<verts_per_face(); ++offset)
    {
        if (face_vert_id(fid,offset) == vid) return offset;
    }
    assert(false && "Something is off here...");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
int Trimesh<M,V,E,F>::face_shared(const uint eid0, const uint eid1) const
{
    for(uint fid0 : adj_e2f(eid0))
    for(uint fid1 : adj_e2f(eid1))
    {
        if (fid0 == fid1) return fid0;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
int Trimesh<M,V,E,F>::face_adjacent_along(const uint fid, const uint vid0, const uint vid1) const
{
    for(uint nbr : adj_f2f(fid))
    {
        if (face_contains_vert(nbr,vid0) &&
            face_contains_vert(nbr,vid1))
        {
            return nbr;
        }
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::face_angle_at_vert(const uint fid, const uint vid, const int unit) const
{
    uint offset;

         if (face_vert_id(fid,0) == vid) offset = 0;
    else if (face_vert_id(fid,1) == vid) offset = 1;
    else if (face_vert_id(fid,2) == vid) offset = 2;
    else { assert(false); offset=0; } // offset=0 kills uninitialized warning message

    vec3d P = vert(vid);
    vec3d u = face_vert(fid,(offset+1)%verts_per_face()) - P;
    vec3d v = face_vert(fid,(offset+2)%verts_per_face()) - P;

    switch (unit)
    {
        case RAD : return u.angle_rad(v);
        case DEG : return u.angle_deg(v);
        default  : assert(false);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// for a definition of cap and needles, see:
// A Robust Procedure to Eliminate Degenerate Faces from Triangle Meshes
// Mario Botsch, Leif P. Kobbelt
// VMW 2001
//
template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::face_is_cap(const uint fid, const double angle_thresh_deg) const
{
    for(uint offset=0; offset<verts_per_face(); ++offset)
    {
        uint vid = face_vert_id(fid,offset);
        if (face_angle_at_vert(fid,vid,DEG) > angle_thresh_deg)
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
template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::face_is_needle(const uint fid, const double angle_thresh_deg) const
{
    for(uint offset=0; offset<verts_per_face(); ++offset)
    {
        uint vid = face_vert_id(fid,offset);
        if (face_angle_at_vert(fid,vid,DEG) < angle_thresh_deg)
        {
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::face_add(const uint vid0, const uint vid1, const uint vid2, const F & data)
{
    assert(vid0 >= 0 && vid0 < num_verts());
    assert(vid1 >= 0 && vid1 < num_verts());
    assert(vid2 >= 0 && vid2 < num_verts());

    uint fid = num_faces();
    //
    faces.push_back(vid0);
    faces.push_back(vid1);
    faces.push_back(vid2);
    //
    f_data.push_back(data);
    //
    f2e.push_back(std::vector<uint>());
    f2f.push_back(std::vector<uint>());
    //
    v2f.at(vid0).push_back(fid);
    v2f.at(vid1).push_back(fid);
    v2f.at(vid2).push_back(fid);
    //
    ipair new_e[3]   = { unique_pair(vid0, vid1), unique_pair(vid1, vid2), unique_pair(vid2, vid0) };
    int   new_eid[3] = { -1, -1, -1 };
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        ipair e = unique_pair(edge_vert_id(eid,0), edge_vert_id(eid,1));
        for(uint i=0; i<3; ++i) if (e == new_e[i]) new_eid[i] = eid;
    }
    //
    for(uint i=0; i<3; ++i)
    {
        if (new_eid[i] == -1)
        {
            new_eid[i] = num_edges();
            edges.push_back(new_e[i].first);
            edges.push_back(new_e[i].second);
            e2f.push_back(std::vector<uint>());
        }
        //
        for(uint nbr : e2f.at(new_eid[i]))
        {
            f2f.at(nbr).push_back(fid);
            f2f.at(fid).push_back(nbr);
        }
        e2f.at(new_eid[i]).push_back(fid);
        f2e.at(fid).push_back(new_eid[i]);
    }
    //
    update_f_normal(fid);
    update_v_normal(vid0);
    update_v_normal(vid1);
    update_v_normal(vid2);

    return fid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::face_switch_id(const uint fid0, const uint fid1)
{
    for(uint offset=0; offset<3; ++offset)
    {
        std::swap(faces.at(3*fid0+offset),faces.at(3*fid1+offset));
    }
    std::swap(f_data.at(fid0), f_data.at(fid1));
    std::swap(f2e.at(fid0),    f2e.at(fid1));
    std::swap(f2f.at(fid0),    f2f.at(fid1));

    for(auto & nbrs : v2f)
    for(uint & curr : nbrs)
    {
        if (curr == fid0) curr = fid1; else
        if (curr == fid1) curr = fid0;
    }

    for(auto & nbrs : e2f)
    for(uint & curr : nbrs)
    {
        if (curr == fid0) curr = fid1; else
        if (curr == fid1) curr = fid0;
    }

    for(auto & nbrs : f2f)
    for(uint & curr : nbrs)
    {
        if (curr == fid0) curr = fid1; else
        if (curr == fid1) curr = fid0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::face_remove_unreferenced(const uint fid)
{
    face_switch_id(fid, num_faces()-1);
    faces.resize(faces.size()-3);
    f_data.pop_back();
    f2e.pop_back();
    f2f.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::set<uint> Trimesh<M,V,E,F>::vert_n_ring(const uint vid, const uint n) const
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
double Trimesh<M,V,E,F>::vert_area(const uint vid) const
{
    double area = 0.0;
    for(uint fid : adj_v2f(vid)) area += face_area(fid);
    area /= static_cast<double>(verts_per_face());
    return area;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Trimesh<M,V,E,F>::vert_mass(const uint vid) const
{
    return vert_area(vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::vert_opposite_to(const uint fid, const uint vid0, const uint vid1) const
{
    assert(face_contains_vert(fid, vid0));
    assert(face_contains_vert(fid, vid1));
    for(uint offset=0; offset<3; ++offset)
    {
        uint vid = face_vert_id(fid,offset);
        if (vid != vid0 && vid != vid1) return vid;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::vert_opposite_to(const uint eid, const uint vid) const
{
    assert(edge_contains_vert(eid, vid));
    if (edge_vert_id(eid,0) != vid) return edge_vert_id(eid,0);
    else                            return edge_vert_id(eid,1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::vert_is_boundary(const uint vid) const
{
    for(uint eid : adj_v2e(vid)) if (edge_is_boundary(eid)) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Trimesh<M,V,E,F>::verts_are_adjacent(const uint vid0, const uint vid1) const
{
    for(uint nbr : adj_v2v(vid0)) if (vid1==nbr) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::vert_valence(const uint vid) const
{
    return adj_v2v(vid).size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::vert_shared(const uint eid0, const uint eid1) const
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
bool Trimesh<M,V,E,F>::vert_is_local_min(const uint vid, const int tex_coord) const
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
bool Trimesh<M,V,E,F>::vert_is_local_max(const uint vid, const int tex_coord) const
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
bool Trimesh<M,V,E,F>::vert_is_critical_p(const uint vid) const
{
    // WARNING: does not handle saddle points!
    return (vert_is_local_max(vid) || vert_is_local_min(vid));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,F>::vert_boundary_edges(const uint vid) const
{
    std::vector<uint> b_edges;
    for(uint eid : adj_v2e(vid)) if (edge_is_boundary(eid)) b_edges.push_back(eid);
    return b_edges;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Trimesh<M,V,E,F>::vert_add(const vec3d & pos, const V & data)
{
    uint vid = num_verts();
    verts.push_back(pos);
    v_data.push_back(data);
    v2v.push_back(std::vector<uint>());
    v2e.push_back(std::vector<uint>());
    v2f.push_back(std::vector<uint>());
    bb.min = bb.min.min(pos);
    bb.max = bb.max.max(pos);
    return vid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::vert_switch_id(const uint vid0, const uint vid1)
{
    std::swap(verts.at(vid0),verts.at(vid1));
    std::swap(v2v.at(vid0),v2v.at(vid1));
    std::swap(v2e.at(vid0),v2e.at(vid1));
    std::swap(v2f.at(vid0),v2f.at(vid1));

    for(uint & curr : edges)
    {
        if (curr == vid0) curr = vid1; else
        if (curr == vid1) curr = vid0;
    }

    for(uint & curr : faces)
    {
        if (curr == vid0) curr = vid1; else
        if (curr == vid1) curr = vid0;
    }

    for(auto & nbrs : v2v)
    for(uint & curr : nbrs)
    {
        if (curr == vid0) curr = vid1; else
        if (curr == vid1) curr = vid0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::vert_remove_unreferenced(const uint vid)
{
    vert_switch_id(vid, num_verts()-1);
    verts.pop_back();
    v2v.pop_back();
    v2e.pop_back();
    v2f.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Trimesh<M,V,E,F>::vert_ordered_one_ring(const uint vid,
                                         std::vector<uint> & v_ring,       // sorted list of adjacent vertices
                                         std::vector<uint> & f_ring,       // sorted list of adjacent triangles
                                         std::vector<uint> & e_ring,       // sorted list of edges incident to vid
                                         std::vector<uint> & e_link) const // sorted list of edges opposite to vid
{
    v_ring.clear();
    f_ring.clear();
    e_ring.clear();
    e_link.clear();

    if (adj_v2f(vid).empty()) return;
    uint curr_f = adj_v2f(vid).front();

    // If there are boundary edges it is important to start from the right triangle (i.e. right-most),
    // otherwise it will be impossible to cover the entire umbrella
    std::vector<uint> b_edges = vert_boundary_edges(vid);
    if (b_edges.size()  > 0)
    {
        assert(b_edges.size() == 2); // otherwise there is no way to cover the whole umbrella walking through adjacent triangles!!!

        uint  eid    = b_edges.front();
              curr_f = adj_e2f(eid).front(); assert(adj_e2f(eid).size()==1);
        uint  v1     = vert_opposite_to(curr_f,edge_vert_id(eid,0), edge_vert_id(eid,1));
        uint  tmp    = edge_opposite_to(curr_f, vid);
        uint  v0     = (edge_vert_id(tmp,0)==v1) ? edge_vert_id(tmp,1) : edge_vert_id(tmp,0);

        vec3d u = vert(v0) - vert(vid);
        vec3d v = vert(v1) - vert(vid);
        vec3d n = vert_data(vid).normal;
        if (u.cross(v).dot(n) < 0) curr_f = adj_e2f(b_edges.back()).front();
    }

    uint curr_e  = edge_opposite_to(curr_f, vid);
    uint first_v = edge_vert_id(curr_e, 0);
    uint curr_v  = edge_vert_id(curr_e, 1);

    // impose CCW orientation...
    if ((vert(first_v)-vert(vid)).cross(vert(curr_v)-vert(vid)).dot(vert_data(vid).normal) < 0)
    {
        std::swap(first_v,curr_v);
    }

    v_ring.push_back(first_v);
    e_ring.push_back(edge_opposite_to(curr_f, curr_v));

    while(curr_v != first_v)
    {
        v_ring.push_back(curr_v);
        f_ring.push_back(curr_f);
        e_link.push_back(edge_opposite_to(curr_f, vid));

        curr_f = face_adjacent_along(curr_f, vid, curr_v);

        if (curr_f != -1)
        {
            curr_e = edge_opposite_to(curr_f, vid);
            curr_v = edge_vert_id(curr_e, 0) == curr_v ? edge_vert_id(curr_e, 1) : edge_vert_id(curr_e, 0);
            e_ring.push_back(edge_opposite_to(curr_f, curr_v));
        }
        else
        {
            e_ring.push_back(edge_opposite_to(f_ring.back(),v_ring.at(v_ring.size()-2)));
            first_v = curr_v; // force exit from while()
        }
    }

    if (curr_f != -1)
    {
        f_ring.push_back(curr_f);
        e_link.push_back(edge_opposite_to(curr_f, vid));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,F>::vert_ordered_vert_ring(const uint vid) const
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
std::vector<uint> Trimesh<M,V,E,F>::vert_ordered_face_ring(const uint vid) const
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
std::vector<uint> Trimesh<M,V,E,F>::vert_ordered_edge_ring(const uint vid) const
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
std::vector<uint> Trimesh<M,V,E,F>::vert_ordered_edge_link(const uint vid) const
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
std::vector<float> Trimesh<M,V,E,F>::export_uvw_param(const int mode) const
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


}
