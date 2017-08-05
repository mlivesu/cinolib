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
#include <cinolib/meshes/abstract_volume_mesh.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void AbstractPolyhedralMesh<M,V,E,F,P>::clear()
{
    AbstractMesh<M,V,E,P>::clear();
    //
    faces.clear();
    triangulated_faces.clear();
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
uint AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_id(const uint pid, const uint off) const
{
    return this->polys.at(pid).at(off);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
bool AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_is_CW(const uint pid, const uint off) const
{
    return (polys_face_winding.at(pid).at(off) == true);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
bool AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_is_CCW(const uint pid, const uint off) const
{
    return (polys_face_winding.at(pid).at(off) == false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
uint AbstractPolyhedralMesh<M,V,E,F,P>::poly_face_offset(const uint pid, const uint fid) const
{
    for(uint off=0; off<this->polys.at(pid).size(); ++off)
    {
        if (poly_face_id(pid,off) == fid) return off;
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
bool AbstractPolyhedralMesh<M,V,E,F,P>::poly_contains_vert(const uint pid, const uint vid) const
{
    for(uint v : adj_p2v(pid)) if(v == vid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d AbstractPolyhedralMesh<M,V,E,F,P>::poly_centroid(const uint pid) const
{
    vec3d c(0,0,0);
    for(uint vid : adj_p2v(pid)) c += this->vert(vid);
    c /= static_cast<double>(this->verts_per_poly(pid));
    return c;
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
bool AbstractPolyhedralMesh<M,V,E,F,P>::face_is_on_srf(const uint fid) const
{
    return f_on_srf.at(fid);
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
bool AbstractPolyhedralMesh<M,V,E,F,P>::edge_is_on_srf(const uint eid) const
{
    return e_on_srf.at(eid);
}

}
