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
#include <cinolib/meshes/abstract_polyhedralmesh.h>
#include <cinolib/geometry/triangle.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::clear()
{
    AbstractMesh<M,V,E,P>::clear();
    //
    faces.clear();
    polys_face_winding.clear();
    //
    v_on_srf.clear();
    e_on_srf.clear();
    f_on_srf.clear();
    //
    f_data.clear();
    //
    v2f.clear();
    e2f.clear();
    f2e.clear();
    f2f.clear();
    f2p.clear();
    p2v.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::init()
{
    this->update_adjacency();
    this->update_bbox();

    this->v_data.resize(this->num_verts());
    this->e_data.resize(this->num_edges());
    this->f_data.resize(this->num_faces());
    this->p_data.resize(this->num_polys());

    this->update_normals();

    this->copy_xyz_to_uvw(UVW_param);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::update_adjacency()
{
    this->v2v.clear(); this->v2v.resize(this->num_verts());
    this->v2e.clear(); this->v2e.resize(this->num_verts());
    this->v2f.clear(); this->v2f.resize(this->num_verts());
    this->v2p.clear(); this->v2p.resize(this->num_verts());
    this->f2e.clear(); this->f2e.resize(this->num_faces());
    this->f2f.clear(); this->f2f.resize(this->num_faces());
    this->f2p.clear(); this->f2p.resize(this->num_faces());
    this->p2v.clear(); this->p2v.resize(this->num_polys());
    this->p2e.clear(); this->p2e.resize(this->num_polys());
    this->p2p.clear(); this->p2p.resize(this->num_polys());

    std::map<ipair,std::set<uint>> e2f_map;
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        std::set<uint> poly_verts; //unique list
        for(uint fid : this->polys.at(pid))
        {
            f2p.at(fid).push_back(pid);
            uint nv = this->verts_per_face(fid);
            for(uint off=0; off<nv; ++off)
            {
                uint vid = face_vert_id(fid, off);
                ipair e  = unique_pair(vid,face_vert_id(fid,(off+1)%nv));
                e2f_map[e].insert(fid);
                poly_verts.insert(vid);
            }
        }
        for(uint vid : poly_verts) this->v2p.at(vid).push_back(pid);
        std::copy(poly_verts.begin(), poly_verts.end(), std::back_inserter(p2v.at(pid)));
    }

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        for(uint off=0; off<verts_per_face(fid); ++off)
        {
            uint vid = face_vert_id(fid,off);
            v2f.at(vid).push_back(fid);
        }
    }

    this->edges.clear();
    this->e2f.clear(); this->e2f.resize(e2f_map.size());
    this->e2p.clear(); this->e2p.resize(e2f_map.size());

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

        std::vector<uint> fids(e2f_it.second.begin(),e2f_it.second.end());
        for(uint i=0; i<fids.size(); ++i)
        {
            uint fid = fids.at(i);
            f2e.at(fid).push_back(eid);
            e2f.at(eid).push_back(fid);

            for(uint j=i+1; j<fids.size(); ++j)
            {
                uint nbr = fids.at(j);
                f2f.at(fid).push_back(nbr);
                f2f.at(nbr).push_back(fid);
            }
        }
    }

    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        std::set<uint> edge_polys;
        for(uint fid : e2f.at(eid))
        for(uint pid : f2p.at(fid))
        {
            edge_polys.insert(pid);
        }
        for(uint pid : edge_polys) this->p2e.at(pid).push_back(eid);
        std::copy(edge_polys.begin(), edge_polys.end(), std::back_inserter(this->e2p.at(eid)));
    }

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        assert(f2p.at(fid).size() < 3);
        for(uint i=0;   i<f2p.at(fid).size()-1; ++i)
        for(uint j=i+1; j<f2p.at(fid).size();   ++j)
        {
            this->p2p.at(f2p.at(fid).at(i)).push_back(f2p.at(fid).at(j));
            this->p2p.at(f2p.at(fid).at(j)).push_back(f2p.at(fid).at(i));
        }
    }

    v_on_srf = std::vector<bool>(this->num_verts(),false);
    e_on_srf = std::vector<bool>(this->num_edges(),false);
    f_on_srf = std::vector<bool>(this->num_faces(),false);

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if (f2p.at(fid).size() == 1)
        {
            f_on_srf.at(fid) = true;
            for(uint eid : f2e.at(fid)) e_on_srf.at(eid) = true;
            for(uint off=0; off<this->verts_per_face(fid); ++off) v_on_srf.at(face_vert_id(fid,off)) = true;
        }
    }

    logger << this->num_verts() << "\tverts" << endl;
    logger << this->num_edges() << "\tedges" << endl;
    logger << this->num_faces() << "\tfaces" << endl;
    logger << this->num_polys() << "\tpolys" << endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::update_normals()
{
    update_f_normals();
    update_v_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::update_f_normals()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        update_f_normal(fid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::update_v_normals()
{
    for(uint vid=0; vid<this->num_verts(); ++vid)
    {
        if(vert_is_on_srf(vid)) update_v_normal(vid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::update_v_normal(const uint vid)
{
    vec3d n(0,0,0);
    for(uint fid : adj_v2f(vid))
    {
        if (face_is_on_srf(fid)) n += face_data(fid).normal;
    }
    n.normalize();
    this->vert_data(vid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::face_set_color(const Color & c)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::face_unmark_all()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).marked = false;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::face_set_alpha(const float alpha)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_id(const uint pid, const uint off) const
{
    return this->polys.at(pid).at(off);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_is_CW(const uint pid, const uint fid) const
{
    uint off = poly_face_offset(pid, fid);
    return (polys_face_winding.at(pid).at(off) == false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_is_CCW(const uint pid, const uint fid) const
{
    return !poly_face_is_CW(pid,fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_offset(const uint pid, const uint fid) const
{
    assert(poly_contains_face(pid,fid));
    for(uint off=0; off<this->polys.at(pid).size(); ++off)
    {
        if (poly_face_id(pid,off) == fid) return off;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_normal(const uint pid, const uint fid) const
{
    assert(poly_contains_face(pid,fid));
    if (poly_face_is_CCW(pid,fid)) return this->face_data(fid).normal;
    return -this->face_data(fid).normal;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
int AbstractPolyhedralMesh<M,V,E,F,P>::poly_adj_through_face(const uint pid, const uint fid) const
{
    assert(poly_contains_face(pid,fid));

    if (face_is_on_srf(fid)) return -1;

    for(uint nbr : this->adj_f2p(fid))
    {
        if (nbr!=pid) return nbr;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::poly_is_on_surf(const uint pid) const
{
    for(uint off=0; off<faces_per_poly(pid); ++off)
    {
        if (f_on_srf.at(poly_face_id(pid,off))) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::poly_contains_face(const uint pid, const uint fid) const
{
    for(uint off=0; off<faces_per_poly(pid); ++off)
    {
        if(poly_face_id(pid,off) == fid) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::faces_are_disjoint(const uint fid0, const uint fid1) const
{
    for(uint i=0; i<verts_per_face(fid0); ++i)
    for(uint j=0; j<verts_per_face(fid1); ++j)
    {
        if (face_vert_id(fid0,i) == face_vert_id(fid1,j)) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d AbstractPolyhedralMesh<M,V,E,F,P>::face_vert(const uint fid, const uint off) const
{
    return this->vert(face_vert_id(fid,off));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_vert_id(const uint fid, const uint off) const
{
    return faces.at(fid).at(off);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_edge_id(const uint fid, const uint vid0, const uint vid1) const
{
    assert(face_contains_vert(fid,vid0));
    assert(face_contains_vert(fid,vid1));
    for(uint eid : adj_f2e(fid))
    {
        if (this->edge_contains_vert(eid,vid0) &&
            this->edge_contains_vert(eid,vid1))
        {
            return eid;
        }
    }
    assert(false && "Something is off here");
    return 0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_edge_id(const uint fid, const uint off) const
{
    uint vid0 = this->face_vert_id(fid, off);
    uint vid1 = this->face_vert_id(fid, (off+1)%this->verts_per_face(fid));
    int  eid  = this->edge_id(vid0, vid1);
    assert(eid>=0);
    return eid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_adj_through_edge(const uint fid, const uint eid, const uint pid) const
{
    assert(this->poly_contains_face(pid, fid));
    assert(this->poly_contains_edge(pid, eid));
    assert(this->face_contains_edge(fid, eid));

    for(uint f : this->adj_e2f(eid))
    {
        if (f!=fid && poly_contains_face(pid,f)) return f;
    }

    return 0;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
ipair AbstractPolyhedralMesh<M,V,E,F,P>::face_edges_from_vert(const uint fid, const uint vid) const
{
    assert(face_contains_vert(fid,vid));

    ipair e;
    uint found = 0;
    for(uint eid : this->adj_f2e(fid))
    {
        if (this->edge_contains_vert(eid,vid))
        {
            if (found == 0) e.first = eid;
            else            e.second = eid;
            ++found;
        }
    }
    assert(found == 2);
    return e;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_adj_srf_edge(const uint fid, const uint eid, const uint vid) const
{
    assert(this->face_contains_edge(fid,eid));
    assert(this->face_contains_vert(fid,vid));

    for(uint e : this->adj_f2e(fid))
    {
        if (e!=eid && edge_is_on_srf(e) && this->edge_contains_vert(e, vid))
        {
            return e;
        }
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_opp_to_srf_edge(const uint fid, const uint eid) const
{
    assert(face_is_on_srf(fid));
    assert(edge_is_on_srf(eid));
    assert(face_contains_edge(fid,eid));

    for(uint f : adj_f2f(fid))
    {
        if (face_is_on_srf(f) && face_contains_edge(f,eid))
        {
            return f;
        }
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_shared_edge(const uint fid0, const uint fid1) const
{
    for(uint eid : this->adj_f2e(fid0))
    {
        if (this->face_contains_edge(fid1, eid)) return eid;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::face_is_on_srf(const uint fid) const
{
    return f_on_srf.at(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::face_contains_vert(const uint fid, const uint vid) const
{
    for(uint v : this->adj_f2v(fid))
    {
        if (v == vid) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::face_contains_edge(const uint fid, const uint eid) const
{
    for(uint e : this->adj_f2e(fid))
    {
        if (e == eid) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d AbstractPolyhedralMesh<M,V,E,F,P>::face_centroid(const uint fid) const
{
    vec3d c(0,0,0);
    for(uint off=0; off<verts_per_face(fid); ++off) c += face_vert(fid,off);
    c /= static_cast<double>(verts_per_face(fid));
    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double AbstractPolyhedralMesh<M,V,E,F,P>::face_mass(const uint fid) const
{
    return face_area(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double AbstractPolyhedralMesh<M,V,E,F,P>::face_area(const uint fid) const
{
    double area = 0.0;
    std::vector<uint> tris = face_tessellation(fid);
    for(uint i=0; i<tris.size()/3; ++i)
    {
        area += triangle_area(this->vert(tris.at(3*i+0)),
                              this->vert(tris.at(3*i+1)),
                              this->vert(tris.at(3*i+2)));
    }
    return area;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
int AbstractPolyhedralMesh<M,V,E,F,P>::poly_shared_face(const uint pid0, const uint pid1) const
{
    for(uint fid0 : adj_p2f(pid0))
    for(uint fid1 : adj_p2f(pid1))
    {
        if (fid0==fid1) return fid0;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
double AbstractPolyhedralMesh<M,V,E,F,P>::poly_mass(const uint pid) const
{
    return poly_volume(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::vert_is_on_srf(const uint vid) const
{
    return v_on_srf.at(vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
double AbstractPolyhedralMesh<M,V,E,F,C>::vert_mass(const uint vid) const
{
    return vert_volume(vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
double AbstractPolyhedralMesh<M,V,E,F,C>::vert_volume(const uint vid) const
{
    double vol = 0.0;    
    for(uint pid : this->adj_v2p(vid)) vol += this->poly_volume(pid);
    vol /= static_cast<double>(this->adj_v2p(vid).size());
    return vol;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::vert_adj_srf_edges(const uint vid) const
{
    std::vector<uint> srf_e;
    for(uint eid : this->adj_v2e(vid))
    {
        if (this->edge_is_on_srf(eid)) srf_e.push_back(eid);
    }
    return srf_e;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::vert_ordered_srf_vert_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent surfaces vertices
    std::vector<uint> e_ring; // sorted list of surface edges incident to vid
    std::vector<uint> f_ring; // sorted list of adjacent surface faces
    verd_ordered_srf_one_ring(vid, v_ring, e_ring, f_ring);
    return v_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::vert_ordered_srf_edge_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent surfaces vertices
    std::vector<uint> e_ring; // sorted list of surface edges incident to vid
    std::vector<uint> f_ring; // sorted list of adjacent surface faces
    verd_ordered_srf_one_ring(vid, v_ring, e_ring, f_ring);
    return e_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::vert_ordered_srf_face_ring(const uint vid) const
{
    std::vector<uint> v_ring; // sorted list of adjacent surfaces vertices
    std::vector<uint> e_ring; // sorted list of surface edges incident to vid
    std::vector<uint> f_ring; // sorted list of adjacent surface faces
    verd_ordered_srf_one_ring(vid, v_ring, e_ring, f_ring);
    return f_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,C>::verd_ordered_srf_one_ring(const uint          vid,
                                                                  std::vector<uint> & v_ring,
                                                                  std::vector<uint> & e_ring,
                                                                  std::vector<uint> & f_ring) const
{
    assert(vert_is_on_srf(vid));

    v_ring.clear();
    e_ring.clear();
    f_ring.clear();

    uint curr_e = this->vert_adj_srf_edges(vid).front();
    uint curr_f = this->edge_adj_srf_faces(curr_e).front();
    uint curr_v = this->vert_opposite_to(curr_e, vid);

    do
    {
        v_ring.push_back(curr_v);
        e_ring.push_back(curr_e);
        f_ring.push_back(curr_f);

        curr_e = face_adj_srf_edge(curr_f, curr_e, vid);
        curr_f = face_opp_to_srf_edge(curr_f, curr_e);
        curr_v = this->vert_opposite_to(curr_e, vid);
    }
    while(e_ring.size() < this->vert_adj_srf_edges(vid).size());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::edge_is_on_srf(const uint eid) const
{
    return e_on_srf.at(eid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::edge_ordered_poly_ring(const uint eid) const
{
    std::vector<uint> plist;
    if (this->adj_e2p(eid).empty()) return plist;

    uint curr_f = this->adj_e2f(eid).front();
    uint curr_p = this->adj_f2p(curr_f).front();

    if (this->edge_is_on_srf(eid)) // make sure to start from a face exposed on the surface,
    {                              // otherwise it'll be impossible to cover the whole umbrella
        curr_f = this->edge_adj_srf_faces(eid).front();
        curr_p = this->adj_f2p(curr_f).front();
    }    

    plist.push_back(curr_p);

    do
    {
        curr_f = face_adj_through_edge(curr_f, eid, curr_p);
        int tmp = poly_adj_through_face(curr_p, curr_f);
        if (tmp >= 0)
        {
            curr_p = static_cast<uint>(tmp);
            plist.push_back(curr_p);
        }
    }
    while (plist.size() < this->adj_e2p(eid).size());

    return plist;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::edge_adj_srf_faces(const uint eid) const
{
    std::vector<uint> srf_f;
    for(uint fid : adj_e2f(eid))
    {
        if (face_is_on_srf(fid)) srf_f.push_back(fid);
    }
    return srf_f;
}


}
