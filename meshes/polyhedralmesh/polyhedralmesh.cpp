/*******************************************************************************
*  Copyright(C) 2016, Marco Livesu                                             *
*  All rights reserved.                                                        *
*                                                                              *
*  This file is part of CinoLib                                                *
*                                                                              *
*  CinoLib is free software; you can redistribute it and/or modify             *
*  it under the terms of the GNU General Public License as published by        *
*  the Free Software Foundation; either version 3 of the License, or           *
*  (at your option) any later version.                                         *
*                                                                              *
*  This program is distributed in the hope that it will be useful,             *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*  GNU General Public License (http://www.gnu.org/licenses/gpl.txt)            *
*  for more details.                                                           *
*                                                                              *
*  Contact:                                                                    *
*     Marco Livesu                                                             *
*     email:    marco.livesu@gmail.com                                         *
*     homepage: http://pers.ge.imati.cnr.it/livesu/                            *
*                                                                              *
*     Italian National Research Council (CNR)                                  *
*     Institute for Applied Mathematics and Information Technologies (IMATI)   *
*     Via de Marini, 6                                                         *
*     16149 Genoa,                                                             *
*     Italy                                                                    *
*******************************************************************************/
#include <cinolib/meshes/polyhedralmesh/polyhedralmesh.h>
#include <cinolib/timer.h>
#include <cinolib/io/read_write.h>
#include <cinolib/common.h>

#include <algorithm>
#include <float.h>
#include <map>
#include <set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
Polyhedralmesh<M,V,E,F,C>::Polyhedralmesh(const std::vector<vec3d>             & verts,
                                          const std::vector<std::vector<uint>> & faces,
                                          const std::vector<std::vector<uint>> & cells)
