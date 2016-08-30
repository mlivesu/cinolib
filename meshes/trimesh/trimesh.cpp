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
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/bfs.h>
#include <cinolib/timer.h>
#include <cinolib/io/read_write.h>

#include <queue>

namespace cinolib
{

CINO_INLINE
Trimesh::Trimesh(const char * filename)
{
    timer_start("load");

    load(filename);
    init();

    timer_stop("load");
}

CINO_INLINE
Trimesh::Trimesh(const std::vector<double> & coords,
                 const std::vector<u_int>  & tris)
{
    clear();
    this->coords = coords;
    this->tris   = tris;
    init();
}

CINO_INLINE
void Trimesh::clear()
{
    bb.reset();
    coords.clear();
    tris.clear();
    edges.clear();
    u_text.clear();
    t_label.clear();
    v_norm.clear();
    t_norm.clear();
    vtx2vtx.clear();
    vtx2tri.clear();
    vtx2edg.clear();
    tri2tri.clear();
    tri2edg.clear();
    edg2tri.clear();
}

CINO_INLINE
void Trimesh::init()
{
    update_adjacency();
    update_bbox();
    update_normals();

    u_text.resize(num_vertices(), 0.0);
    if (static_cast<int>(t_label.size()) != num_triangles()) t_label.resize(num_triangles(), 0);
}

CINO_INLINE
void Trimesh::update_adjacency()
{
    timer_start("Build adjacency");

    edges.clear();

    vtx2vtx.clear();
    vtx2edg.clear();
    edg2tri.clear();
    tri2tri.clear();
    tri2edg.clear();
    vtx2tri.clear();

    vtx2tri.resize(num_vertices());

    typedef std::map<ipair, std::vector<int> > mymap;
    mymap edge_tri_map;

    for(int tid=0; tid<num_triangles(); ++tid)
    {
        int tid_ptr = tid * 3;

        for(int i=0; i<3; ++i)
        {
            int  vid0 = tris[tid_ptr + i];
            int  vid1 = tris[tid_ptr + (i+1)%3];

            vtx2tri[vid0].push_back(tid);

            ipair e = unique_pair(vid0, vid1);
            edge_tri_map[e].push_back(tid);
        }
    }

    edg2tri.resize(edge_tri_map.size());
    tri2edg.resize(num_triangles());
    tri2tri.resize(num_triangles());
    vtx2vtx.resize(num_vertices());
    vtx2edg.resize(num_vertices());

    int fresh_id = 0;
    for(mymap::iterator it=edge_tri_map.begin(); it!=edge_tri_map.end(); ++it)
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
        //assert(tids.size() <= 2 && "Non manifold edge!");
        //assert(tids.size() >= 1 && "Non manifold edge!");
        //if (tids.size() > 2 || tids.size() < 1) cerr << "Non manifold edge! " << edge_vertex(eid, 0) << "\t" << edge_vertex(eid, 1) << endl;

        for(int tid : tids)
        {
            tri2edg[tid].push_back(eid);
            edg2tri[eid].push_back(tid);
        }

        if (tids.size() >= 2)
        {
            for (size_t i=0; i<tids.size()-1; i++)
            {
                tri2tri[tids[i]].push_back(tids[i+1]);
                tri2tri[tids[i+1]].push_back(tids[i]);
            }

            if (tids.size() > 2)
            {
                tri2tri[tids[0]].push_back(tids[tids.size()-1]);
                tri2tri[tids[tids.size()-1]].push_back(tids[0]);
            }
        }
    }

    logger << num_vertices()  << "\tvertices"  << endl;
    logger << num_triangles() << "\ttriangles" << endl;
    logger << num_edges()     << "\tedges"     << endl;

    timer_stop("Build adjacency");
}

CINO_INLINE
void Trimesh::update_t_normal(const int tid)
{
    int tid_ptr = tid * 3;

    vec3d v0 = vertex(tris.at(tid_ptr+0));
    vec3d v1 = vertex(tris.at(tid_ptr+1));
    vec3d v2 = vertex(tris.at(tid_ptr+2));

    vec3d u = v1 - v0;    u.normalize();
    vec3d v = v2 - v0;    v.normalize();
    vec3d n = u.cross(v); n.normalize();

    t_norm.at(tid_ptr + 0) = n.x();
    t_norm.at(tid_ptr + 1) = n.y();
    t_norm.at(tid_ptr + 2) = n.z();
}

CINO_INLINE
void Trimesh::update_t_normals()
{
    t_norm.resize(num_triangles()*3);
    for(int tid=0; tid<num_triangles(); ++tid)
    {
        update_t_normal(tid);
    }
}

CINO_INLINE
void Trimesh::update_v_normals()
{
    v_norm.resize(num_vertices()*3);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        update_v_normal(vid);
    }
}

CINO_INLINE
void Trimesh::update_v_normal(const int vid)
{
    if (adj_vtx2tri(vid).empty()) return;

    vec3d sum(0,0,0);
    for(int nbr : adj_vtx2tri(vid))
    {
        sum += triangle_normal(nbr);
    }

    sum /= double(adj_vtx2tri(vid).size());
    sum.normalize();

    int vid_ptr = vid * 3;
    v_norm.at(vid_ptr + 0) = sum.x();
    v_norm.at(vid_ptr + 1) = sum.y();
    v_norm.at(vid_ptr + 2) = sum.z();
}

CINO_INLINE
std::string Trimesh::loaded_file() const
{
    return filename;
}

CINO_INLINE
int Trimesh::num_vertices() const
{
    return coords.size()/3;
}

CINO_INLINE
int Trimesh::num_triangles() const
{
    return tris.size()  /3;
}

CINO_INLINE
int Trimesh::num_elements() const
{
    return tris.size()  /3;
}

CINO_INLINE
int Trimesh::num_edges() const
{
    return edges.size() /2;
}

CINO_INLINE
const std::vector<int> & Trimesh::adj_vtx2vtx(const int vid) const
{
    return vtx2vtx.at(vid);
}

CINO_INLINE
const std::vector<int> &Trimesh::adj_vtx2edg(const int vid) const
{
    return vtx2edg.at(vid);
}

CINO_INLINE
const std::vector<int> &Trimesh::adj_vtx2tri(const int vid) const
{
    return vtx2tri.at(vid);
}

CINO_INLINE
const std::vector<int> &Trimesh::adj_vtx2ele(const int vid) const
{
    return adj_vtx2tri(vid);
}

CINO_INLINE
const std::vector<int> &Trimesh::adj_edg2tri(const int eid) const
{
    return edg2tri.at(eid);
}

