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
#include <cinolib/io/read_write.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::load(const char * filename)
{
    this->clear();
    std::vector<double> coords;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare(".off") == 0 ||
        filetype.compare(".OFF") == 0)
    {
        read_OFF(filename, coords, this->polys);
    }
    else if (filetype.compare(".obj") == 0 ||
             filetype.compare(".OBJ") == 0)
    {
        read_OBJ(filename, coords, this->polys);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    this->verts = vec3d_from_serialized_xyz(coords);

    this->mesh_data().filename = std::string(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::save(const char * filename) const
{
    std::vector<double> coords = serialized_xyz_from_vec3d(this->verts);

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        write_OFF(filename, coords, this->polys);
    }
    else if (filetype.compare("obj") == 0 ||
             filetype.compare("OBJ") == 0)
    {
        write_OBJ(filename, coords, this->polys);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << endl;
        exit(-1);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::init()
{
    this->update_adjacency();
    this->update_bbox();

    this->v_data.resize(this->num_verts());
    this->e_data.resize(this->num_edges());
    this->p_data.resize(this->num_polys());

    this->update_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::update_adjacency()
{
    this->v2v.clear(); this->v2v.resize(this->num_verts());
    this->v2e.clear(); this->v2e.resize(this->num_verts());
    this->v2p.clear(); this->v2p.resize(this->num_verts());
    this->p2p.clear(); this->p2p.resize(this->num_polys());
    this->p2e.clear(); this->p2e.resize(this->num_polys());

    std::map<ipair,std::vector<uint>> e2f_map;
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        for(uint offset=0; offset<this->verts_per_poly(fid); ++offset)
        {
            uint vid0 = this->poly_vert_id(fid,offset);
            uint vid1 = this->poly_vert_id(fid,(offset+1)%this->verts_per_poly(fid));
            this->v2p.at(vid0).push_back(fid);
            e2f_map[unique_pair(vid0,vid1)].push_back(fid);
        }
    }

    this->edges.clear();
    this->e2p.clear();
    this->e2p.resize(e2f_map.size());

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
            this->p2e.at(fid).push_back(eid);
            this->e2p.at(eid).push_back(fid);
            for(uint adj_fid : fids) if (fid != adj_fid) this->p2p.at(fid).push_back(adj_fid);
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
    logger << this->num_polys() << "\tfaces" << endl;
    logger << this->num_edges() << "\tedges" << endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::update_v_normal(const uint vid)
{
    vec3d n(0,0,0);
    for(uint fid : this->adj_v2p(vid))
    {
        n += this->poly_data(fid).normal;
    }
    n.normalize();
    this->vert_data(vid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::update_f_normals()
{
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        update_f_normal(fid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::update_v_normals()
{
    for(uint vid=0; vid<this->num_verts(); ++vid)
    {
        update_v_normal(vid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::update_normals()
{
    this->update_f_normals();
    this->update_v_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractPolygonMesh<M,V,E,P>::vert_is_saddle(const uint vid, const int tex_coord) const
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

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractPolygonMesh<M,V,E,P>::vert_is_critical_p(const uint vid, const int tex_coord) const
{
    return (this->vert_is_local_max(vid,tex_coord) ||
            this->vert_is_local_min(vid,tex_coord) ||
            this->vert_is_saddle   (vid,tex_coord));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint AbstractPolygonMesh<M,V,E,P>::vert_opposite_to(const uint eid, const uint vid) const
{
    assert(this->edge_contains_vert(eid, vid));
    if (this->edge_vert_id(eid,0) != vid) return this->edge_vert_id(eid,0);
    else                                  return this->edge_vert_id(eid,1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractPolygonMesh<M,V,E,P>::verts_are_ordered_CCW(const uint fid, const uint curr, const uint prev) const
{
    uint prev_offset = this->poly_vert_offset(fid, prev);
    uint curr_offset = this->poly_vert_offset(fid, curr);
    if (curr_offset == (prev_offset+1)%this->verts_per_poly(fid)) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::vert_ordered_one_ring(const uint vid,
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
    uint curr_f  = this->adj_e2p(curr_e).front();
    // impose CCW winding...
    if (!this->verts_are_ordered_CCW(curr_f, curr_v, vid)) curr_f = this->adj_e2p(curr_e).back();

    // If there are boundary edges it is important to start from the right triangle (i.e. right-most),
    // otherwise it will be impossible to cover the entire umbrella
    std::vector<uint> b_edges = vert_boundary_edges(vid);
    if (b_edges.size()  > 0)
    {
        assert(b_edges.size() == 2); // otherwise there is no way to cover the whole umbrella walking through adjacent triangles!!!

        uint e = b_edges.front();
        uint f = this->adj_e2p(e).front();
        uint v = vert_opposite_to(e, vid);

        if (!this->verts_are_ordered_CCW(f, v, vid))
        {
            e = b_edges.back();
            f = this->adj_e2p(e).front();
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

        uint off = this->poly_vert_offset(curr_f, curr_v);
        for(uint i=0; i<this->verts_per_poly(curr_f)-1; ++i)
        {
            curr_v = this->poly_vert_id(curr_f,(off+i)%this->verts_per_poly(curr_f));
            if (i>0) e_link.push_back( this->poly_edge_id(curr_f, curr_v, v_ring.back()) );
            v_ring.push_back(curr_v);
        }

        curr_e = this->poly_edge_id(curr_f, vid, v_ring.back()); assert(edge_is_manifold(curr_e));
        curr_f = (this->adj_e2p(curr_e).front() == curr_f) ? this->adj_e2p(curr_e).back() : this->adj_e2p(curr_e).front();

        v_ring.pop_back();

        if (edge_is_boundary(curr_e)) e_ring.push_back(curr_e);
    }
    while(e_ring.size() < this->adj_v2e(vid).size());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractPolygonMesh<M,V,E,P>::vert_ordered_vert_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent vertices
    std::vector<uint> f_ring; // sorted list of adjacent triangles
    std::vector<uint> e_ring; // sorted list of edges incident to vid
    std::vector<uint> e_link; // sorted list of edges opposite to vid
    vert_ordered_one_ring(vid, v_ring, f_ring, e_ring, e_link);
    return v_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractPolygonMesh<M,V,E,P>::vert_ordered_poly_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent vertices
    std::vector<uint> f_ring; // sorted list of adjacent triangles
    std::vector<uint> e_ring; // sorted list of edges incident to vid
    std::vector<uint> e_link; // sorted list of edges opposite to vid
    vert_ordered_one_ring(vid, v_ring, f_ring, e_ring, e_link);
    return f_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractPolygonMesh<M,V,E,P>::vert_ordered_edge_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent vertices
    std::vector<uint> f_ring; // sorted list of adjacent triangles
    std::vector<uint> e_ring; // sorted list of edges incident to vid
    std::vector<uint> e_link; // sorted list of edges opposite to vid
    vert_ordered_one_ring(vid, v_ring, f_ring, e_ring, e_link);
    return e_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractPolygonMesh<M,V,E,P>::vert_ordered_edge_link(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent vertices
    std::vector<uint> f_ring; // sorted list of adjacent triangles
    std::vector<uint> e_ring; // sorted list of edges incident to vid
    std::vector<uint> e_link; // sorted list of edges opposite to vid
    vert_ordered_one_ring(vid, v_ring, f_ring, e_ring, e_link);
    return e_link;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractPolygonMesh<M,V,E,P>::vert_area(const uint vid) const
{
    double area = 0.0;
    for(uint fid : this->adj_v2p(vid)) area += poly_area(fid)/static_cast<double>(this->verts_per_poly(fid));
    return area;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractPolygonMesh<M,V,E,P>::vert_mass(const uint vid) const
{
    return vert_area(vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractPolygonMesh<M,V,E,P>::vert_is_boundary(const uint vid) const
{
    for(uint eid : this->adj_v2e(vid)) if (edge_is_boundary(eid)) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractPolygonMesh<M,V,E,P>::vert_boundary_edges(const uint vid) const
{
    std::vector<uint> b_edges;
    for(uint eid : this->adj_v2e(vid)) if (edge_is_boundary(eid)) b_edges.push_back(eid);
    return b_edges;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractPolygonMesh<M,V,E,P>::edge_is_manifold(const uint eid) const
{
    return (this->adj_e2p(eid).size() <= 2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractPolygonMesh<M,V,E,P>::edge_is_boundary(const uint eid) const
{
    return (this->adj_e2p(eid).size() == 1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractPolygonMesh<M,V,E,P>::edges_share_face(const uint eid1, const uint eid2) const
{
    for(uint fid1 : this->adj_e2p(eid1))
    for(uint fid2 : this->adj_e2p(eid2))
    {
        if (fid1 == fid2) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
ipair AbstractPolygonMesh<M,V,E,P>::edge_shared(const uint fid0, const uint fid1) const
{
    std::vector<uint> shared_verts;
    uint v0 = this->poly_vert_id(fid0,0);
    uint v1 = this->poly_vert_id(fid0,1);
    uint v2 = this->poly_vert_id(fid0,2);

    if (this->poly_contains_vert(fid1,v0)) shared_verts.push_back(v0);
    if (this->poly_contains_vert(fid1,v1)) shared_verts.push_back(v1);
    if (this->poly_contains_vert(fid1,v2)) shared_verts.push_back(v2);
    assert(shared_verts.size() == 2);

    ipair e;
    e.first  = shared_verts.front();
    e.second = shared_verts.back();
    return e;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::edge_mark_labeling_boundaries()
{
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        std::set<int> unique_labels;
        for(uint fid : this->adj_e2p(eid)) unique_labels.insert(this->poly_data(fid).label);

        this->edge_data(eid).marked = (unique_labels.size()>=2);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractPolygonMesh<M,V,E,P>::poly_angle_at_vert(const uint fid, const uint vid, const int unit) const
{
    assert(this->poly_contains_vert(fid,vid));

    uint offset = 0;
    for(uint i=0; i<this->verts_per_poly(fid); ++i) if (this->poly_vert_id(fid,i) == vid) offset = i;
    assert(this->poly_vert_id(fid,offset) == vid);
    //
    // the code above substitutes this one (which was specific for AbstractMeshes...)
    //
    //     if (poly_vert_id(fid,0) == vid) offset = 0;
    //else if (poly_vert_id(fid,1) == vid) offset = 1;
    //else if (poly_vert_id(fid,2) == vid) offset = 2;
    //else { assert(false); offset=0; } // offset=0 kills uninitialized warning message

    vec3d p = this->vert(vid);
    vec3d u = this->poly_vert(fid,(offset+1)%this->verts_per_poly(fid)) - p;
    vec3d v = this->poly_vert(fid,(offset+2)%this->verts_per_poly(fid)) - p;

    switch (unit)
    {
        case RAD : return u.angle_rad(v);
        case DEG : return u.angle_deg(v);
        default  : assert(false);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int AbstractPolygonMesh<M,V,E,P>::poly_shared(const uint eid0, const uint eid1) const
{
    for(uint fid0 : this->adj_e2p(eid0))
    for(uint fid1 : this->adj_e2p(eid1))
    {
        if (fid0 == fid1) return fid0;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int AbstractPolygonMesh<M,V,E,P>::poly_adjacent_along(const uint fid, const uint vid0, const uint vid1) const
{
    // WARNING : assume the edge vid0,vid1 is manifold!
    uint eid = this->poly_edge_id(fid, vid0, vid1);
    assert(edge_is_manifold(eid));

    for(uint nbr : this->adj_p2p(fid))
    {
        if (this->poly_contains_vert(nbr,vid0) &&
            this->poly_contains_vert(nbr,vid1))
        {
            return nbr;
        }
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int AbstractPolygonMesh<M,V,E,P>::poly_opposite_to(const uint eid, const uint fid) const
{
    assert(this->poly_contains_edge(fid,eid));
    assert(this->edge_is_manifold(eid));
    assert(!this->adj_e2p(eid).empty());

    if (this->edge_is_boundary(eid)) return -1;
    if (this->adj_e2p(eid).front() != fid) return this->adj_e2p(eid).front();
    return this->adj_e2p(eid).back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool AbstractPolygonMesh<M,V,E,P>::poly_is_boundary(const uint fid) const
{
    return (this->adj_p2p(fid).size() < 3);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::normalize_area()
{
    double area = 0.0;
    for(uint fid=0; fid<this->num_polys(); ++fid) area += this->poly_mass(fid);
    area = std::max(1e-4,area); // avoid creating degenerate faces...
    double s = 1.0 / sqrt(area);
    for(uint vid=0; vid<this->num_verts(); ++vid) this->vert(vid) *= s;
    this->update_bbox();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<ipair> AbstractPolygonMesh<M,V,E,P>::get_boundary_edges() const
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

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> AbstractPolygonMesh<M,V,E,P>::get_boundary_vertices() const
{
    std::vector<uint> res;
    for(uint vid=0; vid<this->num_verts(); ++vid) if (this->vert_is_boundary(vid)) res.push_back(vid);
    return res;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::poly_show_all()
{
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        this->poly_data(fid).visible = true;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double AbstractPolygonMesh<M,V,E,P>::poly_mass(const uint fid) const
{
    return this->poly_area(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> AbstractPolygonMesh<M,V,E,P>::export_per_poly_labels() const
{
    std::vector<int> labels(this->num_polys());
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        labels.at(this->poly_data(fid).label);
    }
    return labels;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<Color> AbstractPolygonMesh<M,V,E,P>::export_per_poly_colors() const
{
    std::vector<Color> colors(this->num_polys());
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        colors.at(fid) = this->poly_data(fid).color;
    }
    return colors;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::poly_flip_winding_order(const uint fid)
{
    std::reverse(this->polys.at(fid).begin(), this->polys.at(fid).end());

    update_f_normal(fid);
    for(uint off=0; off<this->verts_per_poly(fid); ++off) update_v_normal(this->poly_vert_id(fid,off));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::poly_set_color(const Color & c)
{
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        this->poly_data(fid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::poly_set_alpha(const float alpha)
{
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        this->poly_data(fid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::poly_color_wrt_label()
{
    std::map<int,uint> l_map;
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        int l = this->poly_data(fid).label;
        if (DOES_NOT_CONTAIN(l_map,l)) l_map[l] = l_map.size();
    }
    uint n_labels = l_map.size();
    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        this->poly_data(fid).color = Color::scatter(n_labels,l_map.at(this->poly_data(fid).label));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void AbstractPolygonMesh<M,V,E,P>::operator+=(const AbstractPolygonMesh<M,V,E,P> & m)
{
    uint nv = this->num_verts();
    uint nf = this->num_polys();
    uint ne = this->num_edges();

    std::vector<uint> tmp;
    for(uint fid=0; fid<m.num_polys(); ++fid)
    {
        std::vector<uint> f;
        for(uint off=0; off<m.verts_per_poly(fid); ++off) f.push_back(nv + m.poly_vert_id(fid,off));
        this->polys.push_back(f);

        this->p_data.push_back(m.poly_data(fid));

        tmp.clear();
        for(uint eid : m.p2e.at(fid)) tmp.push_back(ne + eid);
        this->p2e.push_back(tmp);

        tmp.clear();
        for(uint nbr : m.p2p.at(fid)) tmp.push_back(nf + nbr);
        this->p2p.push_back(tmp);
    }
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        this->edges.push_back(nv + m.edge_vert_id(eid,0));
        this->edges.push_back(nv + m.edge_vert_id(eid,1));

        this->e_data.push_back(m.edge_data(eid));

        tmp.clear();
        for(uint tid : m.e2p.at(eid)) tmp.push_back(nf + tid);
        this->e2p.push_back(tmp);
    }
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        this->verts.push_back(m.vert(vid));
        this->v_data.push_back(m.vert_data(vid));

        tmp.clear();
        for(uint eid : m.v2e.at(vid)) tmp.push_back(ne + eid);
        this->v2e.push_back(tmp);

        tmp.clear();
        for(uint tid : m.v2p.at(vid)) tmp.push_back(nf + tid);
        this->v2p.push_back(tmp);

        tmp.clear();
        for(uint nbr : m.v2v.at(vid)) tmp.push_back(nv + nbr);
        this->v2v.push_back(tmp);
    }

    this->update_bbox();

    logger << "Appended " << m.mesh_data().filename << " to mesh " << this->mesh_data().filename << endl;
    logger << this->num_polys() << " faces" << endl;
    logger << this->num_verts() << " verts" << endl;
}

}
