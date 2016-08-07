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
#include <cinolib/meshes/tetmesh/tetmesh.h>
#include <cinolib/timer.h>
#include <cinolib/io/read_write.h>

#include <float.h>
#include <map>
#include <set>


namespace cinolib
{


CINO_INLINE
Tetmesh::Tetmesh(const char * filename)
{
    timer_start("load tetmesh");

    load(filename);
    init();

    print_quality_statistics();

    timer_stop("load tetmesh");
}

CINO_INLINE
const Bbox & Tetmesh::bbox() const
{
    return bb;
}


CINO_INLINE
void Tetmesh::load(const char * filename)
{
    timer_start("Load Tetmesh");

    clear();

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
    {
        std::vector<uint> hexa; // not used here
        read_MESH(filename, coords, tets, hexa);
    }
    else if (filetype.compare(".tet") == 0 ||
             filetype.compare(".TET") == 0)
    {
        read_TET(filename, coords, tets);
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        std::vector<uint> hexa; // not used here
        read_VTU(filename, coords, tets, hexa);
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        std::vector<uint> hexa; // not used here
        read_VTK(filename, coords, tets, hexa);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    logger << tets.size()   / 4 << " tetrahedra read" << endl;
    logger << coords.size() / 3 << " vertices   read" << endl;

    this->filename = std::string(filename);

    timer_stop("Load Tetmesh");
}

CINO_INLINE
void Tetmesh::save(const char * filename) const
{
    timer_start("Save Tetmesh");

    std::string str(filename);
    std::string filetype = str.substr(str.size()-4,4);

    if (filetype.compare("mesh") == 0 ||
        filetype.compare("MESH") == 0)
    {
        std::vector<uint> hexa; // empty
        write_MESH(filename, coords, tets, hexa);
    }
    else if (filetype.compare(".tet") == 0 ||
             filetype.compare(".TET") == 0)
    {
        write_TET(filename, coords, tets);
    }
    else if (filetype.compare(".vtu") == 0 ||
             filetype.compare(".VTU") == 0)
    {
        std::vector<uint> hexa; // empty
        write_VTU(filename, coords, tets, hexa);
    }
    else if (filetype.compare(".vtk") == 0 ||
             filetype.compare(".VTK") == 0)
    {
        std::vector<uint> hexa; // empty
        write_VTK(filename, coords, tets, hexa);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    timer_stop("Save Tetmesh");
}

CINO_INLINE
Tetmesh::Tetmesh(const std::vector<double> & coords,
                 const std::vector<u_int>  & tets)
{
    clear();
    std::copy(coords.begin(), coords.end(), std::back_inserter(this->coords));
    std::copy(tets.begin(), tets.end(), std::back_inserter(this->tets));
    init();
}

CINO_INLINE
const std::vector<double> &Tetmesh::vector_coords() const
{
    return coords;
}

CINO_INLINE
const std::vector<uint> &Tetmesh::vector_tris() const
{
    return tris;
}

CINO_INLINE
const std::vector<float> &Tetmesh::vector_v_float_scalar() const
{
    return u_text;
}

CINO_INLINE
const std::vector<int> &Tetmesh::vector_t_int_scalar() const
{
    return t_label;
}

CINO_INLINE
std::string Tetmesh::loaded_file() const
{
    return filename;
}

CINO_INLINE
void Tetmesh::clear()
{
    bb.reset();
    coords.clear();
    tets.clear();
    edges.clear();
    tris.clear();
    v_on_srf.clear();
    e_on_srf.clear();
    t_norm.clear();
    u_text.clear();
    t_label.clear();
    vtx2vtx.clear();
    vtx2edg.clear();
    vtx2tet.clear();
    vtx2tri.clear();
    edg2tet.clear();
    tet2edg.clear();
    tet2tet.clear();
    tet2tri.clear();
    tri2tet.clear();
}

CINO_INLINE
void Tetmesh::init()
{
    u_text.resize(num_vertices());
    t_label.resize(num_tetrahedra());

    for(int vid=0; vid<num_vertices(); ++vid)
    {
        u_text[vid] = vertex(vid).y();
    }
    normalize_u_text_field();

    update_bbox();
    update_interior_adjacency();
    update_surface_adjacency();
    update_t_normals();

    logger << "BB min: " << bb.min << endl;
    logger << "BB max: " << bb.max << endl;
}

CINO_INLINE
void Tetmesh::update_bbox()
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
void Tetmesh::update_interior_adjacency()
{
    timer_start("Build adjacency");

    edges.clear();
    vtx2vtx.clear();
    vtx2edg.clear();
    vtx2tet.clear();
    edg2tet.clear();
    tet2tet.clear();
    tet2edg.clear();

    vtx2tet.resize(num_vertices());

    typedef std::map<ipair, std::vector<int> > mymap;
    mymap edge_tet_map;

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        int tid_ptr = tid * 4;

        int vid4 = tets[tid_ptr + 3];
        vtx2tet[vid4].push_back(tid);

        for(int i=0; i<3; ++i)
        {
            int  vid0 = tets[tid_ptr + i];
            int  vid1 = tets[tid_ptr + (i+1)%3];

            vtx2tet[vid0].push_back(tid);

            ipair e1 = unique_pair(vid0, vid1);
            edge_tet_map[e1].push_back(tid);

            ipair e2 = unique_pair(vid0, vid4);
            edge_tet_map[e2].push_back(tid);
        }
    }

    edg2tet.resize(edge_tet_map.size());
    tet2edg.resize(num_tetrahedra());
    tet2tet.resize(num_tetrahedra());
    vtx2vtx.resize(num_vertices());
    vtx2edg.resize(num_vertices());

    std::set<ipair> tet_pairs;

    int fresh_id = 0;
    for(mymap::iterator it=edge_tet_map.begin(); it!=edge_tet_map.end(); ++it)
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

        std::vector<int> tids = it->second;
        //assert(tids.size() <= 2);
        //sassert(tids.size() >= 1);
        for(int i=0; i<(int)tids.size(); ++i)
        {
            int tid = tids[i];

            tet2edg[tid].push_back(eid);
            edg2tet[eid].push_back(tid);

            for(int j=i+1; j<(int)tids.size(); ++j)
            {
                if (shared_facet(tids[i], tids[j]) != -1)
                {
                    ipair p = unique_pair(tids[j], tids[i]);
                    if (DOES_NOT_CONTAIN(tet_pairs, p))
                    {
                        tet_pairs.insert(p);
                        tet2tet[tids[i]].push_back(tids[j]);
                        tet2tet[tids[j]].push_back(tids[i]);
                        // sanity checks
                        assert(tet2tet[tids[j]].size() <= 4);
                        assert(tet2tet[tids[i]].size() <= 4);
                    }
                }
            }
        }

//        if (tids.size() == 2)
//        {
//            tet2tet[tids[0]].push_back(tids[1]);
//            tet2tet[tids[1]].push_back(tids[0]);
//        }
    }

    logger << num_vertices()   << "\tvertices"   << endl;
    logger << num_tetrahedra() << "\ttetrahedra" << endl;
    logger << num_edges()      << "\tedges"      << endl;

    timer_stop("Build adjacency");
}

CINO_INLINE
void Tetmesh::update_surface_adjacency()
{
    tris.clear();
    tri2tet.clear();
    v_on_srf.resize(num_vertices(), false);
    e_on_srf.resize(num_edges(), false);
    tet2tri.resize(num_tetrahedra());
    vtx2tri.resize(num_vertices());

    edg2tri.resize(num_edges());
    tri2edg.clear();
    tri2tri.clear();

    timer_start("Build Surface");

    typedef std::vector<int> face;
    std::map< face,std::pair<int,int> > tri2tet_map;

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        int tid_ptr = tid * 4;

        for(int fid=0; fid<4; ++fid)
        {
            face f;
            f.push_back(tets[tid_ptr + TET_FACES[fid][0]]);
            f.push_back(tets[tid_ptr + TET_FACES[fid][1]]);
            f.push_back(tets[tid_ptr + TET_FACES[fid][2]]);

            sort(f.begin(), f.end());

            if (CONTAINS(tri2tet_map,f)) tri2tet_map.erase(f);
            else                         tri2tet_map[f] = std::make_pair(tid,fid);
        }
    }