CINO_INLINE
const std::vector<int> &Trimesh::adj_tri2edg(const int tid) const
{
    return tri2edg.at(tid);
}

CINO_INLINE
const std::vector<int> &Trimesh::adj_ele2edg(const int tid) const
{
    return adj_tri2edg(tid);
}

CINO_INLINE
const std::vector<int> &Trimesh::adj_tri2tri(const int tid) const
{
    return tri2tri.at(tid);
}

CINO_INLINE
void Trimesh::load(const char * filename)
{
    timer_start("Load Trimesh");

    clear();

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        std::vector<uint> quads; // ignored here
        read_OFF(filename, coords, tris, quads);
    }
    else
    if (filetype.compare("obj") == 0 ||
        filetype.compare("OBJ") == 0)
    {
        std::vector<uint> quads; // ignored here
        read_OBJ(filename, coords, tris, quads);
    }
    else
    if (filetype.compare(".iv") == 0 ||
        filetype.compare(".IV") == 0)
    {
        read_IV(filename, coords, tris, t_label);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    logger << tris.size() / 3   << " triangles read" << endl;
    logger << coords.size() / 3 << " vertices  read" << endl;

    this->filename = std::string(filename);

    timer_stop("Load Trimesh");
}

CINO_INLINE
void Trimesh::save(const char * filename) const
{
    timer_start("Save Trimesh");

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        std::vector<uint> quads; // empty
        write_OFF(filename, coords, tris, quads);
    }
    else
    if (filetype.compare("obj") == 0 ||
        filetype.compare("OBJ") == 0)
    {
        std::vector<uint> quads; // empty
        write_OBJ(filename, coords, tris, quads);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save() : file format not supported yet " << endl;
        exit(-1);
    }

    logger << tris.size() / 3   << " triangles written" << endl;
    logger << coords.size() / 3 << " vertices  written" << endl;

    timer_stop("Save Trimesh");
}

CINO_INLINE
int Trimesh::vertex_opposite_to(const int tid, const int vid0, const int vid1) const
{
    for(int i=0; i<3; ++i)
    {
        int vid = triangle_vertex_id(tid, i);
        if (vid != vid0 && vid != vid1) return vid;
    }
    assert(false);
}

CINO_INLINE
int Trimesh::vertex_opposite_to(const int eid, const int vid) const
{
    int v = edge_vertex_id(eid, 0);
    if (v != vid) return v;
    return edge_vertex_id(eid, 1);
    assert(false);
}

CINO_INLINE
int Trimesh::edge_opposite_to(const int tid, const int vid) const
{
    assert(triangle_contains_vertex(tid, vid));
    std::vector<int> edges = adj_tri2edg(tid);
    for(int i=0; i<(int)edges.size(); ++i)
    {
        int eid = edges[i];
        if (edge_vertex_id(eid, 0) != vid && edge_vertex_id(eid, 1) != vid) return eid;
    }

    logger << " Triangle: " << tid << " [ " << tris.at(tid*3+0) << " " << tris.at(tid*3+1) << " " << tris.at(tid*3+2) << " ] " << endl;
    assert(false);
}

CINO_INLINE
double Trimesh::triangle_angle_at_vertex(const int tid, const int vid, const bool rad) const
{
    int i;

         if (triangle_vertex_id(tid,0) == vid) i = 0;
    else if (triangle_vertex_id(tid,1) == vid) i = 1;
    else if (triangle_vertex_id(tid,2) == vid) i = 2;
    else assert(false);

    vec3d P = vertex(vid);
    vec3d u = triangle_vertex(tid, (i+1)%3) - P;
    vec3d v = triangle_vertex(tid, (i+2)%3) - P;

    if (rad) return u.angle_rad(v);
    return u.angle_deg(v);
}

/* For a definition of caps and needles please refer to:
 * A Robust Procedure to Eliminate Degenerate Faces from Triangle Meshes
 * Mario Botsch and Leif P. Kobbelt
 * VMW 2001
*/
CINO_INLINE
bool Trimesh::triangle_is_cap(const int tid, const double angle_thresh_deg) const
{
    vec3d vtx[3] =
    {
        triangle_vertex(tid,0),
        triangle_vertex(tid,1),
        triangle_vertex(tid,2)
    };
    for(int i=0; i<3; ++i)
    {
        vec3d  u = vtx[(i+1)%3] - vtx[i];
        vec3d  v = vtx[(i+2)%3] - vtx[i];
        if (u.angle_deg(v) > angle_thresh_deg) return true;
    }
    return false;
}

/* For a definition of caps and needles please refer to:
 * A Robust Procedure to Eliminate Degenerate Faces from Triangle Meshes
 * Mario Botsch and Leif P. Kobbelt
 * VMW 2001
*/
CINO_INLINE
bool Trimesh::triangle_is_needle(const int tid, const double angle_thresh_deg) const
{
    vec3d vtx[3] =
    {
        triangle_vertex(tid,0),
        triangle_vertex(tid,1),
        triangle_vertex(tid,2)
    };
    for(int i=0; i<3; ++i)
    {
        vec3d  u = vtx[(i+1)%3] - vtx[i];
        vec3d  v = vtx[(i+2)%3] - vtx[i];
        if (u.angle_deg(v) < angle_thresh_deg) return true;
    }
    return false;
}

CINO_INLINE
double Trimesh::element_mass(const int tid) const
{
    vec3d P = triangle_vertex(tid, 0);
    vec3d u = triangle_vertex(tid, 1) - P;
    vec3d v = triangle_vertex(tid, 2) - P;
    double area = 0.5 * u.cross(v).length();
    return area;
}

CINO_INLINE
double Trimesh::vertex_mass(const int vid) const
{
    std::vector<int> tris = adj_vtx2tri(vid);
    double mass = 0.0;
    for(int i=0; i<(int)tris.size(); ++i)
    {
        int tid = tris[i];
        mass += element_mass(tid);
    }
    mass /= 3.0;
    return mass;
}

// has adjacent triangles with different per triangle iscalars
CINO_INLINE
bool Trimesh::vertex_is_border(const int vid) const
{
    std::vector<int> tris = adj_vtx2tri(vid);
    std::set<int> tri_scalars;
    for(int i=0; i<(int)tris.size(); ++i)
    {
        tri_scalars.insert( triangle_label(tris[i]) );
    }
    return (tri_scalars.size() > 1);
}

