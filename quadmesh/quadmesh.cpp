/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
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
#include "quadmesh.h"
#include "../bfs.h"
#include "../timer.h"
#include "../io/read_write.h"

#include <queue>

namespace cinolib
{

CINO_INLINE
Quadmesh::Quadmesh(const char * filename)
{
    timer_start("load");

    load(filename);
    init();

    timer_stop("load");
}

CINO_INLINE
Quadmesh::Quadmesh(const std::vector<double> & coords,
                   const std::vector<u_int>  & quads)
{
    clear();
    this->coords = coords;
    this->quads   = quads;
    logger << "quadmesh init call" << endl;
    init();
    logger << "quadmesh init ok" << endl;
}

CINO_INLINE
void Quadmesh::clear()
{
    bb.reset();
    coords.clear();
    quads.clear();
    edges.clear();
    u_text.clear();
    q_label.clear();
    v_norm.clear();
    q_norm.clear();
    vtx2vtx.clear();
    vtx2quad.clear();
    vtx2edg.clear();
    quad2quad.clear();
    quad2edg.clear();
    edg2quad.clear();
}

CINO_INLINE
void Quadmesh::init()
{
    update_adjacency();
    update_bbox();
    update_normals();

    u_text.resize(num_vertices(), 0.0);
    if (q_label.empty()) q_label.resize(num_quads(), 0);
}

CINO_INLINE
void Quadmesh::update_adjacency()
{
    timer_start("Build adjacency");

    edges.clear();

    vtx2vtx.clear();
    vtx2edg.clear();
    edg2quad.clear();
    quad2quad.clear();
    quad2edg.clear();
    vtx2quad.clear();

    vtx2quad.resize(num_vertices());

    typedef std::map<ipair, std::vector<int> > mymap;
    mymap edge_quad_map;

    for(int qid=0; qid<num_quads(); ++qid)
    {
        int qid_ptr = qid * 4;

        for(int i=0; i<4; ++i)
        {
            int  vid0 = quads[qid_ptr + i];
            int  vid1 = quads[qid_ptr + (i+1)%4];

            vtx2quad[vid0].push_back(qid);

            ipair e = unique_pair(vid0, vid1);
            edge_quad_map[e].push_back(qid);
        }
    }

    edg2quad.resize(edge_quad_map.size());
    quad2edg.resize(num_quads());
    quad2quad.resize(num_quads());
    vtx2vtx.resize(num_vertices());
    vtx2edg.resize(num_vertices());

    int fresh_id = 0;
    for(mymap::iterator it=edge_quad_map.begin(); it!=edge_quad_map.end(); ++it)
    {
        ipair e   = it->first;
        int  eid  = fresh_id++;
        int  vid0 = e.first;
        int  vid1 = e.second;

        edges.push_back(vid0);
        edges.push_back(vid1);

        vtx2vtx[vid0].push_back(vid1);
        vtx2vtx[vid1].push_back(vid0);

        vtx2edg[vid0].push_back(eid);
        vtx2edg[vid1].push_back(eid);

        std::vector<int> qids = it->second;
        //assert(qids.size() <= 2 && "Non manifold edge!");
        //assert(qids.size() >= 1 && "Non manifold edge!");
        //if (qids.size() > 2 || qids.size() < 1) cerr << "Non manifold edge! " << edge_vertex(eid, 0) << "\t" << edge_vertex(eid, 1) << endl;

        for(int i=0; i<(int)qids.size(); ++i)
        {
            int qid = qids[i];

            quad2edg[qid].push_back(eid);
            edg2quad[eid].push_back(qid);
        }
        if (qids.size() == 2)
        {
            quad2quad[qids[0]].push_back(qids[1]);
            quad2quad[qids[1]].push_back(qids[0]);
            assert(quad2quad[qids[0]].size() <= 4);
            assert(quad2quad[qids[1]].size() <= 4);
        }
    }

    logger << num_vertices()  << "\tvertices"  << endl;
    logger << num_quads()     << "\tquads"     << endl;
    logger << num_edges()     << "\tedges"     << endl;

    timer_stop("Build adjacency");
}

CINO_INLINE
void Quadmesh::update_q_normals()
{
    // TO UPDATE!!!!!!

    q_norm.clear();
    q_norm.resize(num_quads()*3);

    for(int qid=0; qid<num_quads(); ++qid)
    {
        int qid_vid_ptr = qid * 4;

        vec3d v0 = vertex(quads[qid_vid_ptr+0]);
        vec3d v1 = vertex(quads[qid_vid_ptr+1]);
        vec3d v2 = vertex(quads[qid_vid_ptr+2]);

        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        int qid_norm_ptr = qid * 3;
        q_norm[qid_norm_ptr + 0] = n.x();
        q_norm[qid_norm_ptr + 1] = n.y();
        q_norm[qid_norm_ptr + 2] = n.z();
    }
}

CINO_INLINE
void Quadmesh::update_v_normals()
{
    v_norm.clear();
    v_norm.resize(num_vertices()*3);

    for(int vid=0; vid<num_vertices(); ++vid)
    {
        std::vector<int> nbrs = adj_vtx2quad(vid);

        vec3d sum(0,0,0);
        for(int i=0; i<(int)nbrs.size(); ++i)
        {
            sum += quad_normal(nbrs[i]);
        }

        //assert(nbrs.size() > 0);
        sum /= nbrs.size();
        sum.normalize();

        int vid_ptr = vid * 3;
        v_norm[vid_ptr + 0] = sum.x();
        v_norm[vid_ptr + 1] = sum.y();
        v_norm[vid_ptr + 2] = sum.z();
    }
}

CINO_INLINE
std::string Quadmesh::loaded_file() const
{
    return filename;
}

CINO_INLINE
int Quadmesh::num_vertices() const
{
    return coords.size()/3;
}

CINO_INLINE
int Quadmesh::num_quads() const
{
    return quads.size()/4;
}

CINO_INLINE
int Quadmesh::num_elements() const
{
    return quads.size()/4;
}

CINO_INLINE
int Quadmesh::num_edges() const
{
    return edges.size()/2;
}

CINO_INLINE
const std::vector<int> &Quadmesh::adj_vtx2vtx(const int vid) const
{
    return vtx2vtx.at(vid);
}

CINO_INLINE
const std::vector<int> &Quadmesh::adj_vtx2edg(const int vid) const
{
    return vtx2edg.at(vid);
}

CINO_INLINE
const std::vector<int> &Quadmesh::adj_vtx2quad(const int vid) const
{
    return vtx2quad.at(vid);
}

CINO_INLINE
const std::vector<int> &Quadmesh::adj_edg2quad(const int eid) const
{
    return edg2quad.at(eid);
}

CINO_INLINE
const std::vector<int> &Quadmesh::adj_quad2edg(const int qid) const
{
    return quad2edg.at(qid);
}



CINO_INLINE
const std::vector<int> &Quadmesh::adj_quad2quad(const int qid) const
{
    return quad2quad.at(qid);
}

CINO_INLINE
void Quadmesh::load(const char * filename)
{
    timer_start("Load Quadmesh");

    clear();

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        std::vector<uint> tris; // ignored here
        read_OFF(filename, coords, tris, quads);
    }
    else
    if (filetype.compare("obj") == 0 ||
        filetype.compare("OBJ") == 0)
    {
        std::vector<uint> tris; // ignored here
        read_OBJ(filename, coords, tris, quads);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    logger << quads.size() / 4   << " quads read" << endl;
    logger << coords.size() / 3  << " vertices  read" << endl;

    this->filename = std::string(filename);

    timer_stop("Load Quadmesh");
}

CINO_INLINE
void Quadmesh::save(const char * filename) const
{
    timer_start("Save Quadmesh");

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        std::vector<uint> tris; // ignored here
        write_OFF(filename, coords, tris, quads);
    }
    else
    if (filetype.compare("obj") == 0 ||
        filetype.compare("OBJ") == 0)
    {
        std::vector<uint> tris; // ignored here
        write_OBJ(filename, coords, tris, quads);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save() : file format not supported yet " << endl;
        exit(-1);
    }