    for(auto it=tri2tet_map.begin(); it!=tri2tet_map.end(); ++it)
    {
        auto obj     = *it;
        int  tid     = obj.second.first;
        int  fid     = obj.second.second;
        int  tid_ptr = tid * 4;

        int vid0 = tets[tid_ptr + TET_FACES[fid][0]];
        int vid1 = tets[tid_ptr + TET_FACES[fid][1]];
        int vid2 = tets[tid_ptr + TET_FACES[fid][2]];

        tris.push_back(vid0);
        tris.push_back(vid1);
        tris.push_back(vid2);

        v_on_srf[vid0] = true;
        v_on_srf[vid1] = true;
        v_on_srf[vid2] = true;

        int fresh_id = tri2tet.size();

        vtx2tri[vid0].push_back(fresh_id);
        tet2tri[tid].push_back(fresh_id);
        tri2tet.push_back(tid);

        std::vector<int> edges = adj_tet2edg(tid);
        std::vector<int> tmp;
        tri2edg.push_back(tmp);
        assert(edges.size() == 6);
        for(size_t i=0; i<edges.size(); ++i)
        {
            int  eid   = edges[i];
            int  eid0  = edge_vertex_id(eid, 0);
            int  eid1  = edge_vertex_id(eid, 1);
            bool has_0 = (eid0 == vid0 || eid0 == vid1 || eid0 == vid2);
            bool has_1 = (eid1 == vid0 || eid1 == vid1 || eid1 == vid2);

            if (has_0 && has_1)
            {
                edg2tri[eid].push_back(fresh_id);
                tri2edg[fresh_id].push_back(eid);
            }
        }
    }