// is along an open boundary
CINO_INLINE
bool Trimesh::vertex_is_boundary(const int vid) const
{
    std::vector<int> edges = adj_vtx2edg(vid);
    for(int i=0; i<(int)edges.size(); ++i)
    {
        if (edge_is_boundary(edges[i])) return true;
    }
    return false;
}

CINO_INLINE
bool Trimesh::vertices_are_adjacent(const int vid0, const int vid1) const
{
    for(int nbr : adj_vtx2vtx(vid0))
    {
        if (nbr == vid1) return true;
    }
    return false;
}

CINO_INLINE
bool Trimesh::edge_is_manifold(const int eid) const
{
    return (adj_edg2tri(eid).size() == 2);
}

CINO_INLINE
bool Trimesh::edge_is_boundary(const int eid) const
{
    return (adj_edg2tri(eid).size() < 2);
}

CINO_INLINE
bool Trimesh::edge_is_border(const int eid) const
{
    assert(edge_is_manifold(eid));

    int tid_0 = adj_edg2tri(eid).at(0);
    int tid_1 = adj_edg2tri(eid).at(1);

    return (triangle_label(tid_0) != triangle_label(tid_1));
}

CINO_INLINE
bool Trimesh::triangle_is_boundary(const int tid) const
{
    return (adj_tri2tri(tid).size() < 3);
}

CINO_INLINE
int Trimesh::edge_vertex_id(const int eid, const int offset) const
{
    int eid_ptr = eid * 2;
    return edges.at(eid_ptr + offset);
}

CINO_INLINE
int Trimesh::triangle_vertex_id(const int tid, const int offset) const
{
    int tid_ptr = tid * 3;
    return tris.at(tid_ptr + offset);
}

CINO_INLINE
int Trimesh::elem_vertex_id(const int tid, const int offset) const
{
    return triangle_vertex_id(tid, offset);
}


CINO_INLINE
int Trimesh::triangle_vertex_offset(const int tid, const int vid) const
{
    assert(triangle_contains_vertex(tid, vid));
    for(int off=0; off<verts_per_element; ++off)
    {
        if (triangle_vertex_id(tid,off) == vid) return off;
    }
    assert(false);
}

CINO_INLINE
int Trimesh::elem_vertex_offset(const int eid, const int vid) const
{
    return triangle_vertex_offset(eid, vid);
}

CINO_INLINE
vec3d Trimesh::triangle_normal(const int tid) const
{
    int tid_ptr = tid * 3;
    return vec3d(t_norm.at(tid_ptr + 0), t_norm.at(tid_ptr + 1), t_norm.at(tid_ptr + 2));
}

CINO_INLINE
vec3d Trimesh::vertex_normal(const int vid) const
{
    int vid_ptr = vid * 3;
    return vec3d(v_norm.at(vid_ptr+0), v_norm.at(vid_ptr+1), v_norm.at(vid_ptr+2));
}

CINO_INLINE
vec3d Trimesh::vertex(const int vid) const
{
    int vid_ptr = vid * 3;
    return vec3d(coords.at(vid_ptr+0), coords.at(vid_ptr+1), coords.at(vid_ptr+2));
}

CINO_INLINE
void Trimesh::set_vertex(const int vid, const vec3d &pos)
{
    int vid_ptr = vid * 3;
    coords.at(vid_ptr + 0) = pos.x();
    coords.at(vid_ptr + 1) = pos.y();
    coords.at(vid_ptr + 2) = pos.z();
}

CINO_INLINE
int Trimesh::vertex_valence(const int vid) const
{
    return adj_vtx2vtx(vid).size();
}

CINO_INLINE
float Trimesh::triangle_min_u_text(const int tid) const
{
    float vals[3] =
    {
        vertex_u_text(triangle_vertex_id(tid,0)),
        vertex_u_text(triangle_vertex_id(tid,1)),
        vertex_u_text(triangle_vertex_id(tid,2)),
    };
    return *std::min_element(vals, vals+3);
}

CINO_INLINE
int Trimesh::triangle_edge_local_to_global(const int tid, const int off) const
{
    assert(off>=0 && off <=2);

    int vid0 = triangle_vertex_id(tid, TRI_EDGES[off][0]);
    int vid1 = triangle_vertex_id(tid, TRI_EDGES[off][1]);

    for(int eid : adj_tri2edg(tid))
    {
        if (edge_contains_vertex(eid, vid0) && edge_contains_vertex(eid, vid1))
        {
            return eid;
        }
    }
    assert(false && "Something is off here...");
}

CINO_INLINE
int Trimesh::triangle_vertex_global_to_local(const int tid, const int vid) const
{
    assert(vid>=0 && vid<=num_vertices());
    assert(triangle_contains_vertex(tid, vid));
    if (triangle_vertex_id(tid,0) == vid) return 0;
    if (triangle_vertex_id(tid,1) == vid) return 1;
    if (triangle_vertex_id(tid,2) == vid) return 2;
    assert(false && "Something is off here...");
    return -1;
}

CINO_INLINE
float Trimesh::vertex_u_text(const int vid) const
{
    return u_text.at(vid);
}

CINO_INLINE
void Trimesh::set_vertex_u_text(const int vid, const float s)
{
    u_text.at(vid) = s;
}

CINO_INLINE
int Trimesh::vertex_with_min_u_text() const
{
    return (std::min_element(u_text.begin(), u_text.end()) - u_text.begin());
}

CINO_INLINE
int Trimesh::triangle_label(const int tid) const
{
    return t_label.at(tid);
}

CINO_INLINE
int Trimesh::elem_label(const int tid) const
{
    return triangle_label(tid);
}

CINO_INLINE
void Trimesh::triangle_set_label(const int tid, const int i)
{
    t_label.at(tid) = i;
}

CINO_INLINE
void Trimesh::update_normals()
{
    update_t_normals();
    update_v_normals();
}

CINO_INLINE
bool Trimesh::triangle_contains_vertex(const int tid, const int vid) const
{
    if (triangle_vertex_id(tid, 0) == vid) return true;
    if (triangle_vertex_id(tid, 1) == vid) return true;
    if (triangle_vertex_id(tid, 2) == vid) return true;
    return false;
}

CINO_INLINE
std::vector<int> Trimesh::get_boundary_vertices() const
{
    std::set<int> unique_border;
    for(int eid=0; eid<num_edges(); ++eid)
    {
        if (edge_is_boundary(eid))
        {
            unique_border.insert(edge_vertex_id(eid, 0));
            unique_border.insert(edge_vertex_id(eid, 1));
        }
    }

    std::vector<int> border;
    std::copy(unique_border.begin(), unique_border.end(), std::back_inserter(border));

    return border;
}