    logger << quads.size() / 4   << " quads written" << endl;
    logger << coords.size() / 3  << " vertices  written" << endl;

    timer_stop("Save Quadmesh");
}

CINO_INLINE
int Quadmesh::edge_vertex_id(const int eid, const int offset) const
{
    int eid_ptr = eid * 2;
    return edges.at(eid_ptr + offset);
}

CINO_INLINE
int Quadmesh::quad_vertex_id(const int qid, const int offset) const
{
    int qid_ptr = qid * 4;
    return quads.at(qid_ptr + offset);
}

CINO_INLINE
vec3d Quadmesh::quad_normal(const int qid) const
{
    int qid_ptr = qid * 3;
    return vec3d(q_norm.at(qid_ptr + 0), q_norm.at(qid_ptr + 1), q_norm.at(qid_ptr + 2));
}

CINO_INLINE
vec3d Quadmesh::vertex_normal(const int vid) const
{
    int vid_ptr = vid * 3;
    return vec3d(v_norm.at(vid_ptr+0), v_norm.at(vid_ptr+1), v_norm.at(vid_ptr+2));
}

CINO_INLINE
vec3d Quadmesh::vertex(const int vid) const
{
    int vid_ptr = vid * 3;
    return vec3d(coords.at(vid_ptr+0), coords.at(vid_ptr+1), coords.at(vid_ptr+2));
}