    tri2tri.resize(num_srf_triangles());

    for(int eid=0; eid<num_edges(); ++eid)
    {
        std::vector<int> tris = edg2tri[eid];
        if (!(tris.empty() || tris.size() == 2))
        {
            logger << "\tedge " << eid << " is non manifold! " << tris.size() << endl;
            logger << edge_vertex(eid,0) << "\t" << edge_vertex(eid,1) << endl;
            u_text[edge_vertex_id(eid,0)] = 10.0;
            u_text[edge_vertex_id(eid,1)] = 10.0;
        }
        //assert(tris.empty() || tris.size() == 2);
        if (!tris.empty())
        {
            int t0 = tris[0];
            int t1 = tris[1];
            tri2tri[t0].push_back(t1);
            tri2tri[t1].push_back(t0);
        }

        e_on_srf[eid] = !(edg2tri[eid].empty());
        //if (e_on_srf[eid]) assert(adj_edg2tri.size() == 2);
    }

    timer_stop("Build Surface");

    logger << tris.size() / 3 << " triangles were generated" << endl;
}

CINO_INLINE
void Tetmesh::update_t_normals()
{
    t_norm.clear();
    t_norm.resize(num_srf_triangles()*3);

    for(int tid=0; tid<num_srf_triangles(); ++tid)
    {
        int tid_ptr = tid * 3;

        vec3d v0 = vertex(tris[tid_ptr+0]);
        vec3d v1 = vertex(tris[tid_ptr+1]);
        vec3d v2 = vertex(tris[tid_ptr+2]);

        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        t_norm[tid_ptr + 0] = n.x();
        t_norm[tid_ptr + 1] = n.y();
        t_norm[tid_ptr + 2] = n.z();
    }
}

CINO_INLINE
int Tetmesh::num_vertices() const
{
    return coords.size()/3;
}

CINO_INLINE
int Tetmesh::num_tetrahedra() const
{
    return tets.size()/4;
}

CINO_INLINE
int Tetmesh::num_elements() const
{
    return tets.size()/4;
}

CINO_INLINE
int Tetmesh::num_edges() const
{
    return edges.size()/2;
}

CINO_INLINE
int Tetmesh::num_srf_triangles() const
{
    return tris.size()/3;
}