CINO_INLINE
std::vector<std::pair<int,int> > Trimesh::get_boundary_edges() const
{    
    std::vector<ipair> border;
    for(int eid=0; eid<num_edges(); ++eid)
    {
        if (edge_is_boundary(eid))
        {
            ipair e;
            e.first  = edge_vertex_id(eid, 0);
            e.second = edge_vertex_id(eid, 1);
            border.push_back(e);
        }
    }
    return border;
}

CINO_INLINE
void Trimesh::update_bbox()
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
void Trimesh::translate(const vec3d & delta)
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
void Trimesh::rotate(const vec3d & axis, const double angle)
{
    double R[3][3];
    bake_rotation_matrix(axis, angle, R);
    vec3d c = bb.center();

    for(int vid=0; vid<num_vertices(); ++vid)
    {
        vec3d pos = vertex(vid) - c;
        transform(pos, R);
        pos += c;
        set_vertex(vid, pos);
    }
    update_bbox();
    update_normals();
}

CINO_INLINE
void Trimesh::operator+=(const Trimesh & m)
{
    int nv = num_vertices();
    int nt = num_triangles();
    int ne = num_edges();

    for(int tid=0; tid<m.num_triangles(); ++tid)
    {
        tris.push_back(nv + m.triangle_vertex_id(tid,0));
        tris.push_back(nv + m.triangle_vertex_id(tid,1));
        tris.push_back(nv + m.triangle_vertex_id(tid,2));

        t_label.push_back(m.triangle_label(tid));

        vec3d n = m.triangle_normal(tid);
        t_norm.push_back(n.x());
        t_norm.push_back(n.y());
        t_norm.push_back(n.z());

        std::vector<int> tmp;
        for(int eid : m.tri2edg[tid]) tmp.push_back(ne + eid);
        tri2edg.push_back(tmp);

        tmp.clear();
        for(int tid : m.tri2tri[tid]) tmp.push_back(nt + tid);
        tri2tri.push_back(tmp);
    }
    for(int eid=0; eid<m.num_edges(); ++eid)
    {
        edges.push_back(nv + m.edge_vertex_id(eid,0));
        edges.push_back(nv + m.edge_vertex_id(eid,1));

        std::vector<int> tmp;
        for(int tid : m.edg2tri[eid]) tmp.push_back(nt + tid);
        edg2tri.push_back(tmp);
    }
    for(int vid=0; vid<m.num_vertices(); ++vid)
    {
        vec3d pos = m.vertex(vid);
        coords.push_back(pos.x());
        coords.push_back(pos.y());
        coords.push_back(pos.z());

        u_text.push_back(m.vertex_u_text(vid));

        vec3d n = m.vertex_normal(vid);
        v_norm.push_back(n.x());
        v_norm.push_back(n.y());
        v_norm.push_back(n.z());

        std::vector<int> tmp;
        for(int eid : m.vtx2edg[vid]) tmp.push_back(ne + eid);
        vtx2edg.push_back(tmp);

        tmp.clear();
        for(int tid : m.vtx2tri[vid]) tmp.push_back(nt + tid);
        vtx2tri.push_back(tmp);

        tmp.clear();
        for(int nbr : m.vtx2vtx[vid]) tmp.push_back(nv + nbr);
        vtx2vtx.push_back(tmp);
    }

    update_bbox();
}

CINO_INLINE
void Trimesh::center_bbox()
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
void Trimesh::check_topology() const
{
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        for(int nbr : adj_vtx2vtx(vid)) assert(nbr >= 0 && nbr < num_vertices());
        for(int nbr : adj_vtx2edg(vid)) assert(nbr >= 0 && nbr < num_edges());
        for(int nbr : adj_vtx2tri(vid)) assert(nbr >= 0 && nbr < num_triangles());
    }
    for(int eid=0; eid<num_edges(); ++eid)
    {
        assert(edge_vertex_id(eid,0) >= 0 && edge_vertex_id(eid,0) < num_vertices());
        assert(edge_vertex_id(eid,1) >= 0 && edge_vertex_id(eid,1) < num_vertices());
        for(int nbr : adj_edg2tri(eid)) assert(nbr >= 0 && nbr < num_triangles());
    }
    for(int tid=0; tid<num_triangles(); ++tid)
    {
        assert(triangle_vertex_id(tid,0) >= 0 && triangle_vertex_id(tid,0) < num_vertices());
        assert(triangle_vertex_id(tid,1) >= 0 && triangle_vertex_id(tid,1) < num_vertices());
        assert(triangle_vertex_id(tid,2) >= 0 && triangle_vertex_id(tid,2) < num_vertices());
        for(int nbr : adj_tri2tri(tid)) assert(nbr >= 0 && nbr < num_triangles());
        for(int nbr : adj_tri2edg(tid)) assert(nbr >= 0 && nbr < num_edges());
    }
}

CINO_INLINE
void Trimesh::normalize_area()
{
    double area = 0.0;
    for(int tid=0; tid<num_triangles(); ++tid)
    {
        area += element_mass(tid);
    }
    if (area < 1e-4)
    {
        std::cerr << "\nWARNING!! Trimesh Area is close to zero: " << area << endl << endl;
        area = 1e-4;
    }
    double s = 1.0 / sqrt(area);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        set_vertex(vid, vertex(vid) * s);
    }
}

CINO_INLINE
vec3d Trimesh::element_barycenter(const int eid) const
{
    assert(eid >= 0);
    assert(eid < num_triangles());
    vec3d b(0,0,0);
    for(int i=0; i<3; ++i)
    {
        b += triangle_vertex(eid,i);
    }
    b /= 3.0;
    return b;
}

CINO_INLINE
ipair Trimesh::shared_edge(const int tid0, const int tid1) const
{
    std::vector<int> shared_vertices;

    int v0 = triangle_vertex_id(tid0, 0);
    int v1 = triangle_vertex_id(tid0, 1);
    int v2 = triangle_vertex_id(tid0, 2);

    if (triangle_contains_vertex(tid1,v0)) shared_vertices.push_back(v0);
    if (triangle_contains_vertex(tid1,v1)) shared_vertices.push_back(v1);
    if (triangle_contains_vertex(tid1,v2)) shared_vertices.push_back(v2);

    assert(shared_vertices.size() == 2);

    ipair e;
    e.first  = shared_vertices[0];
    e.second = shared_vertices[1];

    return e;
}