: verts(verts)
, faces(faces)
, cells(cells)
{
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
Polyhedralmesh<M,V,E,F,C>::Polyhedralmesh(const std::vector<double>            & coords,
                                          const std::vector<std::vector<uint>> & faces,
                                          const std::vector<std::vector<uint>> & cells)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->faces = faces;
    this->cells = cells;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
Polyhedralmesh<M,V,E,F,C>::Polyhedralmesh(const char * filename)
{
    timer_start("load Polyhedralmesh");

    load(filename);
    init();

    timer_stop("load Polyhedralmesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::load(const char * filename)
{
    timer_start("Load Polyhedralmesh");

    clear();
    std::vector<double> coords;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-6,6);

    if (filetype.compare("hybrid") == 0 ||
        filetype.compare("HYBRID") == 0)
    {
        read_HYBRID(filename, coords, faces, cells);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    verts = vec3d_from_serialized_xyz(coords);

    logger << num_cells() << " polyhedra read" << endl;
    logger << num_faces() << " faces     read" << endl;
    logger << num_verts() << " vertices  read" << endl;

    this->mesh_data().filename = std::string(filename);

    timer_stop("Load Polyhedralmesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::save(const char * filename) const
{
    timer_start("Save Polyhedralmesh");

    std::vector<double> coords = serialized_xyz_from_vec3d(verts);

    std::string str(filename);
    std::string filetype = str.substr(str.size()-6,6);

    if (filetype.compare("hybrid") == 0 ||
        filetype.compare("HYBRID") == 0)
    {
        write_HYBRID(filename, coords, faces, cells);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << endl;
        exit(-1);
    }

    timer_stop("Save Polyhedralmesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::clear()
{
    bb.reset();
    //
    verts.clear();
    edges.clear();
    faces.clear();
    cells.clear();
    v_on_srf.clear();
    e_on_srf.clear();
    f_on_srf.clear();
    //
    M tmp;
    m_data = tmp;;
    v_data.clear();
    e_data.clear();
    f_data.clear();
    c_data.clear();
    //
    v2v.clear();
    v2e.clear();
    v2f.clear();
    v2c.clear();
    e2f.clear();
    e2c.clear();
    f2e.clear();
    f2f.clear();
    f2c.clear();
    c2v.clear();
    c2e.clear();
    c2c.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::init()
{
    update_bbox();
    update_adjacency();

    v_data.resize(num_verts());
    e_data.resize(num_edges());
    c_data.resize(num_cells());
    f_data.resize(num_faces());

    update_face_normals();

    set_uvw_from_xyz(UVW_param);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::update_bbox()
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

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::update_adjacency()
{
    timer_start("Build adjacency");

    v2v.clear(); v2v.resize(num_verts());
    v2e.clear(); v2e.resize(num_verts());
    v2f.clear(); v2f.resize(num_verts());
    v2c.clear(); v2c.resize(num_verts());
    f2e.clear(); f2e.resize(num_faces());
    f2f.clear(); f2f.resize(num_faces());
    f2c.clear(); f2c.resize(num_faces());
    c2v.clear(); c2v.resize(num_cells());
    c2e.clear(); c2e.resize(num_cells());
    c2c.clear(); c2c.resize(num_cells());

    std::map<ipair,std::vector<uint>>             e2f_map;
    std::map<std::vector<uint>,std::vector<uint>> f2c_map;
    for(uint cid=0; cid<num_cells(); ++cid)
    {
        std::vector<uint> sorted_f = cells.at(cid);
        std::sort(sorted_f.begin(), sorted_f.end());
        f2c_map[sorted_f].push_back(cid);

        std::set<uint> cell_verts; //unique list
        for(uint fid : cells.at(cid))
        {
            f2c.at(fid).push_back(cid);

            uint nv = verts_per_face(fid);
            for(uint off=0; off<nv; ++off)
            {
                uint vid = face_vert_id(fid, off);
                ipair e  = unique_pair(vid,face_vert_id(fid,(off+1)%nv));
                e2f_map[e].push_back(cid);

                cell_verts.insert(vid);
            }
        }

        for(uint vid : cell_verts) v2c.at(vid).push_back(cid);
        std::copy(cell_verts.begin(), cell_verts.end(), std::back_inserter(c2v.at(cid)));
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
    e2c.clear(); e2c.reserve(e2f_map.size());

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
        for(uint i=0; i<fids.size(); ++i)
        {
            uint fid = fids.at(i);

            f2e.at(fid).push_back(eid);
            e2f.at(eid).push_back(fid);

            for(uint j=i+1; j<fids.size(); ++j)
            {
                f2f.at(fid).push_back(nbr);
                f2f.at(nbr).push_back(fid);
            }
        }
    }

    for(uint eid=0; eid<num_edges(); ++eid)
    {
        std::set<uint> edge_cells;
        for(uint fid : e2f.at(eid))
        for(uint cid : f2c.at(fid))
        {
            edge_cells.insert(cid);
        }
        for(uint cid : edge_cells) c2e.at(cid).push_back(eid);
        std::copy(edge_cells.begin(), edge_cells.end(), std::back_inserter(e2c.at(eid)));
    }

    for(auto f2c_it : f2c_map)
    {
        for(uint i=0; i<f2c_it.second.size()-1; ++i)
        {
            uint cid = f2c_it.second.at(i);
            for(uint j=i+1; j<f2c_it.second.size(); ++j)
            {
                uint nbr = f2c_it.second.at(j);
                c2c.at(cid).push_back(nbr);
                c2c.at(nbr).push_back(cid);
            }
        }
    }

    v_on_srf = std::vector<bool>(num_verts(),false);
    e_on_srf = std::vector<bool>(num_edges(),false);
    f_on_srf = std::vector<bool>(num_faces(),false);

    for(uint fid=0; fid<num_faces(); ++fid)
    {
        if (f2c.at(fid).size() == 1)
        {
            f_on_srf.at(fid) = true;
            for(uint eid : f2e.at(fid)) e_on_srf.at(eid) = true;
            for(uint off=0; off<verts_per_face(fid); ++off) v_on_srf.at(face_vert_id(fid,off)) = true;
        }
    }

    logger << num_verts() << "\tverts" << endl;
    logger << num_edges() << "\tedges" << endl;
    logger << num_faces() << "\tfaces" << endl;
    logger << num_cells() << "\tcells" << endl;

    timer_stop("Build adjacency");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::update_face_normals()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        vec3d v0 = face_vert(fid,0);
        vec3d v1 = face_vert(fid,1);
        vec3d v2 = face_vert(fid,2);

        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        face_data(fid).normal = n;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
int Polyhedralmesh<M,V,E,F,C>::cell_shared_face(const uint cid0, const uint cid1) const
{
//    for(uint f=0; f<faces_per_cell(); ++f)
//    {
//        if (cell_contains_vert(cid1, cell_vert_id(cid0, HEXA_FACES[f][0])) &&
//            cell_contains_vert(cid1, cell_vert_id(cid0, HEXA_FACES[f][1])) &&
//            cell_contains_vert(cid1, cell_vert_id(cid0, HEXA_FACES[f][2])) &&
//            cell_contains_vert(cid1, cell_vert_id(cid0, HEXA_FACES[f][3])) )
//        {
//            return f;
//        }
//    }
//    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,C>::cell_contains_vert(const uint cid, const uint vid) const
{
//    for(uint i=0; i<verts_per_cell(); ++i)
//    {
//        if (cell_vert_id(cid,i) == vid) return true;
//    }
//    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,C>::cell_centroid(const uint cid) const
{
//    vec3d c(0,0,0);
//    for(uint off=0; off<verts_per_cell(); ++off)
//    {
//        c += cell_vert(cid,off);
//    }
//    c /= static_cast<double>(verts_per_cell());
//    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,C>::elem_centroid(const uint cid) const
{
    return cell_centroid(cid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
uint Polyhedralmesh<M,V,E,F,C>::cell_vert_id(const uint cid, const uint off) const
{
//    uint cid_ptr = cid * verts_per_cell();
//    return cells.at(cid_ptr + off);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
uint Polyhedralmesh<M,V,E,F,C>::cell_edge_id(const uint cid, const uint vid0, const uint vid1) const
{
    assert(cell_contains_vert(cid,vid0));
    assert(cell_contains_vert(cid,vid1));

    ipair query = unique_pair(vid0,vid1);
    for(uint eid : adj_c2e(cid))
    {
        if (unique_pair(edge_vert_id(eid,0), edge_vert_id(eid,1)) == query) return eid;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,C>::cell_vert(const uint cid, const uint off) const
{
//    return verts.at(cell_vert_id(cid,off));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,C>::face_centroid(const uint fid) const
{
//    vec3d c(0,0,0);
//    for(uint off=0; off<verts_per_face(); ++off) c += face_vert(fid,off);
//    c /= static_cast<double>(verts_per_face());
//    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
uint Polyhedralmesh<M,V,E,F,C>::face_vert_id(const uint fid, const uint off) const
{
//    uint fid_ptr = fid * verts_per_face();
//    return faces.at(fid_ptr + off);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,C>::face_vert(const uint fid, const uint off) const
{
    return verts.at(face_vert_id(fid,off));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
uint Polyhedralmesh<M,V,E,F,C>::face_edge_id(const uint fid, const uint vid0, const uint vid1) const
{
    assert(face_contains_vert(fid,vid0));
    assert(face_contains_vert(fid,vid1));

    ipair query = unique_pair(vid0,vid1);
    for(uint eid : adj_f2e(fid))
    {
        if (unique_pair(edge_vert_id(eid,0), edge_vert_id(eid,1)) == query) return eid;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,C>::face_contains_vert(const uint fid, const uint vid) const
{
//    for(uint i=0; i<verts_per_face(); ++i)
//    {
//        if (face_vert_id(fid,i) == vid) return true;
//    }
//    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
uint Polyhedralmesh<M,V,E,F,C>::edge_vert_id(const uint eid, const uint off) const
{
    uint eid_ptr = 2*eid;
    return edges.at(eid_ptr + off);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
vec3d Polyhedralmesh<M,V,E,F,C>::edge_vert(const uint eid, const uint off) const
{
    return verts.at(edge_vert_id(eid,off));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::elem_show_all()
{
    for(uint cid=0; cid<num_cells(); ++cid)
    {
        cell_data(cid).visible = true;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::vert_set_color(const Color & c)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::vert_set_alpha(const float alpha)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        vert_data(vid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::edge_set_color(const Color & c)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::edge_set_alpha(const float alpha)
{
    for(uint eid=0; eid<num_edges(); ++eid)
    {
        edge_data(eid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::face_set_color(const Color & c)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::face_set_alpha(const float alpha)
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::cell_set_color(const Color & c)
{
    for(uint cid=0; cid<num_cells(); ++cid)
    {
        cell_data(cid).color = c;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::cell_set_alpha(const float alpha)
{
    for(uint cid=0; cid<num_cells(); ++cid)
    {
        cell_data(cid).color.a = alpha;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,C>::edge_is_on_srf(const uint eid) const
{
    return e_on_srf.at(eid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
std::vector<float> Polyhedralmesh<M,V,E,F,C>::export_uvw_param(const int mode) const
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

template<class M, class V, class E, class F, class C>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,C>::set_uvw_from_xyz(const int mode)
{
    for(uint vid=0; vid<num_verts(); ++vid)
    {
        switch (mode)
        {
            case U_param  : vert_data(vid).uvw[0] = static_cast<float>(vert(vid).x()); break;
            case V_param  : vert_data(vid).uvw[1] = static_cast<float>(vert(vid).y()); break;
            case W_param  : vert_data(vid).uvw[2] = static_cast<float>(vert(vid).z()); break;
            case UV_param : vert_data(vid).uvw[0] = static_cast<float>(vert(vid).x());
                            vert_data(vid).uvw[1] = static_cast<float>(vert(vid).y()); break;
            case UW_param : vert_data(vid).uvw[0] = static_cast<float>(vert(vid).x());
                            vert_data(vid).uvw[2] = static_cast<float>(vert(vid).z()); break;
            case VW_param : vert_data(vid).uvw[1] = static_cast<float>(vert(vid).y());
                            vert_data(vid).uvw[2] = static_cast<float>(vert(vid).z()); break;
            case UVW_param: vert_data(vid).uvw[0] = static_cast<float>(vert(vid).z());
                            vert_data(vid).uvw[1] = static_cast<float>(vert(vid).y());
                            vert_data(vid).uvw[2] = static_cast<float>(vert(vid).z()); break;
            default: assert(false);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class C>
CINO_INLINE
std::vector<double> Polyhedralmesh<M,V,E,F,C>::vector_coords() const
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

}