CINO_INLINE
const std::vector<int> & Tetmesh::adj_vtx2vtx(const int vid) const
{
    return vtx2vtx.at(vid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_vtx2edg(const int vid) const
{
    return vtx2edg.at(vid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_vtx2tri(const int vid) const
{
    return vtx2tri.at(vid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_vtx2tet(const int vid) const
{
    return vtx2tet.at(vid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_edg2tet(const int eid) const
{
    return edg2tet.at(eid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_edg2tri(const int eid) const
{
    return edg2tri.at(eid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_tet2edg(const int tid) const
{
    return tet2edg.at(tid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_tet2tet(const int tid) const
{
    return tet2tet.at(tid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_tet2tri(const int tid) const
{
    return tet2tri.at(tid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_tri2tri(const int tid) const
{
    return tri2tri.at(tid);
}

CINO_INLINE
const std::vector<int> &Tetmesh::adj_tri2edg(const int tid) const
{
    return tri2edg.at(tid);
}

CINO_INLINE
const int &Tetmesh::adj_tri2tet(const int tid) const
{
    return tri2tet.at(tid);
}

CINO_INLINE
vec3d Tetmesh::vertex(const int vid) const
{
    int vid_ptr = vid * 3;
    return vec3d(coords[vid_ptr+0], coords[vid_ptr+1], coords[vid_ptr+2]);
}

CINO_INLINE
void Tetmesh::set_vertex_u_text(const int vid, const float val)
{
    u_text.at(vid) = val;
}

CINO_INLINE
void Tetmesh::set_tet_label(const int tid, const int label)
{
    t_label.at(tid) = label;
}

CINO_INLINE
int Tetmesh::tet_label(const int tid) const
{
    return t_label.at(tid);
}

CINO_INLINE
int Tetmesh::elem_label(const int tid) const
{
    return tet_label(tid);
}

CINO_INLINE
float Tetmesh::vertex_u_text(const int vid) const
{
    return u_text.at(vid);
}

CINO_INLINE
float Tetmesh::min_u_text() const
{
    return *std::min_element(u_text.begin(), u_text.end());
}

CINO_INLINE
float Tetmesh::max_u_text() const
{
    return *std::max_element(u_text.begin(), u_text.end());
}

CINO_INLINE
int Tetmesh::max_t_label() const
{
    return *std::max_element(t_label.begin(), t_label.end());
}

CINO_INLINE
void Tetmesh::set_vertex(const int vid, const vec3d &pos)
{
    int vid_ptr = vid * 3;
    coords[vid_ptr + 0] = pos.x();
    coords[vid_ptr + 1] = pos.y();
    coords[vid_ptr + 2] = pos.z();
}

CINO_INLINE
bool Tetmesh::is_surface_vertex(const int vid) const
{
    return v_on_srf[vid];
}

CINO_INLINE
bool Tetmesh::is_surface_edge(const int eid) const
{
    return e_on_srf[eid];
}

CINO_INLINE
vec3d Tetmesh::triangle_normal(const int tid) const
{
    int tid_ptr = tid * 3;
    return vec3d(t_norm[tid_ptr + 0], t_norm[tid_ptr + 1], t_norm[tid_ptr + 2]);
}

CINO_INLINE
vec3d Tetmesh::tet_centroid(const int tid) const
{
    return (tet_vertex(tid,0) +
            tet_vertex(tid,1) +
            tet_vertex(tid,2) +
            tet_vertex(tid,3))* 0.25;
}

CINO_INLINE
int Tetmesh::tet_vertex_id(const int tid, const int offset) const
{
    int tid_ptr = tid * 4;
    return tets[tid_ptr + offset];
}

CINO_INLINE
vec3d Tetmesh::tet_vertex(const int tid, const int offset) const
{
    int tid_ptr = tid * 4;
    return vertex(tets[tid_ptr + offset]);
}

CINO_INLINE
vec3d Tetmesh::elem_vertex(const int eid, const int offset) const
{
    return tet_vertex(eid, offset);
}

CINO_INLINE
int Tetmesh::tri_vertex_id(const int tid, const int offset) const
{
    int tid_ptr = tid * 3;
    return tris[tid_ptr + offset];
}

CINO_INLINE
vec3d Tetmesh::tri_vertex(const int tid, const int offset) const
{
    int tid_ptr = tid * 3;
    return vertex(tris[tid_ptr + offset]);
}

CINO_INLINE
bool Tetmesh::tet_contains_vertex(const int tid, const std::set<int> &vids) const
{
    for(int vid : vids)
    {
        if (tet_contains_vertex(tid, vid)) return true;
    }
    return false;
}

CINO_INLINE
bool Tetmesh::tet_contains_vertex(const int tid, const int vid) const
{
    if (tet_vertex_id(tid, 0) == vid) return true;
    if (tet_vertex_id(tid, 1) == vid) return true;
    if (tet_vertex_id(tid, 2) == vid) return true;
    if (tet_vertex_id(tid, 3) == vid) return true;
    return false;
}

CINO_INLINE
bool Tetmesh::tet_contains_edge(const int tid, const int eid) const
{
    for(int nbr : adj_tet2edg(tid))
    {
        if (nbr == eid) return true;
    }
    return false;
}

CINO_INLINE
int Tetmesh::adjacent_tet_through_facet(const int tid, const int facet) const
{
    std::vector<int> nbrs = adj_tet2tet(tid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        if (shared_facet(tid, nbrs[i]) == facet) return nbrs[i];
    }
    return -1;
}

CINO_INLINE
int Tetmesh::shared_facet(const int tid0, const int tid1) const
{
    for(int f=0; f<4; ++f)
    {
        if (tet_contains_vertex(tid1, tet_vertex_id(tid0, TET_FACES[f][0])) &&
            tet_contains_vertex(tid1, tet_vertex_id(tid0, TET_FACES[f][1])) &&
            tet_contains_vertex(tid1, tet_vertex_id(tid0, TET_FACES[f][2])) )
        {
            return f;
        }
    }
    //assert("Tets do not share a facet!" && false);
    return -1;
}

CINO_INLINE
void Tetmesh::print_quality_statistics(bool list_folded_elements) const
{
    double asj = 0.0;
    double msj = FLT_MAX;
    int    inv = 0;

    if (list_folded_elements) logger << "Folded Tets: ";

    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        double q = tet_quality(tid);

        asj += q;
        msj = std::min(msj, q);

        if (q <= 0.0)
        {
            ++inv;

            if (list_folded_elements) logger << tid << " - ";
        }
    }
    asj /= double(num_tetrahedra());

    if (list_folded_elements) logger << endl << endl;

    logger << endl;
    logger << "MIN SJ : " << msj << endl;
    logger << "AVG SJ : " << asj << endl;
    logger << "INV EL : " << inv << " (out of " << num_tetrahedra() << ")" << endl;
    logger << endl;
}

CINO_INLINE
double Tetmesh::vertex_mass(const int vid) const
{
    std::vector<int> tets = adj_vtx2tri(vid);
    double mass = 0.0;
    for(int i=0; i<(int)tets.size(); ++i)
    {
        int tid = tets[i];
        mass += tet_volume(tid);
    }
    mass /= 4.0;
    return mass;
}

CINO_INLINE
int Tetmesh::vertex_valence(const int vid) const
{
    return adj_vtx2vtx(vid).size();
}

CINO_INLINE
int Tetmesh::tet_vertex_opposite_to(const int tid, const int facet) const
{
    for(int offset=0; offset<4; ++offset)
    {
        int vid = tet_vertex_id(tid, offset);

        if (tet_vertex_id(tid, TET_FACES[facet][0]) != vid &&
            tet_vertex_id(tid, TET_FACES[facet][1]) != vid &&
            tet_vertex_id(tid, TET_FACES[facet][2]) != vid)
        {
            return vid;
        }
    }
    assert(false);
}

CINO_INLINE
int Tetmesh::tet_face_opposite_to(const int tid, const int vid) const
{
    assert(tet_contains_vertex(tid, vid));
    for(int f=0; f<4; ++f)
    {
        if (tet_vertex_id(tid, TET_FACES[f][0]) != vid &&
            tet_vertex_id(tid, TET_FACES[f][1]) != vid &&
            tet_vertex_id(tid, TET_FACES[f][2]) != vid)
        {
            return f;
        }
    }
    assert(false);
}

CINO_INLINE
int Tetmesh::tet_edge_id(const int tid, const int vid0, const int vid1) const
{
    assert(tet_contains_vertex(tid, vid0));
    assert(tet_contains_vertex(tid, vid1));

    ipair ref = unique_pair(vid0, vid1);

    for(int eid : adj_tet2edg(tid))
    {
        if (unique_pair(edge_vertex_id(eid,0), edge_vertex_id(eid,1))==ref) return eid;
    }
    assert(false);
}


CINO_INLINE
int Tetmesh::tet_edge_opposite_to(const int tid, const int vid0, const int vid1) const
{
    for(int e=0; e<6; ++e)
    {
        if ((tet_vertex_id(tid, TET_EDGES[e][0]) == vid0 && tet_vertex_id(tid, TET_EDGES[e][1]) == vid1)) return e;
        if ((tet_vertex_id(tid, TET_EDGES[e][0]) == vid1 && tet_vertex_id(tid, TET_EDGES[e][1]) == vid0)) return e;
    }
    assert(false);
}

CINO_INLINE
double Tetmesh::tet_edge_length(const int tid, const int eid) const
{
    vec3d A = tet_vertex(tid, TET_EDGES[eid][0]);
    vec3d B = tet_vertex(tid, TET_EDGES[eid][1]);
    return (A-B).length();
}


CINO_INLINE
double Tetmesh::tet_face_area(const int tid, const int fid) const
{
    vec3d A = tet_vertex(tid, TET_FACES[fid][0]);
    vec3d B = tet_vertex(tid, TET_FACES[fid][1]);
    vec3d C = tet_vertex(tid, TET_FACES[fid][2]);
    return (0.5 * (B-A).cross(C-A).length());
}

CINO_INLINE
void Tetmesh::scale(const double x_scale, const double y_scale, const double z_scale)
{
    vec3d scale_fact(x_scale, y_scale, z_scale);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        set_vertex(vid, vertex(vid) * scale_fact);
    }
    update_bbox();
    update_t_normals();
}


CINO_INLINE
void Tetmesh::normalize_u_text_field()
{
    float min = *std::min_element(u_text.begin(), u_text.end());
    float max = *std::max_element(u_text.begin(), u_text.end());
    float den =  max-min;
    for(float & val : u_text)
    {
        val = (val - min)/den;
    }
}

CINO_INLINE
vec3d Tetmesh::tet_face_normal(const int tid, const int fid) const
{
    vec3d A = tet_vertex(tid, TET_FACES[fid][0]);
    vec3d B = tet_vertex(tid, TET_FACES[fid][1]);
    vec3d C = tet_vertex(tid, TET_FACES[fid][2]);
    vec3d n = (B-A).cross(C-A);
    n.normalize();
    return n;
}

CINO_INLINE
double Tetmesh::tet_dihedral_angle(const int tid, const int fid0, const int fid1) const
{
    vec3d   n0 = tet_face_normal(tid, fid0);
    vec3d   n1 = tet_face_normal(tid, fid1);
    double  alpha = acos(n0.dot(-n1));
    assert(!std::isnan(alpha));
    return alpha;
}

CINO_INLINE
double Tetmesh::edge_length(const int eid) const
{
    return (edge_vertex(eid, 0) - edge_vertex(eid, 1)).length();
}

CINO_INLINE
vec3d Tetmesh::edge_vertex(const int eid, const int offset) const
{
    int eid_ptr = eid * 2;
    int vid     = edges[eid_ptr + offset];
    int vid_ptr = vid * 3;
    return vec3d(coords[vid_ptr + 0], coords[vid_ptr + 1], coords[vid_ptr + 2]);
}

CINO_INLINE
double Tetmesh::tet_quality(const int tid) const
{
    return tet_scaled_jacobian(tet_vertex(tid,0),
                               tet_vertex(tid,1),
                               tet_vertex(tid,2),
                               tet_vertex(tid,3));
}

CINO_INLINE
double Tetmesh::elem_quality(const int eid) const
{
    return tet_quality(eid);
}

CINO_INLINE
bool Tetmesh::tet_is_adjacent_to(const int tid, const int nbr) const
{
    for(int t : adj_tet2tet(tid))
    {
        if (t==nbr) return true;
    }
    return false;
}

CINO_INLINE
double Tetmesh::avg_edge_length() const
{
    double sum = 0.0;
    for(int eid=0; eid<(int)num_edges(); ++eid)
    {
        sum += edge_length(eid);
    }
    return sum/=double(num_edges());
}

CINO_INLINE
vec3d Tetmesh::element_barycenter(const int tid) const
{
    vec3d b(0,0,0);
    for(int i=0; i<4; ++i)
    {
        b += tet_vertex(tid,i);
    }
    b /= 4.0;
    return b;
}

CINO_INLINE
double Tetmesh::vertex_quality(const int vid) const
{
    double q = 1.0;
    std::vector<int> nbrs = adj_vtx2tet(vid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        q = std::min(q, tet_quality(nbrs[i]));
    }
    return q;
}

CINO_INLINE
int Tetmesh::vertex_inverted_elements(const int vid) const
{
    int count = 0;
    std::vector<int> nbrs = adj_vtx2tet(vid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        if (tet_quality(nbrs[i]) < 0) ++count;
    }
    return count;
}

CINO_INLINE
double Tetmesh::tet_volume(const int tid) const
{
    return tet_unsigned_volume(tet_vertex(tid,0),
                               tet_vertex(tid,1),
                               tet_vertex(tid,2),
                               tet_vertex(tid,3));
}

CINO_INLINE
int Tetmesh::edge_vertex_id(const int eid, const int offset) const
{
    int eid_ptr = eid * 2;
    return edges[eid_ptr + offset];
}

CINO_INLINE
std::vector<int> Tetmesh::tet_one_ring(const int tid) const
{
    int vid_a = tet_vertex_id(tid, 0);
    int vid_b = tet_vertex_id(tid, 1);
    int vid_c = tet_vertex_id(tid, 2);
    int vid_d = tet_vertex_id(tid, 3);

    std::vector<int> vtx_adj[4] =
    {
        adj_vtx2tet(vid_a),
        adj_vtx2tet(vid_b),
        adj_vtx2tet(vid_c),
        adj_vtx2tet(vid_d)
    };

    std::set<int> unique_one_ring;
    for(int i=0; i<4; ++i)
    {
        for(size_t j=0; j<vtx_adj[i].size(); ++j)
        {
            if (vtx_adj[i][j] != tid) unique_one_ring.insert(vtx_adj[i][j]);
        }
    }

    std::vector<int> one_ring;
    std::copy(unique_one_ring.begin(), unique_one_ring.end(), std::back_inserter(one_ring));

    return one_ring;
}

CINO_INLINE
std::vector<int> Tetmesh::get_flipped_tets() const
{
    std::vector<int> list;
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        if (tet_quality(tid) < 0) list.push_back(tid);
    }
    return list;
}

CINO_INLINE
void Tetmesh::export_submesh_with_label(const int             label,
                                        std::vector<double> & sub_coords,
                                        std::vector<u_int>  & sub_tets,
                                        std::map<int, int>  & vid2sub_vid,
                                        std::map<int, int>  & sub_vid2vid) const
{
    assert(sub_coords.empty());
    assert(sub_tets.empty());
    assert(vid2sub_vid.empty());
    assert(sub_vid2vid.empty());

    std::vector<int> selected_tets;
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        if (tet_label(tid) == label) selected_tets.push_back(tid);
    }

    std::set<int> vids;
    for(int tid : selected_tets)
    {
        vids.insert(tet_vertex_id(tid,0));
        vids.insert(tet_vertex_id(tid,1));
        vids.insert(tet_vertex_id(tid,2));
        vids.insert(tet_vertex_id(tid,3));
    }

    int fresh_id = 0;
    for(int vid : vids)
    {
        sub_coords.push_back(vertex(vid).x());
        sub_coords.push_back(vertex(vid).y());
        sub_coords.push_back(vertex(vid).z());
        vid2sub_vid[vid] = fresh_id;
        sub_vid2vid[fresh_id] = vid;
        ++fresh_id;
    }

    for(int tid : selected_tets)
    {
        sub_tets.push_back( vid2sub_vid.at(tet_vertex_id(tid,0)) );
        sub_tets.push_back( vid2sub_vid.at(tet_vertex_id(tid,1)) );
        sub_tets.push_back( vid2sub_vid.at(tet_vertex_id(tid,2)) );
        sub_tets.push_back( vid2sub_vid.at(tet_vertex_id(tid,3)) );
    }
}

CINO_INLINE
Tetmesh Tetmesh::export_submesh_with_label(const int label) const
{
    std::vector<double> sub_coords;
    std::vector<u_int>  sub_tets;
    std::map<int, int>  vid2sub_vid;
    std::map<int, int>  sub_vid2vid;
    export_submesh_with_label(label, sub_coords, sub_tets, vid2sub_vid, sub_vid2vid);
    return Tetmesh(sub_coords, sub_tets);
}

CINO_INLINE
std::vector<int> Tetmesh::edge_ordered_tet_ring(const int eid) const
{
    std::vector<int> ring = adj_edg2tet(eid);

    assert(!ring.empty());
    assert((!is_surface_edge(eid)) || (is_surface_edge(eid) && !adj_edg2tri(eid).empty()));

    // if it's a surface edge start from a surface tet, if it's not you can start anywhere
    //
    int curr  = (is_surface_edge(eid)) ? adj_tri2tet(adj_edg2tri(eid)[0]) : ring[0];

    std::set<int> visited;
    std::vector<int> ordered_ring;
    do
    {
        ordered_ring.push_back(curr);
        visited.insert(curr);
        int next = -1;
        for(int tid : ring)
        {
            assert(tet_contains_edge(tid, eid));
            if (tid != curr && tet_is_adjacent_to(tid, curr) && DOES_NOT_CONTAIN(visited, tid))
            {
                next = tid;
            }
        }
        curr = next;
    } while (curr != -1);

    assert(ordered_ring.size() == adj_edg2tet(eid).size());
    return ordered_ring;
}

CINO_INLINE
bool Tetmesh::barycentric_coordinates(const int tid, const vec3d & P, std::vector<double> & wgts) const
{
    assert(tid>=0);
    assert(tid<num_tetrahedra());
    return tet_barycentric_coords(tet_vertex(tid,0),
                                  tet_vertex(tid,1),
                                  tet_vertex(tid,2),
                                  tet_vertex(tid,3),
                                  P, wgts);
}


CINO_INLINE
Trimesh Tetmesh::export_surface(std::map<int,int> & tet2tri_map,
                                std::map<int,int> & tri2tet_map) const
{
    assert(tet2tri_map.empty());
    assert(tri2tet_map.empty());

    std::vector<double> coords;
    std::vector<u_int>  srf;

    int fresh_id = 0;

    for(int vid=0; vid<num_vertices(); ++vid)
    {
        if (is_surface_vertex(vid))
        {
            vec3d pos = vertex(vid);
            coords.push_back(pos.x());
            coords.push_back(pos.y());
            coords.push_back(pos.z());
            tet2tri_map[vid] = fresh_id;
            tri2tet_map[fresh_id] = vid;
            ++fresh_id;
        }
    }
    for(size_t i=0; i<tris.size(); ++i)
    {
        srf.push_back(tet2tri_map[tris[i]]);
    }
    return Trimesh(coords, tris);
}

CINO_INLINE
Trimesh Tetmesh::export_surface() const
{
    std::map<int,int> tet2tri, tri2tet;
    return export_surface(tet2tri, tri2tet);
}


CINO_INLINE
void Tetmesh::operator+=(const Tetmesh & m)
{
    int nv = num_vertices();
    int nt = num_tetrahedra();
    int ne = num_edges();
    int ns = num_srf_triangles();

    for(int tid=0; tid<m.num_tetrahedra(); ++tid)
    {
        tets.push_back(nv + m.tet_vertex_id(tid,0));
        tets.push_back(nv + m.tet_vertex_id(tid,1));
        tets.push_back(nv + m.tet_vertex_id(tid,2));
        tets.push_back(nv + m.tet_vertex_id(tid,3));

        t_label.push_back(m.tet_label(tid));

        std::vector<int> tmp;
        for(int eid : m.tet2edg[tid]) tmp.push_back(ne + eid);
        tet2edg.push_back(tmp);

        tmp.clear();
        for(int tid : m.tet2tet[tid]) tmp.push_back(nt + tid);
        tet2tet.push_back(tmp);

        tmp.clear();
        for(int tid : m.tet2tri[tid]) tmp.push_back(ns + tid);
        tet2tri.push_back(tmp);
    }
    for(int eid=0; eid<m.num_edges(); ++eid)
    {
        edges.push_back(nv + m.edge_vertex_id(eid,0));
        edges.push_back(nv + m.edge_vertex_id(eid,1));

        e_on_srf.push_back(m.e_on_srf[eid]);

        std::vector<int> tmp;
        for(int tid : m.edg2tet[eid]) tmp.push_back(nt + tid);
        edg2tet.push_back(tmp);

        tmp.clear();
        for(int tid : m.edg2tri[eid]) tmp.push_back(ns + tid);
        edg2tri.push_back(tmp);
    }
    for(int sid=0; sid<m.num_srf_triangles(); ++sid)
    {
        tris.push_back(nv + m.tri_vertex_id(sid,0));
        tris.push_back(nv + m.tri_vertex_id(sid,1));
        tris.push_back(nv + m.tri_vertex_id(sid,2));

        vec3d n = m.triangle_normal(sid);
        t_norm.push_back(n.x());
        t_norm.push_back(n.y());
        t_norm.push_back(n.z());

        tri2tet.push_back(nt + m.tri2tet[sid]);

        std::vector<int> tmp;
        for(int eid : m.tri2edg[sid]) tmp.push_back(ne + eid);
        tri2edg.push_back(tmp);

        tmp.clear();
        for(int nbr : m.tri2tri[sid]) tmp.push_back(ns + nbr);
        tri2tri.push_back(tmp);
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
        for(int tid : m.vtx2tet[vid]) tmp.push_back(nt + tid);
        vtx2tet.push_back(tmp);

        tmp.clear();
        for(int sid : m.vtx2tri[vid]) tmp.push_back(ns + sid);
        vtx2tri.push_back(tmp);

        tmp.clear();
        for(int nbr : m.vtx2vtx[vid]) tmp.push_back(nv + nbr);
        vtx2vtx.push_back(tmp);
    }

    update_bbox();
}

CINO_INLINE
void Tetmesh::normalize_volume()
{
    double vol = 0.0;
    for(int tid=0; tid<num_tetrahedra(); ++tid)
    {
        vol += tet_volume(tid);
    }
    logger << "volume before: " << vol << endl;
    if (vol < 1e-4)
    {
        std::cerr << "\nWARNING!! Tetmesh Volume is close to zero: " << vol << endl << endl;
        vol = 1e-4;
    }
    double s = 1.0 / pow(vol, 1.0/3.0);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        set_vertex(vid, vertex(vid) * s);
    }
    {
        double vol = 0.0;
        for(int tid=0; tid<num_tetrahedra(); ++tid)
        {
            vol += tet_volume(tid);
        }
        logger << "volume after: " << vol << endl;
    }
    update_bbox();
}

CINO_INLINE
bool Tetmesh::empty() const
{
    return coords.size() == 0;
}

CINO_INLINE
void Tetmesh::center_bbox()
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

CINO_INLINE
void Tetmesh::translate(const vec3d & delta)
{
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        vec3d pos = vertex(vid);
        pos += delta;
        set_vertex(vid, pos);
    }
    update_bbox();
}


CINO_INLINE
bool Tetmesh::vertex_is_local_minima(const int vid) const
{
    float v = vertex_u_text(vid);
    for(int nbr : adj_vtx2vtx(vid))
    {
        if (vertex_u_text(nbr) <= v) return false;
    }
    return true;
}

CINO_INLINE
bool Tetmesh::vertex_is_local_maxima(const int vid) const
{
    float v = vertex_u_text(vid);
    for(int nbr : adj_vtx2vtx(vid))
    {
        if (vertex_u_text(nbr) >= v) return false;
    }
    return true;
}

CINO_INLINE
bool Tetmesh::vertex_is_critical_point(const int vid) const
{
    return (vertex_is_local_maxima(vid) || vertex_is_local_minima(vid));
}

CINO_INLINE
float Tetmesh::tet_min_u_text(const int tid) const
{
    float vals[4] =
    {
        vertex_u_text(tet_vertex_id(tid,0)),
        vertex_u_text(tet_vertex_id(tid,1)),
        vertex_u_text(tet_vertex_id(tid,2)),
        vertex_u_text(tet_vertex_id(tid,3))
    };
    return *std::min_element(vals, vals+4);
}


}
