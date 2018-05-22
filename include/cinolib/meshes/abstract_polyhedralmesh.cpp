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
#include <cinolib/geometry/polygon.h>
#include <unordered_set>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::clear()
{
    AbstractMesh<M,V,E,P>::clear();
    //
    faces.clear();
    face_triangles.clear();
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
void AbstractPolyhedralMesh<M,V,E,F,P>::init(const std::vector<vec3d>             & verts,
                                             const std::vector<std::vector<uint>> & faces,
                                             const std::vector<std::vector<uint>> & polys,
                                             const std::vector<std::vector<bool>> & polys_face_winding)
{
    for(auto v : verts) this->vert_add(v);
    for(auto f : faces) this->face_add(f);
    for(uint pid=0; pid<polys.size(); ++pid) this->poly_add(polys.at(pid), polys_face_winding.at(pid));

    this->copy_xyz_to_uvw(UVW_param);

    std::cout << "new mesh\t"      <<
                 this->num_verts() << "V / " <<
                 this->num_edges() << "E / " <<
                 this->num_faces() << "F / " <<
                 this->num_polys() << "P   " << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::num_srf_verts() const
{
    uint count = 0;
    for(uint vid=0; vid<this->num_verts(); ++vid)
    {
        if(this->vert_is_on_srf(vid)) ++count;
    }
    return count;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::num_srf_edges() const
{
    uint count = 0;
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if(this->edge_is_on_srf(eid)) ++count;
    }
    return count;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::num_srf_faces() const
{
    uint count = 0;
    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        if(this->face_is_on_srf(fid)) ++count;
    }
    return count;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::num_srf_polys() const
{
    uint count = 0;
    for(uint pid=0; pid<this->num_polys(); ++pid)
    {        
        if(this->poly_is_on_surf(pid)) ++count;
    }
    return count;
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
void AbstractPolyhedralMesh<M,V,E,F,P>::update_f_tessellation()
{
    this->face_triangles.resize(this->num_faces());
    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        update_f_tessellation(fid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::update_f_tessellation(const uint fid)
{
    // Assume convexity and try trivial tessellation first. If something flips
    // apply earcut algorithm to get a valid triangulation

    std::vector<vec3d> n;
    for (uint i=2; i<this->verts_per_face(fid); ++i)
    {
        uint vid0 = this->faces.at(fid).at( 0 );
        uint vid1 = this->faces.at(fid).at(i-1);
        uint vid2 = this->faces.at(fid).at( i );

        face_triangles.at(fid).push_back(vid0);
        face_triangles.at(fid).push_back(vid1);
        face_triangles.at(fid).push_back(vid2);

        n.push_back((this->vert(vid1)-this->vert(vid0)).cross(this->vert(vid2)-this->vert(vid0)));
    }

    bool bad_tessellation = false;
    for(uint i=0; i<n.size()-1; ++i) if (n.at(i).dot(n.at(i+1))<0) bad_tessellation = true;

    if (bad_tessellation)
    {
        // NOTE: the triangulation is constructed on a proxy polygon obtained
        // projecting the actual polygon onto the best fitting plane. Bad things
        // can still happen for highly non-planar polygons...

        std::vector<vec3d> vlist(this->verts_per_face(fid));
        for (uint i=0; i<this->verts_per_face(fid); ++i)
        {
            vlist.at(i) = this->face_vert(fid,i);
        }
        //
        std::vector<uint> tris;
        if (polygon_triangulate(vlist, tris))
        {
            face_triangles.at(fid).clear();
            for(uint off : tris) face_triangles.at(fid).push_back(this->face_vert_id(fid,off));
        }
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
    if (n.length()>0) n.normalize();
    this->vert_data(vid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::face_tessellation(const uint fid) const
{
    return face_triangles.at(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::face_verts_id(const uint fid, const bool sort_by_vid) const
{
    if(sort_by_vid)
    {
        std::vector<uint> v_list = this->adj_f2v(fid);
        SORT_VEC(v_list);
        return v_list;
    }
    return this->adj_f2v(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
int AbstractPolyhedralMesh<M,V,E,F,P>::face_id(const std::vector<uint> & f) const
{
    std::vector<uint> query = SORT_VEC(f);

    uint vid = f.front();
    for(uint fid : this->adj_v2f(vid))
    {
        if(this->face_verts_id(fid,true)==query) return fid;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::face_is_tri(const uint fid) const
{
    if(this->verts_per_face(fid)==3) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::face_is_quad(const uint fid) const
{
    if(this->verts_per_face(fid)==4) return true;
    return false;
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
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::poly_e2f(const uint pid, const uint eid) const
{
    assert(this->poly_contains_edge(pid,eid));
    std::vector<uint> faces;
    for(uint fid : this->adj_e2f(eid))
    {
        if (this->poly_contains_face(pid,fid)) faces.push_back(fid);
    }
    assert(faces.size()==2);
    return faces;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::poly_v2f(const uint pid, const uint vid) const
{
    assert(this->poly_contains_vert(pid,vid));
    std::vector<uint> faces;
    for(uint fid : this->adj_v2f(vid))
    {
        if(this->poly_contains_face(pid,fid)) faces.push_back(fid);
    }
    return faces;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::poly_is_on_surf(const uint pid) const
{
    for(uint off=0; off<faces_per_poly(pid); ++off)
    {
        if(f_on_srf.at(poly_face_id(pid,off))) return true;
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
bool AbstractPolyhedralMesh<M,V,E,F,P>::faces_are_adjacent(const uint fid0, const uint fid1) const
{
    for(uint eid : this->adj_f2e(fid0))
    for(uint nbr : this->adj_e2f(eid))
    {
        if(nbr==fid1) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool AbstractPolyhedralMesh<M,V,E,F,P>::faces_share_poly(const uint fid0, const uint fid1) const
{
    for(uint pid0 : this->adj_f2p(fid0))
    for(uint pid1 : this->adj_f2p(fid1))
    {
        if (pid0 == pid1) return true;
    }
    return false;
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
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_vert_offset(const uint fid, const uint vid) const
{
    for(uint off=0; off<verts_per_face(fid); ++off)
    {
        if (face_vert_id(fid,off) == vid) return off;
    }
    assert(false); // something went wrong
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
    assert(fid0!=fid1);
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
std::vector<uint> AbstractPolyhedralMesh<M,V,E,F,P>::face_v2e(const uint fid, const uint vid) const
{
    assert(this->face_contains_vert(fid,vid));
    std::vector<uint> edges;
    for(uint eid : this->adj_v2e(vid))
    {
        if (this->face_contains_edge(fid,eid)) edges.push_back(eid);
    }
    assert(edges.size()==2);
    return edges;
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
bool AbstractPolyhedralMesh<M,V,E,F,P>::face_winding_agrees_with(const uint fid, const uint vid0, const uint vid1) const
{
    assert(face_contains_vert(fid, vid0));
    assert(face_contains_vert(fid, vid1));

    uint off0 = face_vert_offset(fid, vid0);
    uint off1 = face_vert_offset(fid, vid1);

    if (off1 == (off0+1)%verts_per_face(fid)) return true;
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
    vert_ordered_srf_one_ring(vid, v_ring, e_ring, f_ring);
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
    vert_ordered_srf_one_ring(vid, v_ring, e_ring, f_ring);
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
    vert_ordered_srf_one_ring(vid, v_ring, e_ring, f_ring);
    return f_ring;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,C>::vert_ordered_srf_one_ring(const uint          vid,
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::vert_switch_id(const uint vid0, const uint vid1)
{
    if (vid0 == vid1) return;

    std::swap(this->verts.at(vid0),   this->verts.at(vid1));
    std::swap(this->v2v.at(vid0),     this->v2v.at(vid1));
    std::swap(this->v2e.at(vid0),     this->v2e.at(vid1));
    std::swap(this->v2f.at(vid0),     this->v2f.at(vid1));
    std::swap(this->v2p.at(vid0),     this->v2p.at(vid1));
    std::swap(this->v_data.at(vid0),  this->v_data.at(vid1));
    std::swap(this->v_on_srf.at(vid0),this->v_on_srf.at(vid1));

    std::unordered_set<uint> verts_to_update;
    verts_to_update.insert(this->adj_v2v(vid0).begin(), this->adj_v2v(vid0).end());
    verts_to_update.insert(this->adj_v2v(vid1).begin(), this->adj_v2v(vid1).end());

    std::unordered_set<uint> edges_to_update;
    edges_to_update.insert(this->adj_v2e(vid0).begin(), this->adj_v2e(vid0).end());
    edges_to_update.insert(this->adj_v2e(vid1).begin(), this->adj_v2e(vid1).end());

    std::unordered_set<uint> faces_to_update;
    faces_to_update.insert(this->adj_v2f(vid0).begin(), this->adj_v2f(vid0).end());
    faces_to_update.insert(this->adj_v2f(vid1).begin(), this->adj_v2f(vid1).end());

    std::unordered_set<uint> polys_to_update;
    polys_to_update.insert(this->adj_v2p(vid0).begin(), this->adj_v2p(vid0).end());
    polys_to_update.insert(this->adj_v2p(vid1).begin(), this->adj_v2p(vid1).end());

    for(uint nbr : verts_to_update)
    {
        for(uint & vid : this->v2v.at(nbr))
        {
            if (vid == vid0) vid = vid1; else
            if (vid == vid1) vid = vid0;
        }
    }

    for(uint eid : edges_to_update)
    {
        for(uint i=0; i<2; ++i)
        {
            uint & vid = this->edges.at(2*eid+i);
            if (vid == vid0) vid = vid1; else
            if (vid == vid1) vid = vid0;
        }
    }

    for(uint fid : faces_to_update)
    {
        for(uint & vid : this->faces.at(fid))
        {
            if (vid == vid0) vid = vid1; else
            if (vid == vid1) vid = vid0;
        }
        for(uint & vid : this->face_triangles.at(fid))
        {
            if (vid == vid0) vid = vid1; else
            if (vid == vid1) vid = vid0;
        }
    }

    for(uint pid : polys_to_update)
    {
        for(uint & vid : this->p2v.at(pid))
        {
            if (vid == vid0) vid = vid1; else
            if (vid == vid1) vid = vid0;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::vert_remove(const uint vid)
{
    polys_remove(this->adj_v2p(vid));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::vert_remove_unreferenced(const uint vid)
{
    this->v2v.at(vid).clear();
    this->v2e.at(vid).clear();
    this->v2f.at(vid).clear();
    this->v2p.at(vid).clear();
    vert_switch_id(vid, this->num_verts()-1);
    this->verts.pop_back();
    this->v_data.pop_back();
    this->v2v.pop_back();
    this->v2e.pop_back();
    this->v2f.pop_back();
    this->v2p.pop_back();
    this->v_on_srf.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::vert_add(const vec3d & pos)
{
    uint vid = this->num_verts();
    //
    this->verts.push_back(pos);
    //
    V data;
    this->v_data.push_back(data);
    assert(this->verts.size() == this->v_data.size());
    //
    this->v2v.push_back(std::vector<uint>());
    this->v2e.push_back(std::vector<uint>());
    this->v2f.push_back(std::vector<uint>());
    this->v2p.push_back(std::vector<uint>());
    //
    this->v_on_srf.push_back(false);
    //
    this->bb.min = this->bb.min.min(pos);
    this->bb.max = this->bb.max.max(pos);
    //
    return vid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::edge_switch_id(const uint eid0, const uint eid1)
{
    if (eid0 == eid1) return;

    for(uint off=0; off<2; ++off) std::swap(this->edges.at(2*eid0+off), this->edges.at(2*eid1+off));

    std::swap(this->e2f.at(eid0),     this->e2f.at(eid1));
    std::swap(this->e2p.at(eid0),     this->e2p.at(eid1));
    std::swap(this->e_data.at(eid0),  this->e_data.at(eid1));
    std::swap(this->e_on_srf.at(eid0),this->e_on_srf.at(eid1));

    std::unordered_set<uint> verts_to_update;
    verts_to_update.insert(this->edge_vert_id(eid0,0));
    verts_to_update.insert(this->edge_vert_id(eid0,1));
    verts_to_update.insert(this->edge_vert_id(eid1,0));
    verts_to_update.insert(this->edge_vert_id(eid1,1));

    std::unordered_set<uint> faces_to_update;
    faces_to_update.insert(this->adj_e2f(eid0).begin(), this->adj_e2f(eid0).end());
    faces_to_update.insert(this->adj_e2f(eid1).begin(), this->adj_e2f(eid1).end());

    std::unordered_set<uint> polys_to_update;
    polys_to_update.insert(this->adj_e2p(eid0).begin(), this->adj_e2p(eid0).end());
    polys_to_update.insert(this->adj_e2p(eid1).begin(), this->adj_e2p(eid1).end());

    for(uint vid : verts_to_update)
    {
        for(uint & eid : this->v2e.at(vid))
        {
            if (eid == eid0) eid = eid1; else
            if (eid == eid1) eid = eid0;
        }
    }

    for(uint fid : faces_to_update)
    {
        for(uint & eid : this->f2e.at(fid))
        {
            if (eid == eid0) eid = eid1; else
            if (eid == eid1) eid = eid0;
        }
    }
    for(uint pid : polys_to_update)
    {
        for(uint & eid : this->p2e.at(pid))
        {
            if (eid == eid0) eid = eid1; else
            if (eid == eid1) eid = eid0;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::edge_add(const uint vid0, const uint vid1)
{
    assert(vid0 < this->num_verts());
    assert(vid1 < this->num_verts());
    assert(!this->verts_are_adjacent(vid0, vid1));
    assert(DOES_NOT_CONTAIN_VEC(this->v2v.at(vid0), vid1));
    assert(DOES_NOT_CONTAIN_VEC(this->v2v.at(vid1), vid0));
    assert(this->edge_id(vid0, vid1) == -1);
    //
    uint eid = this->num_edges();
    //
    this->edges.push_back(vid0);
    this->edges.push_back(vid1);
    //
    this->e2f.push_back(std::vector<uint>());
    this->e2p.push_back(std::vector<uint>());
    //
    E data;
    this->e_data.push_back(data);
    assert(this->edges.size()/2 == this->e_data.size());
    //
    this->v2v.at(vid1).push_back(vid0);
    this->v2v.at(vid0).push_back(vid1);
    //
    this->v2e.at(vid0).push_back(eid);
    this->v2e.at(vid1).push_back(eid);
    //
    this->e_on_srf.push_back(false);
    //
    return eid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::edge_remove(const uint eid)
{
    polys_remove(this->adj_e2p(eid));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::edge_remove_unreferenced(const uint eid)
{
    this->e2f.at(eid).clear();
    this->e2p.at(eid).clear();
    edge_switch_id(eid, this->num_edges()-1);
    this->edges.resize(this->edges.size()-2);
    this->e_data.pop_back();
    this->e2f.pop_back();
    this->e2p.pop_back();
    this->e_on_srf.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::face_switch_id(const uint fid0, const uint fid1)
{
    // should I do something for poly_face_winding?

    if (fid0 == fid1) return;

    std::swap(this->faces.at(fid0),          this->faces.at(fid1));
    std::swap(this->f_data.at(fid0),         this->f_data.at(fid1));
    std::swap(this->f2e.at(fid0),            this->f2e.at(fid1));
    std::swap(this->f2f.at(fid0),            this->f2f.at(fid1));
    std::swap(this->f2p.at(fid0),            this->f2p.at(fid1));
    std::swap(this->f_on_srf.at(fid0),       this->f_on_srf.at(fid1));
    std::swap(this->face_triangles.at(fid0), this->face_triangles.at(fid1));

    std::unordered_set<uint> verts_to_update;
    verts_to_update.insert(this->adj_f2v(fid0).begin(), this->adj_f2v(fid0).end());
    verts_to_update.insert(this->adj_f2v(fid1).begin(), this->adj_f2v(fid1).end());

    std::unordered_set<uint> edges_to_update;
    edges_to_update.insert(this->adj_f2e(fid0).begin(), this->adj_f2e(fid0).end());
    edges_to_update.insert(this->adj_f2e(fid1).begin(), this->adj_f2e(fid1).end());

    std::unordered_set<uint> faces_to_update;
    faces_to_update.insert(this->adj_f2f(fid0).begin(), this->adj_f2f(fid0).end());
    faces_to_update.insert(this->adj_f2f(fid1).begin(), this->adj_f2f(fid1).end());

    std::unordered_set<uint> polys_to_update;
    polys_to_update.insert(this->adj_f2p(fid0).begin(), this->adj_f2p(fid0).end());
    polys_to_update.insert(this->adj_f2p(fid1).begin(), this->adj_f2p(fid1).end());

    for(uint vid : verts_to_update)
    {
        for(uint & fid : this->v2f.at(vid))
        {
            if (fid == fid0) fid = fid1; else
            if (fid == fid1) fid = fid0;
        }
    }

    for(uint eid : edges_to_update)
    {
        for(uint & fid : this->e2f.at(eid))
        {
            if (fid == fid0) fid = fid1; else
            if (fid == fid1) fid = fid0;
        }
    }

    for(uint nbr : faces_to_update)
    {
        for(uint & fid : this->f2f.at(nbr))
        {
            if (fid == fid0) fid = fid1; else
            if (fid == fid1) fid = fid0;
        }
    }

    for(uint pid : polys_to_update)
    {
        for(uint & fid : this->polys.at(pid))
        {
            if (fid == fid0) fid = fid1; else
            if (fid == fid1) fid = fid0;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::face_add(const std::vector<uint> & f)
{
#ifndef NDEBUG
    for(uint vid : f) assert(vid < this->num_verts());
#endif

    uint fid = this->num_faces();
    this->faces.push_back(f);

    F data;
    this->f_data.push_back(data);
    assert(this->faces.size() == this->f_data.size());

    this->f2e.push_back(std::vector<uint>());
    this->f2f.push_back(std::vector<uint>());
    this->f2p.push_back(std::vector<uint>());

    // add missing edges and handle on_surf flags...
    for(uint i=0; i<f.size(); ++i)
    {
        uint vid0 = f.at(i);
        uint vid1 = f.at((i+1)%f.size());
        int  eid = this->edge_id(vid0, vid1);
        if (eid == -1) eid = this->edge_add(vid0, vid1);
        this->v_on_srf.at(vid0) = false;
        this->e_on_srf.at(eid)  = false;
    }
    this->f_on_srf.push_back(false);

    // update connectivity
    for(uint vid : f)
    {
        this->v2f.at(vid).push_back(fid);
    }
    //
    for(uint i=0; i<f.size(); ++i)
    {
        uint vid0 = f.at(i);
        uint vid1 = f.at((i+1)%f.size());
        int  eid = this->edge_id(vid0, vid1);
        assert(eid >= 0);

        for(uint nbr : this->e2f.at(eid))
        {
            assert(nbr!=fid);
            if (this->faces_are_adjacent(fid,nbr)) continue;
            this->f2f.at(nbr).push_back(fid);
            this->f2f.at(fid).push_back(nbr);
        }

        this->e2f.at(eid).push_back(fid);
        this->f2e.at(fid).push_back(eid);
    }

    this->update_f_normal(fid);
    for(uint vid : f) this->update_v_normal(vid);

    this->face_triangles.push_back(std::vector<uint>());
    update_f_tessellation(fid);

    return fid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::face_remove(const uint fid)
{
    polys_remove(this->adj_f2p(fid));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::face_remove_unreferenced(const uint fid)
{
    this->faces.at(fid).clear();
    this->f2e.at(fid).clear();
    this->f2f.at(fid).clear();
    this->f2p.at(fid).clear();
    this->face_triangles.at(fid).clear();
    face_switch_id(fid, this->num_faces()-1);
    this->faces.pop_back();
    this->f_data.pop_back();
    this->f2e.pop_back();
    this->f2f.pop_back();
    this->f2p.pop_back();
    this->f_on_srf.pop_back();
    this->face_triangles.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::poly_switch_id(const uint pid0, const uint pid1)
{
    if (pid0 == pid1) return;

    std::swap(this->polys.at(pid0),              this->polys.at(pid1));
    std::swap(this->p_data.at(pid0),             this->p_data.at(pid1));
    std::swap(this->p2v.at(pid0),                this->p2v.at(pid1));
    std::swap(this->p2e.at(pid0),                this->p2e.at(pid1));
    std::swap(this->p2p.at(pid0),                this->p2p.at(pid1));
    std::swap(this->polys_face_winding.at(pid0), this->polys_face_winding.at(pid1));

    std::unordered_set<uint> verts_to_update;
    verts_to_update.insert(this->adj_p2v(pid0).begin(), this->adj_p2v(pid0).end());
    verts_to_update.insert(this->adj_p2v(pid1).begin(), this->adj_p2v(pid1).end());

    std::unordered_set<uint> edges_to_update;
    edges_to_update.insert(this->adj_p2e(pid0).begin(), this->adj_p2e(pid0).end());
    edges_to_update.insert(this->adj_p2e(pid1).begin(), this->adj_p2e(pid1).end());

    std::unordered_set<uint> faces_to_update;
    faces_to_update.insert(this->adj_p2f(pid0).begin(), this->adj_p2f(pid0).end());
    faces_to_update.insert(this->adj_p2f(pid1).begin(), this->adj_p2f(pid1).end());

    std::unordered_set<uint> polys_to_update;
    polys_to_update.insert(this->adj_p2p(pid0).begin(), this->adj_p2p(pid0).end());
    polys_to_update.insert(this->adj_p2p(pid1).begin(), this->adj_p2p(pid1).end());

    for(uint vid : verts_to_update)
    {
        for(uint & pid : this->v2p.at(vid))
        {
            if (pid == pid0) pid = pid1; else
            if (pid == pid1) pid = pid0;
        }
    }

    for(uint eid : edges_to_update)
    {
        for(uint & pid : this->e2p.at(eid))
        {
            if (pid == pid0) pid = pid1; else
            if (pid == pid1) pid = pid0;
        }
    }

    for(uint fid : faces_to_update)
    {
        for(uint & pid : this->f2p.at(fid))
        {
            if (pid == pid0) pid = pid1; else
            if (pid == pid1) pid = pid0;
        }
    }

    for(uint nbr : polys_to_update)
    {
        for(uint & pid : this->p2p.at(nbr))
        {
            if (pid == pid0) pid = pid1; else
            if (pid == pid1) pid = pid0;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint AbstractPolyhedralMesh<M,V,E,F,P>::poly_add(const std::vector<uint> & flist,
                                                 const std::vector<bool> & fwinding)
{
#ifndef NDEBUG
    for(uint fid : flist) assert(fid < this->num_faces());
#endif
    assert(flist.size() == fwinding.size());

    uint pid = this->num_polys();
    this->polys.push_back(flist);
    this->polys_face_winding.push_back(fwinding);

    P data;
    this->p_data.push_back(data);
    assert(this->polys.size() == this->p_data.size());

    this->p2v.push_back(std::vector<uint>());
    this->p2e.push_back(std::vector<uint>());
    this->p2p.push_back(std::vector<uint>());

    // update connectivity
    for(uint fid : flist)
    {
        std::vector<uint> &f = faces.at(fid);
        for(uint i=0; i<f.size(); ++i)
        {
            uint vid0 = f.at(i);
            uint vid1 = f.at((i+1)%f.size());
            int  eid = this->edge_id(vid0, vid1);
            assert(eid >= 0);

            if (!this->poly_contains_edge(pid,eid))
            {
                this->e2p.at(eid).push_back(pid);
                this->p2e.at(pid).push_back(eid);
            }

            if (!this->poly_contains_vert(pid,vid0))
            {
                this->p2v.at(pid).push_back(vid0);
                this->v2p.at(vid0).push_back(pid);
            }
        }

        for(uint nbr : this->adj_f2p(fid))
        {
            if (pid!=nbr && DOES_NOT_CONTAIN_VEC(this->p2p.at(pid),nbr))
            {
                assert(DOES_NOT_CONTAIN_VEC(this->p2p.at(nbr),pid));
                this->p2p.at(pid).push_back(nbr);
                this->p2p.at(nbr).push_back(pid);
            }
        }

        this->f2p.at(fid).push_back(pid);
    }

    // update x_on_srf flags
    for(uint fid : flist)
    {
        this->f_on_srf.at(fid) = (this->f2p.at(fid).size()<2);
        //
        for(uint eid : this->adj_f2e(fid))
        {
            this->e_on_srf.at(eid) = false;
            for(uint inc_fid : this->adj_e2f(eid))
            {
                if(this->f_on_srf.at(inc_fid)) this->e_on_srf.at(eid) = true;
            }
        }
        //
        for(uint vid : this->adj_f2v(fid))
        {
            this->v_on_srf.at(vid) = false;
            for(uint inc_fid : this->adj_v2f(vid))
            {
                if(this->f_on_srf.at(inc_fid)) this->v_on_srf.at(vid) = true;
            }
        }
    }

    return pid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::poly_remove_unreferenced(const uint pid)
{
    this->polys.at(pid).clear();
    this->p2v.at(pid).clear();
    this->p2e.at(pid).clear();
    this->p2p.at(pid).clear();
    this->polys_face_winding.at(pid).clear();
    poly_switch_id(pid, this->num_polys()-1);
    this->polys.pop_back();
    this->p_data.pop_back();
    this->p2v.pop_back();
    this->p2e.pop_back();
    this->p2p.pop_back();
    this->polys_face_winding.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::poly_remove(const uint pid)
{
    std::set<uint,std::greater<uint>> dangling_verts; // higher ids first
    std::set<uint,std::greater<uint>> dangling_edges; // higher ids first
    std::set<uint,std::greater<uint>> dangling_faces; // higher ids first

    std::vector<uint> verts_to_update;
    std::vector<uint> edges_to_update;
    std::vector<uint> faces_to_update;

    // disconnect from vertices
    for(uint vid : this->adj_p2v(pid))
    {
        REMOVE_FROM_VEC(this->v2p.at(vid), pid);
        if(this->v2p.at(vid).empty()) dangling_verts.insert(vid);
        else                          verts_to_update.push_back(vid);
    }

    // disconnect from edges
    for(uint eid : this->adj_p2e(pid))
    {
        REMOVE_FROM_VEC(this->e2p.at(eid), pid);
        if(this->e2p.at(eid).empty()) dangling_edges.insert(eid);
        else                          edges_to_update.push_back(eid);
    }

    // disconnect from faces
    for(uint fid : this->adj_p2f(pid))
    {
        REMOVE_FROM_VEC(this->f2p.at(fid), pid);
        if(this->f2p.at(fid).empty()) dangling_faces.insert(fid);
        else                          faces_to_update.push_back(fid);
    }

    // disconnect from other polyhedra
    for(uint nbr : this->adj_p2p(pid)) REMOVE_FROM_VEC(this->p2p.at(nbr), pid);

    // delete dangling faces
    for(uint fid : dangling_faces)
    {
        assert(this->adj_f2p(fid).empty());
        for(uint vid : this->faces.at(fid)) REMOVE_FROM_VEC(this->v2f.at(vid), fid);
        for(uint eid : this->f2e.at(fid))   REMOVE_FROM_VEC(this->e2f.at(eid), fid);
        for(uint nbr : this->f2f.at(fid))   REMOVE_FROM_VEC(this->f2f.at(nbr), fid);
        face_remove_unreferenced(fid);
    }

    // delete dangling edges
    for(uint eid : dangling_edges)
    {
        assert(this->adj_e2f(eid).empty());
        assert(this->adj_e2p(eid).empty());
        uint vid0 = this->edge_vert_id(eid,0);
        uint vid1 = this->edge_vert_id(eid,1);
        //if (vid1 > vid0) std::swap(vid0,vid1); // make sure the highest id is processed first
        REMOVE_FROM_VEC(this->v2e.at(vid0), eid);
        REMOVE_FROM_VEC(this->v2e.at(vid1), eid);
        REMOVE_FROM_VEC(this->v2v.at(vid0), vid1);
        REMOVE_FROM_VEC(this->v2v.at(vid1), vid0);
        edge_remove_unreferenced(eid);
    }

    // delete dangling vertices
    for(uint vid : dangling_verts)
    {
        assert(this->adj_v2e(vid).empty());
        assert(this->adj_v2f(vid).empty());
        assert(this->adj_v2p(vid).empty());
        for(uint nbr : this->adj_v2v(vid)) REMOVE_FROM_VEC(this->v2v.at(nbr), vid);
        vert_remove_unreferenced(vid);
    }

    // update f_on_srf flags
    for(uint fid : faces_to_update)
    {
        this->f_on_srf.at(fid) = this->adj_f2p(fid).size()<2;
    }
    // update e_on_srf flags
    for(uint eid : edges_to_update)
    {
        this->e_on_srf.at(eid) = false;
        for(uint fid : this->adj_e2f(eid)) if(this->face_is_on_srf(fid)) this->e_on_srf.at(eid) = true;
    }
    // update v_on_srf flags
    for(uint vid : verts_to_update)
    {
        this->v_on_srf.at(vid) = false;
        for(uint fid : this->adj_v2f(vid)) if(this->face_is_on_srf(fid)) this->v_on_srf.at(vid) = true;
    }

    poly_remove_unreferenced(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::polys_remove(const std::vector<uint> & pids)
{
    // in order to avoid id conflicts remove all the
    // polys starting from the one with highest id
    //
    std::vector<uint> tmp = SORT_VEC(pids, true);
    for(uint pid : tmp) poly_remove(pid);
}

}
