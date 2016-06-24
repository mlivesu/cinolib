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
#include "hexmesh.h"
#include "../timer.h"

#include <float.h>
#include <map>
#include <set>

#include "../io/read_write.h"

namespace cinolib
{


CINO_INLINE
Hexmesh::Hexmesh(const char * filename)
{
    timer_start("load hexmesh");

    load(filename);
    init();

    timer_stop("load hexmesh");
}

CINO_INLINE
void Hexmesh::load(const char * filename)
{
    timer_start("Load Hexmesh");

    clear();

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
    {
        std::vector<uint> tets; // not used here
        cinolib::read_MESH(filename, coords, tets, hexa);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    logger << hexa.size()   / 8 << " hexahedra read" << endl;
    logger << coords.size() / 3 << " vertices   read" << endl;

    this->filename = std::string(filename);

    timer_stop("Load Hexmesh");
}

CINO_INLINE
void Hexmesh::save(const char * filename) const
{
    timer_start("Save Hexmesh");

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
    {
        std::vector<uint> tets; // empty
        cinolib::write_MESH(filename, coords, tets, hexa);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    timer_stop("Save Hexmesh");
}

CINO_INLINE
Hexmesh::Hexmesh(const std::vector<double> & coords,
                 const std::vector<u_int>  & hexa)
{
    clear();
    std::copy(coords.begin(), coords.end(), std::back_inserter(this->coords));
    std::copy(hexa.begin(), hexa.end(), std::back_inserter(this->hexa));
    init();
}

CINO_INLINE
const std::vector<double> &Hexmesh::vector_coords() const
{
    return coords;
}

CINO_INLINE
const std::vector<uint> &Hexmesh::vector_quads() const
{
    return quads;
}

CINO_INLINE
const std::vector<float> &Hexmesh::vector_v_float_scalar() const
{
    return u_text;
}

CINO_INLINE
const std::vector<int> &Hexmesh::vector_h_int_scalar() const
{
    return t_label;
}

CINO_INLINE
std::string Hexmesh::loaded_file() const
{
    return filename;
}

CINO_INLINE
void Hexmesh::clear()
{
    bb.reset();
    coords.clear();
    hexa.clear();
    edges.clear();
    quads.clear();
    v_on_srf.clear();
    e_on_srf.clear();
    q_norm.clear();
    u_text.clear();
    t_label.clear();
    vtx2vtx.clear();
    vtx2edg.clear();
    vtx2hexa.clear();
    vtx2quad.clear();
    edg2hexa.clear();
    hexa2edg.clear();
    hexa2hexa.clear();
    hexa2quad.clear();
    quad2hexa.clear();
}

CINO_INLINE
void Hexmesh::init()
{
    u_text.resize(num_vertices());
    t_label.resize(num_hexahedra());

    for(int vid=0; vid<num_vertices(); ++vid)
    {
        u_text[vid] = vertex(vid).y();
    }

    update_bbox();
    update_interior_adjacency();
    update_surface_adjacency();
    update_q_normals();

    logger << "BB min: " << bb.min << endl;
    logger << "BB max: " << bb.max << endl;
}

CINO_INLINE
void Hexmesh::update_bbox()
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
void Hexmesh::update_interior_adjacency()
{
    timer_start("Build adjacency");

    edges.clear();
    vtx2vtx.clear();
    vtx2edg.clear();
    vtx2hexa.clear();
    edg2hexa.clear();
    hexa2hexa.clear();
    hexa2edg.clear();

    vtx2hexa.resize(num_vertices());

    typedef std::map<ipair, std::vector<int> > mymap;
    mymap edge_hex_map;

    for(int hid=0; hid<num_hexahedra(); ++hid)
    {
        int hid_ptr = hid * 8;
        int vids[8] =
        {
            hexa[hid_ptr + 0], hexa[hid_ptr + 1], hexa[hid_ptr + 2], hexa[hid_ptr + 3],
            hexa[hid_ptr + 4], hexa[hid_ptr + 5], hexa[hid_ptr + 6], hexa[hid_ptr + 7],
        };

        for(int vid=0; vid<8; ++vid)
        {
            vtx2hexa[vids[vid]].push_back(hid);
        }

        for(int eid=0; eid<12; ++eid)
        {
            ipair e = unique_pair(vids[HEXA_EDGES[eid][0]], vids[HEXA_EDGES[eid][1]]);
            edge_hex_map[e].push_back(hid);
        }
    }

    edg2hexa.resize(edge_hex_map.size());
    hexa2edg.resize(num_hexahedra());
    hexa2hexa.resize(num_hexahedra());
    vtx2vtx.resize(num_vertices());
    vtx2edg.resize(num_vertices());

    std::set<ipair> hex_pairs;

    int fresh_id = 0;
    for(mymap::iterator it=edge_hex_map.begin(); it!=edge_hex_map.end(); ++it)
    {
        ipair e    = it->first;
        int  eid  = fresh_id++;
        int  vid0 = e.first;
        int  vid1 = e.second;

        edges.push_back(vid0);
        edges.push_back(vid1);

        vtx2vtx[vid0].push_back(vid1);
        vtx2vtx[vid1].push_back(vid0);

        vtx2edg[vid0].push_back(eid);
        vtx2edg[vid1].push_back(eid);

        std::vector<int> hids = it->second;
        //assert(tids.size() <= 2);
        //sassert(tids.size() >= 1);
        for(int i=0; i<(int)hids.size(); ++i)
        {
            int tid = hids[i];

            hexa2edg[tid].push_back(eid);
            edg2hexa[eid].push_back(tid);

            for(int j=i+1; j<(int)hids.size(); ++j)
            {
                if (shared_facet(hids[i], hids[j]) != -1)
                {
                    ipair p = unique_pair(hids[j], hids[i]);
                    if (DOES_NOT_CONTAIN(hex_pairs, p))
                    {
                        hex_pairs.insert(p);
                        hexa2hexa[hids[i]].push_back(hids[j]);
                        hexa2hexa[hids[j]].push_back(hids[i]);
                        // sanity checks
                        assert(hexa2hexa[hids[j]].size() <= 6);
                        assert(hexa2hexa[hids[i]].size() <= 6);
                    }
                }
            }
        }
    }

    logger << num_vertices()   << "\tvertices"   << endl;
    logger << num_hexahedra() <<  "\thexahedra" << endl;
    logger << num_edges()      << "\tedges"      << endl;

    timer_stop("Build adjacency");
}

CINO_INLINE
void Hexmesh::update_surface_adjacency()
{
    quads.clear();
    quad2hexa.clear();
    v_on_srf.resize(num_vertices(), false);
    e_on_srf.resize(num_edges(), false);
    hexa2quad.resize(num_hexahedra());
    vtx2quad.resize(num_vertices());

    edg2quad.resize(num_edges());
    quad2edg.clear();
    quad2quad.clear();

    timer_start("Build Surface");

    typedef std::vector<int> face;
    std::map< face,std::pair<int,int> > quad2hex_map;

    for(int hid=0; hid<num_hexahedra(); ++hid)
    {
        int hid_ptr = hid * 8;

        for(int fid=0; fid<6; ++fid)
        {
            face f;
            f.push_back(hexa[hid_ptr + HEXA_FACES[fid][0]]);
            f.push_back(hexa[hid_ptr + HEXA_FACES[fid][1]]);
            f.push_back(hexa[hid_ptr + HEXA_FACES[fid][2]]);
            f.push_back(hexa[hid_ptr + HEXA_FACES[fid][3]]);

            sort(f.begin(), f.end());

            if (CONTAINS(quad2hex_map,f)) quad2hex_map.erase(f);
            else                          quad2hex_map[f] = std::make_pair(hid,fid);
        }
    }

    for(auto it=quad2hex_map.begin(); it!=quad2hex_map.end(); ++it)
    {
        auto obj     = *it;
        int  hid     = obj.second.first;
        int  fid     = obj.second.second;
        int  hid_ptr = hid * 8;

        int vid0 = hexa[hid_ptr + HEXA_FACES[fid][0]];
        int vid1 = hexa[hid_ptr + HEXA_FACES[fid][1]];
        int vid2 = hexa[hid_ptr + HEXA_FACES[fid][2]];
        int vid3 = hexa[hid_ptr + HEXA_FACES[fid][3]];

        quads.push_back(vid0);
        quads.push_back(vid1);
        quads.push_back(vid2);
        quads.push_back(vid3);

        v_on_srf[vid0] = true;
        v_on_srf[vid1] = true;
        v_on_srf[vid2] = true;
        v_on_srf[vid3] = true;

        int fresh_id = quad2hexa.size();

        vtx2quad[vid0].push_back(fresh_id);
        hexa2quad[hid].push_back(fresh_id);
        quad2hexa.push_back(hid);

        std::vector<int> edges = adj_hexa2edg(hid);
        std::vector<int> tmp;
        quad2edg.push_back(tmp);
        assert(edges.size() == 12);
        for(size_t i=0; i<edges.size(); ++i)
        {
            int  eid   = edges[i];
            int  eid0  = edge_vertex_id(eid, 0);
            int  eid1  = edge_vertex_id(eid, 1);
            bool has_0 = (eid0 == vid0 || eid0 == vid1 || eid0 == vid2 || eid0 == vid3);
            bool has_1 = (eid1 == vid0 || eid1 == vid1 || eid1 == vid2 || eid1 == vid3);

            if (has_0 && has_1)
            {
                edg2quad[eid].push_back(fresh_id);
                quad2edg[fresh_id].push_back(eid);
            }
        }
    }

    quad2quad.resize(num_srf_quads());

    for(int eid=0; eid<num_edges(); ++eid)
    {
        std::vector<int> quads = edg2quad[eid];
        if (!(quads.empty() || quads.size() == 2))
        {
            logger << "\tedge " << eid << " is non manifold! " << quads.size() << endl;
            logger << edge_vertex(eid,0) << "\t" << edge_vertex(eid,1) << endl;
            u_text[edge_vertex_id(eid,0)] = 10.0;
            u_text[edge_vertex_id(eid,1)] = 10.0;
        }
        //assert(quads.empty() || quads.size() == 2);
        if (!quads.empty())
        {
            int t0 = quads[0];
            int t1 = quads[1];
            quad2quad[t0].push_back(t1);
            quad2quad[t1].push_back(t0);
        }

        e_on_srf[eid] = !(edg2quad[eid].empty());
        //if (e_on_srf[eid]) assert(adj_edg2tri.size() == 2);
    }

    timer_stop("Build Surface");

    logger << quads.size() / 4 << " quads were generated" << endl;
}

CINO_INLINE
void Hexmesh::update_q_normals()
{
    q_norm.clear();
    q_norm.resize(num_srf_quads()*3);

    for(int qid=0; qid<num_srf_quads(); ++qid)
    {
        int qid_vtx_ptr = qid * 4;

        vec3d v0 = vertex(quads[qid_vtx_ptr+0]);
        vec3d v1 = vertex(quads[qid_vtx_ptr+1]);
        vec3d v2 = vertex(quads[qid_vtx_ptr+2]);

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
int Hexmesh::num_vertices() const
{
    return coords.size()/3;
}

CINO_INLINE
int Hexmesh::num_hexahedra() const
{
    return hexa.size()/8;
}

CINO_INLINE
int Hexmesh::num_elements() const
{
    return hexa.size()/8;
}

CINO_INLINE
int Hexmesh::num_edges() const
{
    return edges.size()/2;
}

CINO_INLINE
int Hexmesh::num_srf_quads() const
{
    return quads.size()/4;
}

CINO_INLINE
const std::vector<int> & Hexmesh::adj_vtx2vtx(const int vid) const
{
    return vtx2vtx.at(vid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_vtx2edg(const int vid) const
{
    return vtx2edg.at(vid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_vtx2quad(const int vid) const
{
    return vtx2quad.at(vid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_vtx2hexa(const int vid) const
{
    return vtx2hexa.at(vid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_edg2hexa(const int eid) const
{
    return edg2hexa.at(eid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_edg2quad(const int eid) const
{
    return edg2quad.at(eid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_hexa2edg(const int tid) const
{
    return hexa2edg.at(tid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_hexa2hexa(const int tid) const
{
    return hexa2hexa.at(tid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_hexa2quad(const int tid) const
{
    return hexa2quad.at(tid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_quad2quad(const int tid) const
{
    return quad2quad.at(tid);
}

CINO_INLINE
const std::vector<int> &Hexmesh::adj_quad2edg(const int tid) const
{
    return quad2edg.at(tid);
}

CINO_INLINE
const int &Hexmesh::adj_quad2hexa(const int tid) const
{
    return quad2hexa.at(tid);
}

CINO_INLINE
vec3d Hexmesh::vertex(const int vid) const
{
    int vid_ptr = vid * 3;
    return vec3d(coords[vid_ptr+0], coords[vid_ptr+1], coords[vid_ptr+2]);
}

CINO_INLINE
void Hexmesh::set_vertex_u_text(const int vid, const float val)
{
    u_text.at(vid) = val;
}

CINO_INLINE
void Hexmesh::set_hex_label(const int tid, const int label)
{
    t_label.at(tid) = label;
}

CINO_INLINE
int Hexmesh::hex_label(const int tid) const
{
    return t_label.at(tid);
}

CINO_INLINE
float Hexmesh::vertex_u_text(const int vid) const
{
    return u_text.at(vid);
}

CINO_INLINE
float Hexmesh::min_u_text() const
{
    return *std::min_element(u_text.begin(), u_text.end());
}

CINO_INLINE
float Hexmesh::max_u_text() const
{
    return *std::max_element(u_text.begin(), u_text.end());
}

CINO_INLINE
int Hexmesh::max_t_label() const
{
    return *std::max_element(t_label.begin(), t_label.end());
}

CINO_INLINE
void Hexmesh::set_vertex(const int vid, const vec3d &pos)
{
    int vid_ptr = vid * 3;
    coords[vid_ptr + 0] = pos.x();
    coords[vid_ptr + 1] = pos.y();
    coords[vid_ptr + 2] = pos.z();
}

CINO_INLINE
bool Hexmesh::is_surface_vertex(const int vid) const
{
    return v_on_srf[vid];
}

CINO_INLINE
bool Hexmesh::is_surface_edge(const int eid) const
{
    return e_on_srf[eid];
}

CINO_INLINE
vec3d Hexmesh::quad_normal(const int qid) const
{
    int qid_ptr = qid * 3;
    return vec3d(q_norm[qid_ptr + 0], q_norm[qid_ptr + 1], q_norm[qid_ptr + 2]);
}

CINO_INLINE
vec3d Hexmesh::hex_centroid(const int qid) const
{
    return (hex_vertex(qid,0) +
            hex_vertex(qid,1) +
            hex_vertex(qid,2) +
            hex_vertex(qid,3) +
            hex_vertex(qid,4) +
            hex_vertex(qid,5) +
            hex_vertex(qid,6) +
            hex_vertex(qid,7)) / 8.0;
}

CINO_INLINE
int Hexmesh::hex_vertex_id(const int hid, const int offset) const
{
    int hid_ptr = hid * 8;
    return hexa[hid_ptr + offset];
}

CINO_INLINE
vec3d Hexmesh::hex_vertex(const int hid, const int offset) const
{
    int hid_ptr = hid * 8;
    return vertex(hexa[hid_ptr + offset]);
}

CINO_INLINE
int Hexmesh::quad_vertex_id(const int qid, const int offset) const
{
    int qid_ptr = qid * 4;
    return quads[qid_ptr + offset];
}

CINO_INLINE
vec3d Hexmesh::quad_vertex(const int qid, const int offset) const
{
    int qid_ptr = qid * 4;
    return vertex(quads[qid_ptr + offset]);
}

CINO_INLINE
bool Hexmesh::hex_contains_vertex(const int hid, const std::set<int> & vids) const
{
    for(int vid : vids)
    {
        if (hex_contains_vertex(hid, vid)) return true;
    }
    return false;
}

CINO_INLINE
bool Hexmesh::hex_contains_vertex(const int hid, const int vid) const
{
    if (hex_vertex_id(hid, 0) == vid) return true;
    if (hex_vertex_id(hid, 1) == vid) return true;
    if (hex_vertex_id(hid, 2) == vid) return true;
    if (hex_vertex_id(hid, 3) == vid) return true;
    if (hex_vertex_id(hid, 4) == vid) return true;
    if (hex_vertex_id(hid, 5) == vid) return true;
    if (hex_vertex_id(hid, 6) == vid) return true;
    if (hex_vertex_id(hid, 7) == vid) return true;
    return false;
}

CINO_INLINE
bool Hexmesh::hex_contains_edge(const int hid, const int eid) const
{
    for(int nbr : adj_hexa2edg(hid))
    {
        if (nbr == eid) return true;
    }
    return false;
}

CINO_INLINE
int Hexmesh::adjacent_hex_through_facet(const int hid, const int facet)
{
    std::vector<int> nbrs = adj_hexa2hexa(hid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        if (shared_facet(hid, nbrs[i]) == facet) return nbrs[i];
    }
    return -1;
}

CINO_INLINE
int Hexmesh::shared_facet(const int hid0, const int hid1) const
{
    for(int f=0; f<6; ++f)
    {
        if (hex_contains_vertex(hid1, hex_vertex_id(hid0, HEXA_FACES[f][0])) &&
            hex_contains_vertex(hid1, hex_vertex_id(hid0, HEXA_FACES[f][1])) &&
            hex_contains_vertex(hid1, hex_vertex_id(hid0, HEXA_FACES[f][2])) &&
            hex_contains_vertex(hid1, hex_vertex_id(hid0, HEXA_FACES[f][3])) )
        {
            return f;
        }
    }
    //assert("Tets do not share a facet!" && false);
    return -1;
}


CINO_INLINE
double Hexmesh::vertex_mass(const int vid) const
{
    std::vector<int> hexa = adj_vtx2hexa(vid);
    double mass = 0.0;
    for(int i=0; i<(int)hexa.size(); ++i)
    {
        int hid = hexa[i];
        mass += hex_volume(hid);
    }
    mass /= 8.0;
    return mass;
}

CINO_INLINE
double Hexmesh::hex_edge_length(const int hid, const int eid) const
{
    vec3d A = hex_vertex(hid, HEXA_EDGES[eid][0]);
    vec3d B = hex_vertex(hid, HEXA_EDGES[eid][1]);
    return (A-B).length();
}


CINO_INLINE
vec3d Hexmesh::hex_face_normal(const int tid, const int fid) const
{
    vec3d A = hex_vertex(tid, HEXA_FACES[fid][0]);
    vec3d B = hex_vertex(tid, HEXA_FACES[fid][1]);
    vec3d C = hex_vertex(tid, HEXA_FACES[fid][2]);
    vec3d n = (B-A).cross(C-A);
    n.normalize();
    return n;
}

CINO_INLINE
double Hexmesh::edge_length(const int eid) const
{
    return (edge_vertex(eid, 0) - edge_vertex(eid, 1)).length();
}

CINO_INLINE
vec3d Hexmesh::edge_vertex(const int eid, const int offset) const
{
    int eid_ptr = eid * 2;
    int vid     = edges[eid_ptr + offset];
    int vid_ptr = vid * 3;
    return vec3d(coords[vid_ptr + 0], coords[vid_ptr + 1], coords[vid_ptr + 2]);
}

CINO_INLINE
double Hexmesh::hex_quality(const int tid) const
{
    return 0.0;
//    return hex_scaled_jacobian(hex_vertex(tid,0),
//                               hex_vertex(tid,1),
//                               hex_vertex(tid,2),
//                               hex_vertex(tid,3));
}

CINO_INLINE
bool Hexmesh::hex_is_adjacent_to(const int hid, const int nbr) const
{
    for(int t : adj_hexa2hexa(hid))
    {
        if (t==nbr) return true;
    }
    return false;
}

CINO_INLINE
double Hexmesh::avg_edge_length() const
{
    double sum = 0.0;
    for(int eid=0; eid<(int)num_edges(); ++eid)
    {
        sum += edge_length(eid);
    }
    return sum/=double(num_edges());
}

CINO_INLINE
vec3d Hexmesh::element_barycenter(const int hid) const
{
    vec3d b(0,0,0);
    for(int i=0; i<8; ++i)
    {
        b += hex_vertex(hid,i);
    }
    b /= 8.0;
    return b;
}

CINO_INLINE
double Hexmesh::vertex_quality(const int hid) const
{
    double q = 1.0;
    std::vector<int> nbrs = adj_vtx2hexa(hid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        q = std::min(q, hex_quality(nbrs[i]));
    }
    return q;
}

CINO_INLINE
int Hexmesh::vertex_inverted_elements(const int vid) const
{
    int count = 0;
    std::vector<int> nbrs = adj_vtx2hexa(vid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        if (hex_quality(nbrs[i]) < 0) ++count;
    }
    return count;
}

CINO_INLINE
double Hexmesh::hex_volume(const int tid) const
{
    return 0.0;
//    return hex_unsigned_volume(hex_vertex(tid,0),
//                               hex_vertex(tid,1),
//                               hex_vertex(tid,2),
//                               hex_vertex(tid,3));
}

CINO_INLINE
int Hexmesh::edge_vertex_id(const int eid, const int offset) const
{
    int eid_ptr = eid * 2;
    return edges[eid_ptr + offset];
}


CINO_INLINE
void Hexmesh::operator+=(const Hexmesh & m)
{
    int nv = num_vertices();
    int nt = num_hexahedra();
    int ne = num_edges();
    int ns = num_srf_quads();

    for(int tid=0; tid<m.num_hexahedra(); ++tid)
    {
        hexa.push_back(nv + m.hex_vertex_id(tid,0));
        hexa.push_back(nv + m.hex_vertex_id(tid,1));
        hexa.push_back(nv + m.hex_vertex_id(tid,2));
        hexa.push_back(nv + m.hex_vertex_id(tid,3));

        t_label.push_back(m.hex_label(tid));

        std::vector<int> tmp;
        for(int eid : m.hexa2edg[tid]) tmp.push_back(ne + eid);
        hexa2edg.push_back(tmp);

        tmp.clear();
        for(int tid : m.hexa2hexa[tid]) tmp.push_back(nt + tid);
        hexa2hexa.push_back(tmp);

        tmp.clear();
        for(int tid : m.hexa2quad[tid]) tmp.push_back(ns + tid);
        hexa2quad.push_back(tmp);
    }
    for(int eid=0; eid<m.num_edges(); ++eid)
    {
        edges.push_back(nv + m.edge_vertex_id(eid,0));
        edges.push_back(nv + m.edge_vertex_id(eid,1));

        e_on_srf.push_back(m.e_on_srf[eid]);

        std::vector<int> tmp;
        for(int tid : m.edg2hexa[eid]) tmp.push_back(nt + tid);
        edg2hexa.push_back(tmp);

        tmp.clear();
        for(int tid : m.edg2quad[eid]) tmp.push_back(ns + tid);
        edg2quad.push_back(tmp);
    }
    for(int sid=0; sid<m.num_srf_quads(); ++sid)
    {
        quads.push_back(nv + m.quad_vertex_id(sid,0));
        quads.push_back(nv + m.quad_vertex_id(sid,1));
        quads.push_back(nv + m.quad_vertex_id(sid,2));

        vec3d n = m.quad_normal(sid);
        q_norm.push_back(n.x());
        q_norm.push_back(n.y());
        q_norm.push_back(n.z());

        quad2hexa.push_back(nt + m.quad2hexa[sid]);

        std::vector<int> tmp;
        for(int eid : m.quad2edg[sid]) tmp.push_back(ne + eid);
        quad2edg.push_back(tmp);

        tmp.clear();
        for(int nbr : m.quad2quad[sid]) tmp.push_back(ns + nbr);
        quad2quad.push_back(tmp);
    }
    for(int vid=0; vid<m.num_vertices(); ++vid)
    {
        vec3d pos = m.vertex(vid);
        coords.push_back(pos.x());
        coords.push_back(pos.y());
        coords.push_back(pos.z());

        v_on_srf.push_back(m.v_on_srf[vid]);
        u_text.push_back(m.vertex_u_text(vid));

        std::vector<int> tmp;
        for(int eid : m.vtx2edg[vid]) tmp.push_back(ne + eid);
        vtx2edg.push_back(tmp);

        tmp.clear();
        for(int tid : m.vtx2hexa[vid]) tmp.push_back(nt + tid);
        vtx2hexa.push_back(tmp);

        tmp.clear();
        for(int sid : m.vtx2quad[vid]) tmp.push_back(ns + sid);
        vtx2quad.push_back(tmp);

        tmp.clear();
        for(int nbr : m.vtx2vtx[vid]) tmp.push_back(nv + nbr);
        vtx2vtx.push_back(tmp);
    }

    update_bbox();
}

CINO_INLINE
void Hexmesh::normalize_volume()
{
    double vol = 0.0;
    for(int tid=0; tid<num_hexahedra(); ++tid)
    {
        vol += hex_volume(tid);
    }
    logger << "volume before: " << vol << endl;
    if (vol < 1e-4)
    {
        std::cerr << "\nWARNING!! Hexmesh Volume is close to zero: " << vol << endl << endl;
        vol = 1e-4;
    }
    double s = 1.0 / pow(vol, 1.0/3.0);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        set_vertex(vid, vertex(vid) * s);
    }
    {
        double vol = 0.0;
        for(int tid=0; tid<num_hexahedra(); ++tid)
        {
            vol += hex_volume(tid);
        }
        logger << "volume after: " << vol << endl;
    }
    update_bbox();
}

CINO_INLINE
bool Hexmesh::empty() const
{
    return coords.size() == 0;
}

CINO_INLINE
void Hexmesh::center_bbox()
{
    update_bbox();
    vec3d center = bb.center();
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        vec3d pos = vertex(vid) - center;
        set_vertex(vid, pos);
    }
    bb.min -= center;
    bb.max -= center;
}

}