CINO_INLINE
bool Trimesh::edge_collapse(const int eid)
{

    // define what to keep and what to remove
    //
    uint vid_keep   = edge_vertex_id(eid, 0);
    uint vid_remove = edge_vertex_id(eid, 1);

    std::set<int> tid_remove(adj_edg2tri(eid).begin(), adj_edg2tri(eid).end());

    std::set<int> edg_keep, edg_remove;

    for(int tid: tid_remove)
    {
        int keep = edge_opposite_to(tid, vid_remove);
        int rem = edge_opposite_to(tid, vid_keep);

        edg_keep.insert(keep);
        edg_remove.insert(rem);
    }
    edg_remove.insert(eid);

    // Pre-processing : check edges
    //
    for(int eid : adj_vtx2edg(vid_remove))
    {
        if (CONTAINS(edg_remove, eid)) continue;

        uint vid0 = edges.at(eid*2+0);
        uint vid1 = edges.at(eid*2+1);

        uint vid0_mod = vid0;
        uint vid1_mod = vid1;

        if (vid0 == vid_remove) vid0_mod = vid_keep; else
        if (vid1 == vid_remove) vid1_mod = vid_keep; else
        assert("Something is off here" && false);

        // check edge

        for (int i=0; i < num_edges(); i++)
        {
            if (i == eid) continue;

            if ((vid0_mod == vid0 && vid1_mod  == vid1) || (vid0_mod == vid1 && vid1_mod == vid0))
            {
                logger << "WARNING : duplicared edge " << eid << " : Impossible to perform edge collapse. " << std::endl;

                //logger << " NEW EDGE: " << eid << " -- " << copy_edges.at(eid*2+0) << " " << copy_edges.at(eid*2+1) << std::endl;
                //logger << " ORIGINAL EDGE: " << i << " -- " << copy_edges.at(i*2+0) << " " << copy_edges.at(i*2+1) << std::endl;

                return false;
            }
        }
    }

    // Pre-processing : check triangles
    //
    for(int tid : adj_vtx2tri(vid_remove))
    {
        if (CONTAINS(tid_remove, tid)) continue;

        //copy_vtx2tri.at(vid_keep).push_back(tid);
        vec3d n_old = triangle_normal(tid);

        uint vid0 = triangle_vertex_id(tid, 0);
        uint vid1 = triangle_vertex_id(tid, 1);
        uint vid2 = triangle_vertex_id(tid, 2);

        if (vid0 == vid_remove) vid0 = vid_keep; else
        if (vid1 == vid_remove) vid1 = vid_keep; else
        if (vid2 == vid_remove) vid2 = vid_keep; else
        assert("Something is off here" && false);

        // check triangle flip
        vec3d v0 = vertex(vid0);
        vec3d v1 = vertex(vid1);
        vec3d v2 = vertex(vid2);

        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        if (n.x() == 0 && n.y() == 0 && n.z() == 0)     // triangle area == 0
        {
            logger << "WARNING : triangle on collinear points " << tid << " : Impossible to perform edge collapse. " << std::endl;
            return false;
        }

        if (n.dot(n_old) < 0 )       // triangle inversion
        {
            logger << "WARNING : triangle inversion " << tid << " : Impossible to perform edge collapse. " << std::endl;
            return false;
        }
    }

    // Everything is ok
    // The edge can be collapsed

    for(int eid : adj_vtx2edg(vid_remove))
    {
        if (CONTAINS(edg_remove, eid)) continue;
        vtx2edg.at(vid_keep).push_back(eid);
        if (edges.at(eid*2+0) == vid_remove) edges.at(eid*2+0) = vid_keep; else
        if (edges.at(eid*2+1) == vid_remove) edges.at(eid*2+1) = vid_keep; else
        assert("Something is off here" && false);
    }
    //
    for(int tid : adj_vtx2tri(vid_remove))
    {
        if (CONTAINS(tid_remove, tid)) continue;
        vtx2tri.at(vid_keep).push_back(tid);
        if (tris.at(tid*3+0) == vid_remove) tris.at(tid*3+0) = vid_keep; else
        if (tris.at(tid*3+1) == vid_remove) tris.at(tid*3+1) = vid_keep; else
        if (tris.at(tid*3+2) == vid_remove) tris.at(tid*3+2) = vid_keep; else
        assert("Something is off here" && false);

        update_t_normal(tid);
    }

    // Migrate references from edge_remove to edge_keep
    //
    for(int tid : tid_remove)
    {
        int e_take = edge_opposite_to(tid, vid_remove);
        int e_give = edge_opposite_to(tid, vid_keep);
        assert(CONTAINS(edg_remove, e_give));

        for(int inc_tri : adj_edg2tri(e_give))
        {
            if (CONTAINS(tid_remove, inc_tri)) continue;

            for (int adj_tri : adj_edg2tri(e_take))
            {
                if (CONTAINS(tid_remove, adj_tri)) continue;

                tri2tri.at(inc_tri).push_back(adj_tri);
                tri2tri.at(adj_tri).push_back(inc_tri);
            }

            edg2tri.at(e_take).push_back(inc_tri);
            tri2edg.at(inc_tri).push_back(e_take);
        }
    }

    // remove references to vid_remove
    //
    for(uint vid : adj_vtx2vtx(vid_remove))
    {
        assert(vid!=vid_remove);
        assert(vid>=0 && int(vid)<num_vertices());

        auto beg = vtx2vtx.at(vid).begin();
        auto end = vtx2vtx.at(vid).end();
        vtx2vtx.at(vid).erase(std::remove(beg, end, vid_remove), end); // Erase-Remove idiom

        if (vid == vid_keep) continue;
        if (!vertices_are_adjacent(vid_keep,vid))
        {
            vtx2vtx.at(vid_keep).push_back(vid);
            vtx2vtx.at(vid).push_back(vid_keep);
        }
    }
    //
    // remove references to any edge in edg_remove.
    //
    for(int edg_rem : edg_remove)
    {
        assert(edg_rem>=0 && edg_rem< num_edges());

        for(int tid : adj_edg2tri(edg_rem))
        {
            assert(tid>= 0 && tid<num_triangles());
            auto beg = tri2edg.at(tid).begin();
            auto end = tri2edg.at(tid).end();
            tri2edg.at(tid).erase(std::remove(beg, end, edg_rem), end); // Erase-Remove idiom
        }

        for(int i=0; i<2; ++i)
        {
            int  vid = edge_vertex_id(edg_rem, i);
            auto beg = vtx2edg.at(vid).begin();
            auto end = vtx2edg.at(vid).end();
            vtx2edg.at(vid).erase(std::remove(beg, end, edg_rem), end); // Erase-Remove idiom
        }
    }
    //
    // remove references to any triangle in tri_remove.
    //
    for(int tid_rem : tid_remove)
    {
        for(int i=0; i<3; ++i)
        {
            int  vid = triangle_vertex_id(tid_rem,i);
            auto beg = vtx2tri.at(vid).begin();
            auto end = vtx2tri.at(vid).end();
            vtx2tri.at(vid).erase(std::remove(beg, end, tid_rem), end); // Erase-Remove idiom

            update_v_normal(vid);
        }

        for(int eid : adj_tri2edg(tid_rem))
        {
            auto beg = edg2tri.at(eid).begin();
            auto end = edg2tri.at(eid).end();
            edg2tri.at(eid).erase(std::remove(beg, end, tid_rem), end); // Erase-Remove idiom
        }

        for(int tid : adj_tri2tri(tid_rem))
        {
            auto beg = tri2tri.at(tid).begin();
            auto end = tri2tri.at(tid).end();
            tri2tri.at(tid).erase(std::remove(beg, end, tid_rem), end); // Erase-Remove idiom
        }
    }


    // clear
    vtx2vtx.at(vid_remove).clear();
    vtx2edg.at(vid_remove).clear();
    vtx2tri.at(vid_remove).clear();

    for(int eid : edg_remove)
    {
        edg2tri.at(eid).clear();
        edges.at(eid*2+0) = INT_MAX;
        edges.at(eid*2+1) = INT_MAX;
    }

    for(int tid : tid_remove)
    {
        tri2edg.at(tid).clear();
        tri2tri.at(tid).clear();
        tris.at(tid*3+0) = INT_MAX;
        tris.at(tid*3+1) = INT_MAX;
        tris.at(tid*3+2) = INT_MAX;
    }

    // Finalize

    // clean vectors...
    remove_unreferenced_vertex(vid_remove);

    std::vector<int> edg_remove_vec(edg_remove.begin(), edg_remove.end());
    std::sort(edg_remove_vec.begin(), edg_remove_vec.end());
    std::reverse(edg_remove_vec.begin(), edg_remove_vec.end());
    for(int eid : edg_remove_vec) remove_unreferenced_edge(eid);

    std::vector<int> tid_remove_vec(tid_remove.begin(), tid_remove.end());
    std::sort(tid_remove_vec.begin(), tid_remove_vec.end());
    std::reverse(tid_remove_vec.begin(), tid_remove_vec.end());
    for(int tid : tid_remove_vec) remove_unreferenced_triangle(tid);

    update_normals();

    return true;
}

