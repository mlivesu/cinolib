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
#include "trimesh.h"
#include "../timer.h"
#include "../io/read_write.h"

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
    if (t_label.empty()) t_label.resize(num_triangles(), 0);
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

        for(int i=0; i<(int)tids.size(); ++i)
        {
            int tid = tids[i];

            tri2edg[tid].push_back(eid);
            edg2tri[eid].push_back(tid);
        }
        if (tids.size() == 2)
        {
            tri2tri[tids[0]].push_back(tids[1]);
            tri2tri[tids[1]].push_back(tids[0]);
            assert(tri2tri[tids[0]].size() <= 3);
            assert(tri2tri[tids[1]].size() <= 3);
        }
    }

    std::cout << num_vertices()  << "\tvertices"  << std::endl;
    std::cout << num_triangles() << "\ttriangles" << std::endl;
    std::cout << num_edges()     << "\tedges"     << std::endl;

    timer_stop("Build adjacency");
}

CINO_INLINE
void Trimesh::update_t_normals()
{
    t_norm.clear();
    t_norm.resize(num_triangles()*3);

    for(int tid=0; tid<num_triangles(); ++tid)
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
void Trimesh::update_v_normals()
{
    v_norm.clear();
    v_norm.resize(num_vertices()*3);

    for(int vid=0; vid<num_vertices(); ++vid)
    {
        std::vector<int> nbrs = adj_vtx2tri(vid);

        vec3d sum(0,0,0);
        for(int i=0; i<(int)nbrs.size(); ++i)
        {
            sum += triangle_normal(nbrs[i]);
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
void Trimesh::load(const char * filename)
{
    timer_start("Load Trimesh");

    clear();

    std::string str(filename);
    std::string filetype = str.substr(str.size()-3,3);

    if (filetype.compare("off") == 0 ||
        filetype.compare("OFF") == 0)
    {
        read_OFF(filename, coords, tris);
    }
    else
    if (filetype.compare("obj") == 0 ||
        filetype.compare("OBJ") == 0)
    {
        read_OBJ(filename, coords, tris);
    }
    else
    if (filetype.compare(".iv") == 0 ||
        filetype.compare(".IV") == 0)
    {
        read_IV(filename, coords, tris, t_label);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << std::endl;
        exit(-1);
    }

    std::cout << tris.size() / 3   << " triangles read" << std::endl;
    std::cout << coords.size() / 3 << " vertices  read" << std::endl;

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
        write_OFF(filename, coords, tris);
    }
    else
    if (filetype.compare("obj") == 0 ||
        filetype.compare("OBJ") == 0)
    {
        write_OBJ(filename, coords, tris);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save() : file format not supported yet " << std::endl;
        exit(-1);
    }

    std::cout << tris.size() / 3   << " triangles written" << std::endl;
    std::cout << coords.size() / 3 << " vertices  written" << std::endl;

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
    assert(false);
}

CINO_INLINE
double Trimesh::angle_at_vertex(const int tid, const int vid) const
{
    int i;

         if (triangle_vertex_id(tid,0) == vid) i = 0;
    else if (triangle_vertex_id(tid,1) == vid) i = 1;
    else if (triangle_vertex_id(tid,2) == vid) i = 2;
    else assert(false);

    vec3d P = vertex(vid);
    vec3d u = triangle_vertex(tid, (i+1)%3) - P; u.normalize();
    vec3d v = triangle_vertex(tid, (i+2)%3) - P; v.normalize();

    double angle = acos(u.dot(v)); assert(!std::isnan(angle));
    return angle;
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
void Trimesh::remove_duplicated_triangles()
{
    timer_start("Remove duplicated triangles from trimesh");

    std::set< std::vector<int> > unique_tris;
    for(int tid=0; tid<num_triangles(); ++tid)
    {
        std::vector<int> tri(3);
        tri[0] = triangle_vertex_id(tid,0);
        tri[1] = triangle_vertex_id(tid,1);
        tri[2] = triangle_vertex_id(tid,2);
        sort(tri.begin(), tri.end());
        if (DOES_NOT_CONTAIN(unique_tris, tri))
        {
            unique_tris.insert(tri);
        }
    }

    std::vector<double> new_coords = coords;
    std::vector<uint>   new_tris;
    for(auto tri : unique_tris)
    {
        new_tris.push_back(tri[0]);
        new_tris.push_back(tri[1]);
        new_tris.push_back(tri[2]);
    }

    std::cout << (tris.size() - new_tris.size())/3 << " duplicated triangles have been removed" << std::endl;

    clear();
    coords = new_coords;
    tris   = new_tris;
    init();

    timer_stop("Remove duplicated triangles from trimesh");
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
void Trimesh::normalize_area()
{
    double area = 0.0;
    for(int tid=0; tid<num_triangles(); ++tid)
    {
        area += element_mass(tid);
    }
    if (area < 1e-4)
    {
        std::cerr << "\nWARNING!! Trimesh Area is close to zero: " << area << std::endl << std::endl;
        area = 1e-4;
    }
    double s = 1.0 / sqrt(area);
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        set_vertex(vid, vertex(vid) * s);
    }
}

CINO_INLINE
vec3d Trimesh::element_barycenter(const int tid) const
{
    vec3d b(0,0,0);
    for(int i=0; i<3; ++i)
    {
        b += triangle_vertex(tid,i);
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
    coords.push_back(v.x());
    coords.push_back(v.y());
    coords.push_back(v.z());
    u_text.push_back(scalar);
    return vid;
}

CINO_INLINE
void Trimesh::set_triangle(const int tid, const int vid0, const int vid1, const int vid2)
{
    assert(vid0 < num_vertices());
    assert(vid1 < num_vertices());
    assert(vid2 < num_vertices());

    int tid_ptr = tid * 3;

    tris[tid_ptr + 0] = vid0;
    tris[tid_ptr + 1] = vid1;
    tris[tid_ptr + 2] = vid2;
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
    assert(vid0 < num_vertices());
    assert(vid1 < num_vertices());
    assert(vid2 < num_vertices());

    int tid = num_triangles();
    tris.push_back(vid0);
    tris.push_back(vid1);
    tris.push_back(vid2);
    t_label.push_back(scalar);
    return tid;
}

CINO_INLINE
std::vector<int> Trimesh::adj_vtx2vtx_ordered(const int vid) const
{
    std::vector<int> ordered_onering;

    int curr_e  = adj_vtx2edg(vid).front();
    int curr_v  = vertex_opposite_to(curr_e, vid);
    int curr_t  = adj_edg2tri(curr_e).front();
    int start_t = curr_t;

    std::cout << "curr_e " << curr_e << std::endl;
    std::cout << "curr_v " << curr_v << std::endl;
    std::cout << "curr_t " << curr_t << std::endl;

    do
    {
        std::cout << "iter" << std::endl;
        ordered_onering.push_back(curr_v);

        std::vector<int> adj_tri = adj_edg2tri(curr_e);
        assert(adj_tri.size() == 2);

        curr_e = edge_opposite_to(curr_t, curr_v);
        curr_t = (adj_tri[0] == curr_t) ? adj_tri[1] : adj_tri[0];
        curr_v = vertex_opposite_to(curr_e, vid);

        std::cout << "-------" << std::endl;
        std::cout << "curr_e " << curr_e << std::endl;
        std::cout << "curr_v " << curr_v << std::endl;
        std::cout << "curr_t " << curr_t << std::endl;
    }
    while(curr_t != start_t);

    assert(ordered_onering.size() == adj_vtx2vtx(vid).size());
    return ordered_onering;
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

}