CINO_INLINE
void Quadmesh::set_vertex(const int vid, const vec3d &pos)
{
    int vid_ptr = vid * 3;
    coords.at(vid_ptr + 0) = pos.x();
    coords.at(vid_ptr + 1) = pos.y();
    coords.at(vid_ptr + 2) = pos.z();
}

CINO_INLINE
int Quadmesh::vertex_valence(const int vid) const
{
    return adj_vtx2vtx(vid).size();
}

CINO_INLINE
float Quadmesh::vertex_u_text(const int vid) const
{
    return u_text.at(vid);
}

CINO_INLINE
void Quadmesh::set_vertex_u_text(const int vid, const float s)
{
    u_text.at(vid) = s;
}

CINO_INLINE
int Quadmesh::quad_label(const int qid) const
{
    return q_label.at(qid);
}

CINO_INLINE
void Quadmesh::quad_set_label(const int qid, const int i)
{
    q_label.at(qid) = i;
}

CINO_INLINE
void Quadmesh::update_normals()
{
    update_q_normals();
    update_v_normals();
}

CINO_INLINE
bool Quadmesh::quad_contains_vertex(const int qid, const int vid) const
{
    if (quad_vertex_id(qid, 0) == vid) return true;
    if (quad_vertex_id(qid, 1) == vid) return true;
    if (quad_vertex_id(qid, 2) == vid) return true;
    if (quad_vertex_id(qid, 3) == vid) return true;
    return false;
}

CINO_INLINE
void Quadmesh::update_bbox()
{
    bb.reset();
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        vec3d v = vertex(vid);
        bb.min = bb.min.min(v);
        bb.max = bb.max.max(v);
    }
}

CINO_INLINE
vec3d Quadmesh::element_barycenter(const int qid) const
{
    vec3d b(0,0,0);
    for(int i=0; i<4; ++i)
    {
        b += quad_vertex(qid,i);
    }
    b /= 4.0;
    return b;
}

CINO_INLINE
vec3d Quadmesh::quad_vertex(const int qid, const int offset) const
{
    int qid_ptr = qid * 4;
    int vid     = quads.at(qid_ptr + offset);
    int vid_ptr = vid * 3;
    return vec3d(coords.at(vid_ptr + 0), coords.at(vid_ptr + 1), coords.at(vid_ptr + 2));
}

CINO_INLINE
vec3d Quadmesh::edge_vertex(const int eid, const int offset) const
{
    int eid_ptr = eid * 2;
    int vid     = edges.at(eid_ptr + offset);
    int vid_ptr = vid * 3;
    return vec3d(coords.at(vid_ptr + 0), coords.at(vid_ptr + 1), coords.at(vid_ptr + 2));
}

CINO_INLINE
const std::vector<double> &Quadmesh::vector_coords() const
{
    return coords;
}

CINO_INLINE
const std::vector<u_int> &Quadmesh::vector_quads() const
{
    return quads;
}

CINO_INLINE
const std::vector<u_int> &Quadmesh::vector_edges() const
{
    return edges;
}


CINO_INLINE
const Bbox &Quadmesh::bbox() const
{
    return bb;
}

CINO_INLINE
const std::vector<float> &Quadmesh::vector_v_float_scalar() const
{
    return u_text;
}

CINO_INLINE
const std::vector<int> &Quadmesh::vector_q_int_scalar() const
{
    return q_label;
}

}