CINO_INLINE
void Trimesh::edge_switch_id(const int eid0, const int eid1)
{
    for(int i=0; i<2; ++i)
    {
        std::swap(edges[2*eid0+i], edges[2*eid1+i]);
    }

    std::swap(edg2tri[eid0], edg2tri[eid1]);

    for(std::vector<int> & nbrs : vtx2edg)
    for(int & curr : nbrs)
    {
        if (curr == eid0) curr = eid1; else
        if (curr == eid1) curr = eid0;
    }

    for(std::vector<int> & nbrs : tri2edg)
    for(int & curr : nbrs)
    {
        if (curr == eid0) curr = eid1; else
        if (curr == eid1) curr = eid0;
    }
}

CINO_INLINE
void Trimesh::remove_unreferenced_edge(const int eid)
{
    edge_switch_id(eid, num_edges()-1);
    edges.resize(edges.size()-2);
    edg2tri.pop_back();
}


CINO_INLINE
bool Trimesh::barycentric_coordinates(const int tid, const vec3d & P, std::vector<double> & wgts) const
{
    assert(tid >= 0);
    assert(tid < num_triangles());
    return triangle_barycentric_coords(triangle_vertex(tid,0),
                                       triangle_vertex(tid,1),
                                       triangle_vertex(tid,2),
                                       P, wgts);
}

CINO_INLINE
bool Trimesh::elem_bary_is_vertex(const int                   tid,
                                  const std::vector<double> & wgts,
                                  int                       & vid,
                                  const double                tol) const
{
    int off;
    if (triangle_bary_is_vertex(wgts, off, tol))
    {
        vid = triangle_vertex_id(tid, off);
        return true;
    }
    return false;
}


CINO_INLINE
bool Trimesh::elem_bary_is_edge(const int                   tid,
                                const std::vector<double> & wgts,
                                int                       & eid,
                                const double                tol) const
{
    int off;
    if (triangle_bary_is_edge(wgts, off, tol))
    {
        eid = triangle_edge_local_to_global(tid, off);
        return true;
    }
    return false;
}


CINO_INLINE
void Trimesh::vertex_switch_id(const int vid0, const int vid1)
{
    for(uint & curr : edges)
    {
        if (curr == (uint)vid0) curr = vid1; else
        if (curr == (uint)vid1) curr = vid0;
    }

    for(uint & curr : tris)
    {
        if (curr == (uint)vid0) curr = vid1; else
        if (curr == (uint)vid1) curr = vid0;
    }

    for(int i=0; i<3; ++i)
    {
        std::swap(coords[3*vid0+i], coords[3*vid1+i]);
        std::swap(v_norm[3*vid0+i], v_norm[3*vid1+i]);
    }
    std::swap(u_text[vid0],  u_text[vid1]);
    std::swap(vtx2vtx[vid0], vtx2vtx[vid1]);
    std::swap(vtx2edg[vid0], vtx2edg[vid1]);
    std::swap(vtx2tri[vid0], vtx2tri[vid1]);

    for(std::vector<int> & nbrs : vtx2vtx)
    for(int & curr : nbrs)
    {
        if (curr == vid0) curr = vid1; else
        if (curr == vid1) curr = vid0;
    }
}

CINO_INLINE
void Trimesh::remove_unreferenced_vertex(const int vid)
{
    vertex_switch_id(vid, num_vertices()-1);

    coords.resize(coords.size()-3);
    v_norm.resize(v_norm.size()-3);
    u_text.resize(u_text.size()-1);
    vtx2vtx.pop_back();
    vtx2edg.pop_back();
    vtx2tri.pop_back();
}

CINO_INLINE
double Trimesh::edge_length(const int eid) const
{
    return (edge_vertex(eid, 0) - edge_vertex(eid, 1)).length();
}

