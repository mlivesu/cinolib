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
#include <cinolib/meshes/polyhedralmesh/polyhedralmesh.h>
#include <cinolib/io/read_write.h>
#include <cinolib/common.h>

#include <algorithm>
#include <cmath>
#include <float.h>
#include <map>
#include <set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Polyhedralmesh<M,V,E,F,P>::Polyhedralmesh(const char * filename)
{
    load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Polyhedralmesh<M,V,E,F,P>::Polyhedralmesh(const std::vector<vec3d>             & verts,
                                          const std::vector<std::vector<uint>> & faces,
                                          const std::vector<std::vector<uint>> & polys,
                                          const std::vector<std::vector<bool>> & polys_face_winding)
{
    this->verts = verts;
    this->faces = faces;
    this->polys = polys;
    this->polys_face_winding = polys_face_winding;

    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::load(const char * filename)
{
    this->clear();
    std::vector<double> coords;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-7,7);

    if (filetype.compare(".hybrid") == 0 ||
        filetype.compare(".HYBRID") == 0)
    {
        read_HYBDRID(filename, coords, this->faces, this->polys, this->polys_face_winding);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    this->verts = vec3d_from_serialized_xyz(coords);

    logger << this->num_verts() << " verts read" << endl;
    logger << this->num_faces() << " faces read" << endl;
    logger << this->num_polys() << " polys read" << endl;

    this->mesh_data().filename = std::string(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::init()
{
    this->update_face_tessellation();
    this->update_adjacency();
    this->update_bbox();

    this->v_data.resize(this->num_verts());
    this->e_data.resize(this->num_edges());
    this->f_data.resize(this->num_faces());
    this->p_data.resize(this->num_polys());

    this->update_f_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::update_adjacency()
{
    this->v2v.clear(); v2v.resize(num_verts());
    v2e.clear(); v2e.resize(num_verts());
    v2f.clear(); v2f.resize(num_verts());
    v2p.clear(); v2p.resize(num_verts());
    f2e.clear(); f2e.resize(num_faces());
    f2f.clear(); f2f.resize(num_faces());
    f2p.clear(); f2p.resize(num_faces());
    p2v.clear(); p2v.resize(num_polys());
    p2e.clear(); p2e.resize(num_polys());
    p2p.clear(); p2p.resize(num_polys());

    std::map<ipair,std::set<uint>> e2f_map;
    for(uint pid=0; pid<num_polys(); ++pid)
    {
        std::set<uint> poly_verts; //unique list
        for(int signed_fid : polys.at(pid))
        {
            uint fid = (signed_fid > 0) ? signed_fid : -signed_fid;
            f2p.at(fid).push_back(pid);
            uint nv = verts_per_face(fid);
            for(uint off=0; off<nv; ++off)
            {
                uint vid = face_vert_id(fid, off);
                ipair e  = unique_pair(vid,face_vert_id(fid,(off+1)%nv));
                e2f_map[e].insert(fid);
                poly_verts.insert(vid);
            }
        }
        for(uint vid : poly_verts) v2p.at(vid).push_back(pid);
        std::copy(poly_verts.begin(), poly_verts.end(), std::back_inserter(p2v.at(pid)));
    }

    for(uint fid=0; fid<num_faces(); ++fid)
    {
        for(uint off=0; off<verts_per_face(fid); ++off)
        {
            uint vid = face_vert_id(fid,off);
            v2f.at(vid).push_back(fid);
        }
    }

    edges.clear();
    e2f.clear(); e2f.resize(e2f_map.size());
    e2p.clear(); e2p.resize(e2f_map.size());

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

    for(uint eid=0; eid<num_edges(); ++eid)
    {
        std::set<uint> edge_polys;
        for(uint fid : e2f.at(eid))
        for(uint pid : f2p.at(fid))
        {
            edge_polys.insert(pid);
        }
        for(uint pid : edge_polys) p2e.at(pid).push_back(eid);
        std::copy(edge_polys.begin(), edge_polys.end(), std::back_inserter(e2p.at(eid)));
    }

    for(uint fid=0; fid<num_faces(); ++fid)
    {
        assert(f2p.at(fid).size() < 3);
        for(uint i=0;   i<f2p.at(fid).size()-1; ++i)
        for(uint j=i+1; j<f2p.at(fid).size();   ++j)
        {
            p2p.at(f2p.at(fid).at(i)).push_back(f2p.at(fid).at(j));
            p2p.at(f2p.at(fid).at(j)).push_back(f2p.at(fid).at(i));
        }
    }

    v_on_srf = std::vector<bool>(num_verts(),false);
    e_on_srf = std::vector<bool>(num_edges(),false);
    f_on_srf = std::vector<bool>(num_faces(),false);

    for(uint fid=0; fid<num_faces(); ++fid)
    {
        if (f2p.at(fid).size() == 1)
        {
            f_on_srf.at(fid) = true;
            for(uint eid : f2e.at(fid)) e_on_srf.at(eid) = true;
            for(uint off=0; off<verts_per_face(fid); ++off) v_on_srf.at(face_vert_id(fid,off)) = true;
        }
    }

    logger << num_verts() << "\tverts" << endl;
    logger << num_edges() << "\tedges" << endl;
    logger << num_faces() << "\tfaces" << endl;
    logger << num_polys() << "\tpolys" << endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::update_face_tessellation()
{
    triangulated_faces.resize(num_faces());
    std::set<uint> bad_faces;

    for(uint fid=0; fid<num_faces(); ++fid)
    {
        // TODO: improve triangulation strategy (this assumes convexity!)
        std::vector<vec3d> n;
        for (uint i=2; i<verts_per_face(fid); ++i)
        {
            uint vid0 = faces.at(fid).at( 0 );
            uint vid1 = faces.at(fid).at(i-1);
            uint vid2 = faces.at(fid).at( i );

            triangulated_faces.at(fid).push_back(vid0);
            triangulated_faces.at(fid).push_back(vid1);
            triangulated_faces.at(fid).push_back(vid2);

            n.push_back((vert(vid1)-vert(vid0)).cross(vert(vid2)-vert(vid0)));
        }
        // check for badly tessellated polygons...
        for(uint i=0; i<n.size()-1; ++i) if (n.at(i).dot(n.at(i+1))<0) bad_faces.insert(fid);
    }
    //
    for(uint fid : bad_faces)
    {
        std::cerr << "WARNING : Bad tessellation occurred for non-convex polygon " << fid << std::endl;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Polyhedralmesh<M,V,E,F,P>::poly_face_id(const uint pid, const uint off) const
{
    return polys.at(pid).at(off);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
bool Polyhedralmesh<M,V,E,F,P>::poly_face_is_CW(const uint pid, const uint off) const
{
    return (polys_face_winding.at(pid).at(off) == true);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
bool Polyhedralmesh<M,V,E,F,P>::poly_face_is_CCW(const uint pid, const uint off) const
{
    return (polys_face_winding.at(pid).at(off) == false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
uint Polyhedralmesh<M,V,E,F,P>::poly_face_offset(const uint pid, const uint fid) const
{
    for(uint off=0; off<polys.at(pid).size(); ++off)
    {
        if (poly_face_id(pid,off) == fid) return off;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_on_surf(const uint pid) const
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
uint Polyhedralmesh<M,V,E,F,P>::edge_vert_id(const uint eid, const uint offset) const
{
    uint   eid_ptr = eid * 2;
    return edges.at(eid_ptr + offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,P>::edge_vert(const uint eid, const uint offset) const
{
    return vert(edge_vert_id(eid,offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,P>::poly_centroid(const uint pid) const
{
    vec3d c(0,0,0);
    for(uint vid : adj_p2v(pid)) c += vert(vid);
    c /= static_cast<double>(verts_per_poly(pid));
    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::update_f_normals()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        assert(verts_per_face(fid)>2);

        // compute the best fitting plane
        std::vector<vec3d> points;
        for(uint off=0; off<verts_per_face(fid); ++off) points.push_back(face_vert(fid,off));
        Plane best_fit(points);

        // adjust orientation (n or -n?)
        vec3d v0 = face_vert(fid,0);
        vec3d v1 = face_vert(fid,1);
        uint  i=2;
        vec3d ccw;
        do { ccw = (v1-v0).cross(face_vert(fid,i)-v0); } while (ccw.length_squared()==0 && i<verts_per_face(fid));

        face_data(fid).normal = (best_fit.n.dot(ccw) < 0) ? -best_fit.n : best_fit.n;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,P>::face_vert(const uint fid, const uint off) const
{
    return vert(face_vert_id(fid,off));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
uint Polyhedralmesh<M,V,E,F,P>::face_vert_id(const uint fid, const uint off) const
{
    return faces.at(fid).at(off);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::face_is_on_srf(const uint fid) const
{
    return f_on_srf.at(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::edge_is_on_srf(const uint eid) const
{
    return e_on_srf.at(eid);
}

}
