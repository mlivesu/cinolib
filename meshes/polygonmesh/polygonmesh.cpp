/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include <cinolib/meshes/polygonmesh/polygonmesh.h>
#include <cinolib/common.h>
#include <cinolib/io/read_write.h>
#include <map>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Polygonmesh<M,V,E,F>::Polygonmesh(const char * filename)
{
    load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Polygonmesh<M,V,E,F>::Polygonmesh(const std::vector<vec3d>             & verts,
                                  const std::vector<std::vector<uint>> & faces)
: verts(verts)
, faces(faces)
{
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Polygonmesh<M,V,E,F>::Polygonmesh(const std::vector<double>            & coords,
                                  const std::vector<std::vector<uint>> & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->faces = faces;

    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::load(const char * filename)
{
    timer_start("Load Polygonmesh");

    clear();
    std::vector<double> coords;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare(".off") == 0 ||
        filetype.compare(".OFF") == 0)
    {
        read_OFF(filename, coords, faces);
    }
    else if (filetype.compare(".obj") == 0 ||
             filetype.compare(".OBJ") == 0)
    {
        read_OBJ(filename, coords, faces);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    verts = vec3d_from_serialized_xyz(coords);

    logger << num_faces() << " polygons read" << endl;
    logger << num_verts() << " vertices read" << endl;

    this->mesh_data().filename = std::string(filename);

    timer_stop("Load Polygonmesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::save(const char * filename) const
{
    timer_start("Save Polygonmesh");

    std::vector<double> coords = serialized_xyz_from_vec3d(verts);

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        write_OFF(filename, coords, faces);
    }
    else if (filetype.compare(".obj") == 0 ||
             filetype.compare(".OBJ") == 0)
    {
        write_OBJ(filename, coords, faces);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << endl;
        exit(-1);
    }

    timer_stop("Save Polygonmesh");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::clear()
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
void Polygonmesh<M,V,E,F>::init()
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
void Polygonmesh<M,V,E,F>::update_adjacency()
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
        for(uint i=0; i<verts_per_face(fid); ++i)
        {
            int  vid0 = faces.at(fid).at(i);
            int  vid1 = faces.at(fid).at((i+1)%verts_per_face(fid));
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
void Polygonmesh<M,V,E,F>::update_bbox()
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
void Polygonmesh<M,V,E,F>::update_f_normals()
{
    for(uint fid=0; fid<num_faces(); ++fid) // TODO: make it more accurate!
    {
        vec3d v0 = face_vert(fid,0);
        vec3d v1 = face_vert(fid,1);
        vec3d v2 = face_vert(fid,2);
        vec3d n  = (v1-v0).cross(v2-v0);
        n.normalize();
        face_data(fid).normal = n;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::update_v_normals()
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
void Polygonmesh<M,V,E,F>::update_normals()
{
    update_f_normals();
    update_v_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Polygonmesh<M,V,E,F>::face_vert_id(const uint fid, const uint offset) const
{
    return faces.at(fid).at(offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Polygonmesh<M,V,E,F>::face_vert(const uint fid, const uint offset) const
{
    return vert(faces.at(fid).at(offset));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Polygonmesh<M,V,E,F>::face_centroid(const uint fid) const
{
    vec3d c(0,0,0);
    for(uint vid : faces.at(fid))
    {
        c += vert(vid);
    }
    c /= static_cast<double>(verts_per_face(fid));
    return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Polygonmesh<M,V,E,F>::elem_centroid(const uint fid) const
{
    return face_centroid(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Polygonmesh<M,V,E,F>::edge_vert_id(const uint eid, const uint offset) const
{
    uint   eid_ptr = eid * 2;
    return edges.at(eid_ptr + offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Polygonmesh<M,V,E,F>::edge_vert(const uint eid, const uint offset) const
{
    uint eid_ptr = eid * 2;
    uint vid     = edges.at(eid_ptr + offset);
    return vert(vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::elem_show_all()
{
    for(uint fid=0; fid<num_faces(); ++fid)
    {
        face_data(fid).visible = true;
    }
}

}