CINO_INLINE
double Trimesh::avg_edge_length() const
{
    double sum = 0.0;
    for(int eid=0; eid<(int)num_edges(); ++eid)
    {
        sum += edge_length(eid);
    }
    return sum/=double(num_edges());
}

CINO_INLINE
double Trimesh::max_edge_length() const
{
    double max = 0.0;
    for(int eid=0; eid<(int)num_edges(); ++eid)
    {
        max = std::max(max, edge_length(eid));
    }
    return max;
}

CINO_INLINE
double Trimesh::min_edge_length() const
{
    double min = FLT_MAX;
    for(int eid=0; eid<(int)num_edges(); ++eid)
    {
        min = std::min(min, edge_length(eid));
    }
    return min;
}

CINO_INLINE
vec3d Trimesh::triangle_vertex(const int tid, const int offset) const
{
    int tid_ptr = tid * 3;
    int vid     = tris.at(tid_ptr + offset);
    int vid_ptr = vid * 3;
    return vec3d(coords.at(vid_ptr + 0), coords.at(vid_ptr + 1), coords.at(vid_ptr + 2));
}

CINO_INLINE
vec3d Trimesh::elem_vertex(const int eid, const int offset) const
{
    return triangle_vertex(eid, offset);
}

CINO_INLINE
bool Trimesh::edge_contains_vertex(const int eid, const int vid) const
{
    if (edge_vertex_id(eid,0) == vid) return true;
    if (edge_vertex_id(eid,1) == vid) return true;
    return false;
}

CINO_INLINE
vec3d Trimesh::edge_vertex(const int eid, const int offset) const
{
    int eid_ptr = eid * 2;
    int vid     = edges.at(eid_ptr + offset);
    int vid_ptr = vid * 3;
    return vec3d(coords.at(vid_ptr + 0), coords.at(vid_ptr + 1), coords.at(vid_ptr + 2));
}

CINO_INLINE
int Trimesh::shared_vertex(const int eid0, const int eid1) const
{
    int e00 = edge_vertex_id(eid0,0);
    int e01 = edge_vertex_id(eid0,1);
    int e10 = edge_vertex_id(eid1,0);
    int e11 = edge_vertex_id(eid1,1);

    if (e00 == e10 || e00 == e11) return e00;
    if (e01 == e10 || e01 == e11) return e01;

    assert(false);
}

CINO_INLINE
int Trimesh::shared_triangle(const int eid0, const int eid1) const
{
    std::vector<int> nbr_e0 = adj_edg2tri(eid0);
    std::vector<int> nbr_e1 = adj_edg2tri(eid1);

    for(int i=0; i<(int)nbr_e0.size(); ++i)
    for(int j=0; j<(int)nbr_e1.size(); ++j)
    {
        if (nbr_e0[i] == nbr_e1[j]) return nbr_e0[i];
    }

    assert(false);
}


CINO_INLINE
int Trimesh::add_vertex(const vec3d & v, const float scalar)
{
    int vid = num_vertices();
    //
    coords.push_back(v.x());
    coords.push_back(v.y());
    coords.push_back(v.z());
    //
    u_text.push_back(scalar);
    //
    v_norm.push_back(0); // vnx
    v_norm.push_back(0); // vny
    v_norm.push_back(0); // vnz
    //
    vtx2vtx.push_back(std::vector<int>());
    vtx2edg.push_back(std::vector<int>());
    vtx2tri.push_back(std::vector<int>());
    //
    bb.min = bb.min.min(v);
    bb.max = bb.max.max(v);

    return vid;
}

CINO_INLINE
void Trimesh::set_triangle(const int tid, const int vid0, const int vid1, const int vid2)
{
    /* WARNING!!!! This completely screws up edge connectivity!!!!!! */

    assert(vid0 < num_vertices());
    assert(vid1 < num_vertices());
    assert(vid2 < num_vertices());

    int tid_ptr = tid * 3;

    tris[tid_ptr + 0] = vid0;
    tris[tid_ptr + 1] = vid1;
    tris[tid_ptr + 2] = vid2;

    update_t_normal(tid);
}

CINO_INLINE
bool Trimesh::edges_share_same_triangle(const int eid1, const int eid2) const
{
    std::vector<int> tris1 = adj_edg2tri(eid1);
    std::vector<int> tris2 = adj_edg2tri(eid2);

    for(int i=0; i<(int)tris1.size(); ++i)
    for(int j=0; j<(int)tris2.size(); ++j)
    {
        if (tris1[i] == tris2[j]) return true;
    }
    return false;
}

CINO_INLINE
int Trimesh::triangle_adjacent_along(const int tid, const int vid0, const int vid1) const
{
    std::vector<int> nbrs = adj_tri2tri(tid);
    for(size_t i=0; i<nbrs.size(); ++i)
    {
        if (triangle_contains_vertex(nbrs[i], vid0) &&
            triangle_contains_vertex(nbrs[i], vid1))
        {
            return nbrs[i];
        }
    }
    assert(false);
}

CINO_INLINE
int Trimesh::add_triangle(const int vid0, const int vid1, const int vid2, const int scalar)
{
    assert(vid0 >= 0 && vid0 < num_vertices());
    assert(vid1 >= 0 && vid1 < num_vertices());
    assert(vid2 >= 0 && vid2 < num_vertices());

    int tid = num_triangles();
    //
    tris.push_back(vid0);
    tris.push_back(vid1);
    tris.push_back(vid2);
    //
    t_label.push_back(scalar);
    //
    tri2edg.push_back(std::vector<int>());
    tri2tri.push_back(std::vector<int>());
    //
    vtx2tri.at(vid0).push_back(tid);
    vtx2tri.at(vid1).push_back(tid);
    vtx2tri.at(vid2).push_back(tid);
    //
    ipair new_e[3]   = { unique_pair(vid0, vid1), unique_pair(vid1, vid2), unique_pair(vid2, vid0) };
    int   new_eid[3] = { -1, -1, -1 };
    for(int eid=0; eid<num_edges(); ++eid)
    {
        ipair e = unique_pair(edge_vertex_id(eid, 0), edge_vertex_id(eid, 1));
        for(int i=0; i<3; ++i) if (e == new_e[i]) new_eid[i] = eid;
    }
    //
    for(int i=0; i<3; ++i)
    {
        if (new_eid[i] == -1)
        {
            new_eid[i] = num_edges();
            edges.push_back(new_e[i].first);
            edges.push_back(new_e[i].second);
            edg2tri.push_back(std::vector<int>());
        }
        //
        for(int nbr : edg2tri.at(new_eid[i]))
        {
            tri2tri.at(nbr).push_back(tid);
            tri2tri.at(tid).push_back(nbr);
        }
        edg2tri.at(new_eid[i]).push_back(tid);
        tri2edg.at(tid).push_back(new_eid[i]);
    }
    //
    t_norm.push_back(0); //tnx
    t_norm.push_back(0); //tny
    t_norm.push_back(0); //tnz
    //
    update_t_normal(tid);
    update_v_normal(vid0);
    update_v_normal(vid1);
    update_v_normal(vid2);

    return tid;
}

