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
#include <cinolib/meshes/abstract_surface_mesh.h>
#include <cinolib/timer.h>
#include <cinolib/io/read_write.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractSurfaceMesh<M,V,E,F>::load(const char * filename)
{
    timer_start("Load Mesh");

    this->clear();
    std::vector<double> coords;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare(".off") == 0 ||
        filetype.compare(".OFF") == 0)
    {
        read_OFF(filename, coords, this->faces);
    }
    else if (filetype.compare(".obj") == 0 ||
             filetype.compare(".OBJ") == 0)
    {
        read_OBJ(filename, coords, this->faces);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    this->verts = vec3d_from_serialized_xyz(coords);

    this->mesh_data().filename = std::string(filename);

    timer_stop("Load Mesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractSurfaceMesh<M,V,E,F>::save(const char * filename) const
{
    timer_start("Save Mesh");

    std::vector<double> coords = serialized_xyz_from_vec3d(this->verts);

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        write_OFF(filename, coords, this->faces);
    }
    else if (filetype.compare("obj") == 0 ||
             filetype.compare("OBJ") == 0)
    {
        write_OBJ(filename, coords, this->faces);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << endl;
        exit(-1);
    }

    timer_stop("Save Mesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractSurfaceMesh<M,V,E,F>::update_adjacency()
{
    timer_start("Build adjacency");

    this->v2v.clear(); this->v2v.resize(this->num_verts());
    this->v2e.clear(); this->v2e.resize(this->num_verts());
    this->v2f.clear(); this->v2f.resize(this->num_verts());
    this->f2f.clear(); this->f2f.resize(this->num_faces());
    this->f2e.clear(); this->f2e.resize(this->num_faces());

    std::map<ipair,std::vector<uint>> e2f_map;
    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        for(uint offset=0; offset<this->verts_per_face(fid); ++offset)
        {
            uint vid0 = this->face_vert_id(fid,offset);
            uint vid1 = this->face_vert_id(fid,(offset+1)%this->verts_per_face(fid));
            this->v2f.at(vid0).push_back(fid);
            e2f_map[unique_pair(vid0,vid1)].push_back(fid);
        }
    }

    this->edges.clear();
    this->e2f.clear();
    this->e2f.resize(e2f_map.size());

    uint fresh_id = 0;
    for(auto e2f_it : e2f_map)
    {
        ipair e    = e2f_it.first;
        uint  eid  = fresh_id++;
        uint  vid0 = e.first;
        uint  vid1 = e.second;

        this->edges.push_back(vid0);
        this->edges.push_back(vid1);

        this->v2v.at(vid0).push_back(vid1);
        this->v2v.at(vid1).push_back(vid0);

        this->v2e.at(vid0).push_back(eid);
        this->v2e.at(vid1).push_back(eid);

        std::vector<uint> fids = e2f_it.second;
        for(uint fid : fids)
        {
            this->f2e.at(fid).push_back(eid);
            this->e2f.at(eid).push_back(fid);
            for(uint adj_fid : fids) if (fid != adj_fid) this->f2f.at(fid).push_back(adj_fid);
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

    logger << this->num_verts() << "\tverts" << endl;
    logger << this->num_faces() << "\tfaces" << endl;
    logger << this->num_edges() << "\tedges" << endl;

    timer_stop("Build adjacency");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractSurfaceMesh<M,V,E,F>::vert_is_saddle(const uint vid, const int tex_coord) const
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
            case U_param : if (this->vert_data(nbr).uvw[0] != this->vert_data(vid).uvw[0]) signs.push_back(this->vert_data(nbr).uvw[0] > this->vert_data(vid).uvw[0]); break;
            case V_param : if (this->vert_data(nbr).uvw[1] != this->vert_data(vid).uvw[1]) signs.push_back(this->vert_data(nbr).uvw[1] > this->vert_data(vid).uvw[1]); break;
            case W_param : if (this->vert_data(nbr).uvw[2] != this->vert_data(vid).uvw[2]) signs.push_back(this->vert_data(nbr).uvw[2] > this->vert_data(vid).uvw[2]); break;
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
bool AbstractSurfaceMesh<M,V,E,F>::vert_is_critical_p(const uint vid, const int tex_coord) const
{
    return (this->vert_is_local_max(vid,tex_coord) ||
            this->vert_is_local_min(vid,tex_coord) ||
            this->vert_is_saddle   (vid,tex_coord));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractSurfaceMesh<M,V,E,F>::vert_opposite_to(const uint eid, const uint vid) const
{
    assert(this->edge_contains_vert(eid, vid));
    if (this->edge_vert_id(eid,0) != vid) return this->edge_vert_id(eid,0);
    else                                  return this->edge_vert_id(eid,1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractSurfaceMesh<M,V,E,F>::verts_are_ordered_CCW(const uint fid, const uint curr, const uint prev) const
{
    uint prev_offset = this->face_vert_offset(fid, prev);
    uint curr_offset = this->face_vert_offset(fid, curr);
    if (curr_offset == (prev_offset+1)%this->verts_per_face(fid)) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractSurfaceMesh<M,V,E,F>::vert_ordered_one_ring(const uint vid,
                                                         std::vector<uint> & v_ring,       // sorted list of adjacent vertices
                                                         std::vector<uint> & f_ring,       // sorted list of adjacent triangles
                                                         std::vector<uint> & e_ring,       // sorted list of edges incident to vid
                                                         std::vector<uint> & e_link) const // sorted list of edges opposite to vid
{
    v_ring.clear();
    f_ring.clear();
    e_ring.clear();
    e_link.clear();

    if (this->adj_v2e(vid).empty()) return;
    uint curr_e  = this->adj_v2e(vid).front(); assert(edge_is_manifold(curr_e));
    uint curr_v  = this->vert_opposite_to(curr_e, vid);
    uint curr_f  = this->adj_e2f(curr_e).front();
    // impose CCW winding...
    if (!this->verts_are_ordered_CCW(curr_f, curr_v, vid)) curr_f = this->adj_e2f(curr_e).back();

    // If there are boundary edges it is important to start from the right triangle (i.e. right-most),
    // otherwise it will be impossible to cover the entire umbrella
    std::vector<uint> b_edges = vert_boundary_edges(vid);
    if (b_edges.size()  > 0)
    {
        assert(b_edges.size() == 2); // otherwise there is no way to cover the whole umbrella walking through adjacent triangles!!!

        uint e = b_edges.front();
        uint f = this->adj_e2f(e).front();
        uint v = vert_opposite_to(e, vid);

        if (!this->verts_are_ordered_CCW(f, v, vid))
        {
            e = b_edges.back();
            f = this->adj_e2f(e).front();
            v = vert_opposite_to(e, vid);
            assert(this->verts_are_ordered_CCW(f, v, vid));
        }

        curr_e = e;
        curr_f = f;
        curr_v = v;
    }

    do
    {
        e_ring.push_back(curr_e);
        f_ring.push_back(curr_f);

        uint off = this->face_vert_offset(curr_f, curr_v);
        for(uint i=0; i<this->verts_per_face(curr_f)-1; ++i)
        {
            curr_v = this->face_vert_id(curr_f,(off+i)%this->verts_per_face(curr_f));
            if (i>0) e_link.push_back( this->face_edge_id(curr_f, curr_v, v_ring.back()) );
            v_ring.push_back(curr_v);
        }

        curr_e = this->face_edge_id(curr_f, vid, v_ring.back()); assert(edge_is_manifold(curr_e));
        curr_f = (this->adj_e2f(curr_e).front() == curr_f) ? this->adj_e2f(curr_e).back() : this->adj_e2f(curr_e).front();

        v_ring.pop_back();

        if (edge_is_boundary(curr_e)) e_ring.push_back(curr_e);
    }
    while(e_ring.size() < this->adj_v2e(vid).size());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> AbstractSurfaceMesh<M,V,E,F>::vert_ordered_vert_ring(const uint vid) const
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
std::vector<uint> AbstractSurfaceMesh<M,V,E,F>::vert_ordered_face_ring(const uint vid) const
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
std::vector<uint> AbstractSurfaceMesh<M,V,E,F>::vert_ordered_edge_ring(const uint vid) const
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
std::vector<uint> AbstractSurfaceMesh<M,V,E,F>::vert_ordered_edge_link(const uint vid) const
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
double AbstractSurfaceMesh<M,V,E,F>::vert_area(const uint vid) const
{
    double area = 0.0;
    for(uint fid : this->adj_v2f(vid)) area += face_area(fid)/static_cast<double>(this->verts_per_face(fid));
    return area;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double AbstractSurfaceMesh<M,V,E,F>::vert_mass(const uint vid) const
{
    return vert_area(vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractSurfaceMesh<M,V,E,F>::vert_is_boundary(const uint vid) const
{
    for(uint eid : this->adj_v2e(vid)) if (edge_is_boundary(eid)) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> AbstractSurfaceMesh<M,V,E,F>::vert_boundary_edges(const uint vid) const
{
    std::vector<uint> b_edges;
    for(uint eid : this->adj_v2e(vid)) if (edge_is_boundary(eid)) b_edges.push_back(eid);
    return b_edges;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractSurfaceMesh<M,V,E,F>::edge_is_manifold(const uint eid) const
{
    return (this->adj_e2f(eid).size() <= 2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractSurfaceMesh<M,V,E,F>::edge_is_boundary(const uint eid) const
{
    return (this->adj_e2f(eid).size() == 1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractSurfaceMesh<M,V,E,F>::edges_share_face(const uint eid1, const uint eid2) const
{
    for(uint fid1 : this->adj_e2f(eid1))
    for(uint fid2 : this->adj_e2f(eid2))
    {
        if (fid1 == fid2) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
ipair AbstractSurfaceMesh<M,V,E,F>::edge_shared(const uint fid0, const uint fid1) const
{
    std::vector<uint> shared_verts;
    uint v0 = this->face_vert_id(fid0,0);
    uint v1 = this->face_vert_id(fid0,1);
    uint v2 = this->face_vert_id(fid0,2);

    if (this->face_contains_vert(fid1,v0)) shared_verts.push_back(v0);
    if (this->face_contains_vert(fid1,v1)) shared_verts.push_back(v1);
    if (this->face_contains_vert(fid1,v2)) shared_verts.push_back(v2);
    assert(shared_verts.size() == 2);

    ipair e;
    e.first  = shared_verts.front();
    e.second = shared_verts.back();
    return e;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
int AbstractSurfaceMesh<M,V,E,F>::face_shared(const uint eid0, const uint eid1) const
{
    for(uint fid0 : this->adj_e2f(eid0))
    for(uint fid1 : this->adj_e2f(eid1))
    {
        if (fid0 == fid1) return fid0;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
int AbstractSurfaceMesh<M,V,E,F>::face_adjacent_along(const uint fid, const uint vid0, const uint vid1) const
{
    // WARNING : assume the edge vid0,vid1 is manifold!
    uint eid = this->face_edge_id(fid, vid0, vid1);
    assert(edge_is_manifold(eid));

    for(uint nbr : this->adj_f2f(fid))
    {
        if (this->face_contains_vert(nbr,vid0) &&
            this->face_contains_vert(nbr,vid1))
        {
            return nbr;
        }
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
int AbstractSurfaceMesh<M,V,E,F>::face_opposite_to(const uint eid, const uint fid) const
{
    assert(this->face_contains_edge(fid,eid));
    assert(this->edge_is_manifold(eid));
    assert(!this->adj_e2f(eid).empty());

    if (this->edge_is_boundary(eid)) return -1;
    if (this->adj_e2f(eid).front() != fid) return this->adj_e2f(eid).front();
    return this->adj_e2f(eid).back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool AbstractSurfaceMesh<M,V,E,F>::face_is_boundary(const uint fid) const
{
    return (this->adj_f2f(fid).size() < 3);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double AbstractSurfaceMesh<M,V,E,F>::face_mass(const uint fid) const
{
    return face_area(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractSurfaceMesh<M,V,E,F>::normalize_area()
{
    double area = 0.0;
    for(uint fid=0; fid<this->num_faces(); ++fid) area += this->elem_mass(fid);
    area = std::max(1e-4,area); // avoid creating degenerate faces...
    double s = 1.0 / sqrt(area);
    for(uint vid=0; vid<this->num_verts(); ++vid) this->vert(vid) *= s;
    this->update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<ipair> AbstractSurfaceMesh<M,V,E,F>::get_boundary_edges() const
{
    std::vector<ipair> res;
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (this->edge_is_boundary(eid))
        {
            ipair e;
            e.first  = this->edge_vert_id(eid,0);
            e.second = this->edge_vert_id(eid,1);
            res.push_back(e);
        }
    }
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> AbstractSurfaceMesh<M,V,E,F>::get_boundary_vertices() const
{
    std::vector<uint> res;
    for(uint vid=0; vid<this->num_verts(); ++vid) if (this->vert_is_boundary(vid)) res.push_back(vid);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractSurfaceMesh<M,V,E,F>::elem_vert_id(const uint fid, const uint offset) const
{
    return this->face_vert_id(fid,offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d AbstractSurfaceMesh<M,V,E,F>::elem_vert(const uint fid, const uint offset) const
{
    return this->face_vert(fid,offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d AbstractSurfaceMesh<M,V,E,F>::elem_centroid(const uint fid) const
{
    return this->face_centroid(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void AbstractSurfaceMesh<M,V,E,F>::elem_show_all()
{
    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        this->face_data(fid).visible = true;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double AbstractSurfaceMesh<M,V,E,F>::elem_mass(const uint fid) const
{
    return this->face_area(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint AbstractSurfaceMesh<M,V,E,F>::elem_vert_offset(const uint fid, const uint vid) const
{
    return this->face_vert_offset(fid,vid);
}


}