CINO_INLINE
void Trimesh::triangle_switch_id(const int tid0, const int tid1)
{
    for(int i=0; i<3; ++i)
    {
        std::swap(tris[3*tid0+i],   tris[3*tid1+i]);
        std::swap(t_norm[3*tid0+i], t_norm[3*tid1+i]);
    }
    std::swap(t_label[tid0], t_label[tid1]);
    std::swap(tri2edg[tid0], tri2edg[tid1]);
    std::swap(tri2tri[tid0], tri2tri[tid1]);

    for(std::vector<int> & nbrs : vtx2tri)
    for(int & curr : nbrs)
    {
        if (curr == tid0) curr = tid1; else
        if (curr == tid1) curr = tid0;
    }

    for(std::vector<int> & nbrs : edg2tri)
    for(int & curr : nbrs)
    {
        if (curr == tid0) curr = tid1; else
        if (curr == tid1) curr = tid0;
    }

    for(std::vector<int> & nbrs : tri2tri)
    for(int & curr : nbrs)
    {
        if (curr == tid0) curr = tid1; else
        if (curr == tid1) curr = tid0;
    }
}

CINO_INLINE
void Trimesh::remove_unreferenced_triangle(const int tid)
{
    uint v1 = tris.at(tid*3+0);
    uint v2 = tris.at(tid*3+1);
    uint v3 = tris.at(tid*3+2);

    triangle_switch_id(tid, num_triangles()-1);

    assert (tris.at((num_triangles()-1)*3+0) == v1);
    assert (tris.at((num_triangles()-1)*3+1) == v2);
    assert (tris.at((num_triangles()-1)*3+2) == v3);

    tris.resize(tris.size()-3);
    t_norm.resize(t_norm.size()-3);
    t_label.resize(t_label.size()-1);
    tri2edg.pop_back();
    tri2tri.pop_back();
}

CINO_INLINE
std::vector<int> Trimesh::adj_vtx2vtx_ordered(const int vid) const
{
    std::vector<int> ordered_onering;

    int curr_e  = adj_vtx2edg(vid).front();
    int curr_v  = vertex_opposite_to(curr_e, vid);
    int curr_t  = adj_edg2tri(curr_e).front();
    int start_t = curr_t;

    logger << "curr_e " << curr_e << endl;
    logger << "curr_v " << curr_v << endl;
    logger << "curr_t " << curr_t << endl;

    do
    {
        logger << "iter" << endl;
        ordered_onering.push_back(curr_v);

        std::vector<int> adj_tri = adj_edg2tri(curr_e);
        assert(adj_tri.size() == 2);

        curr_e = edge_opposite_to(curr_t, curr_v);
        curr_t = (adj_tri[0] == curr_t) ? adj_tri[1] : adj_tri[0];
        curr_v = vertex_opposite_to(curr_e, vid);

        logger << "-------" << endl;
        logger << "curr_e " << curr_e << endl;
        logger << "curr_v " << curr_v << endl;
        logger << "curr_t " << curr_t << endl;
    }
    while(curr_t != start_t);

    assert(ordered_onering.size() == adj_vtx2vtx(vid).size());
    return ordered_onering;
}

CINO_INLINE
const std::vector<double> &Trimesh::vector_coords() const
{
    return coords;
}

CINO_INLINE
const std::vector<u_int> &Trimesh::vector_triangles() const
{
    return tris;
}

CINO_INLINE
const std::vector<u_int> &Trimesh::vector_edges() const
{
    return edges;
}


CINO_INLINE
const Bbox &Trimesh::bbox() const
{
    return bb;
}

CINO_INLINE
const std::vector<float> &Trimesh::vector_v_float_scalar() const
{
    return u_text;
}

CINO_INLINE
const std::vector<int> &Trimesh::vector_t_int_scalar() const
{
    return t_label;
}


CINO_INLINE
std::set<int> Trimesh::vertex_n_ring(const int vid, const int n) const
{
    std::set<int> active_set;
    std::set<int> unique_ring;

    active_set.insert(vid);

    for(int i=0; i<n; ++i)
    {
        std::set<int> next_active_set;

        for(int curr : active_set)
        {
            for(int nbr : adj_vtx2vtx(curr))
            {
                if (DOES_NOT_CONTAIN(unique_ring, nbr) && nbr != vid)
                {
                    next_active_set.insert(nbr);
                }
                unique_ring.insert(nbr);
            }
        }

        active_set = next_active_set;
    }

    return unique_ring;
    //vector<int> ring;
    //std::copy(unique_ring.begin(), unique_ring.end(), std::back_inserter(ring));
    //return ring;
}

CINO_INLINE
int Trimesh::connected_components(std::vector< std::set<int> > & ccs) const
{
    assert(ccs.empty());

    int seed = 0;
    std::vector<bool> visited(num_vertices(), false);

    do
    {
        std::set<int> cc;
        bfs_exahustive<Trimesh>(*this, seed, cc);

        ccs.push_back(cc);
        for(int vid : cc) visited[vid] = true;

        seed = 0;
        while (seed < num_vertices() && visited[seed]) ++seed;
    }
    while (seed < num_vertices());

    return ccs.size();
}

CINO_INLINE
int Trimesh::connected_components() const
{
    std::vector< std::set<int> > ccs;
    return connected_components(ccs);
}

CINO_INLINE
bool Trimesh::vertex_is_local_minima(const int vid) const
{
    float v = vertex_u_text(vid);
    for(int nbr : adj_vtx2vtx(vid))
    {
        if (vertex_u_text(nbr) < v) return false;
    }
    return true;
}

CINO_INLINE
bool Trimesh::vertex_is_local_maxima(const int vid) const
{
    float v = vertex_u_text(vid);
    for(int nbr : adj_vtx2vtx(vid))
    {
        if (vertex_u_text(nbr) > v) return false;
    }
    return true;
}

CINO_INLINE
bool Trimesh::vertex_is_critical_point(const int vid) const
{
    return (vertex_is_local_maxima(vid) || vertex_is_local_minima(vid));
}

}
