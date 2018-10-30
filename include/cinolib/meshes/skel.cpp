/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/meshes/skel.h>
#include <cinolib/io/read_write.h>
#include <queue>

namespace cinolib
{

CINO_INLINE
void Skel::clear()
{
    bb.reset();
    coords.clear();
    segments.clear();
    max_spheres.clear();
    vtx_bone_ids.clear();
    seg_bone_ids.clear();
    vtx2vtx.clear();
    vtx2seg.clear();
    seg2seg.clear();
    vtx_bones.clear();
    seg_bones.clear();
}

CINO_INLINE
void Skel::operator+=(const Skel & s)
{
    int nv = num_vertices();
    int ns = num_segments();
    int nb = num_bones();

    for(int sid=0; sid<s.num_segments(); ++sid)
    {
        segments.push_back(ns + s.segment_vertex_id(sid,0));
        segments.push_back(ns + s.segment_vertex_id(sid,1));

        std::vector<int> tmp;
        for(int sid : s.seg2seg[sid]) tmp.push_back(ns + sid);
        seg2seg.push_back(tmp);

        seg_bone_ids.push_back(nb + s.seg_bone_ids.at(sid));
    }

    for(int vid=0; vid<s.num_vertices(); ++vid)
    {
        vec3d pos = s.vertex(vid);
        coords.push_back(pos.x());
        coords.push_back(pos.y());
        coords.push_back(pos.z());

        max_spheres.push_back(s.max_spheres.at(vid));
        vtx_bone_ids.push_back(nb + s.vtx_bone_ids.at(vid));

        std::vector<int> tmp;
        for(int sid : s.vtx2seg[vid]) tmp.push_back(ns + sid);
        vtx2seg.push_back(tmp);

        tmp.clear();
        for(int nbr : s.vtx2vtx[vid]) tmp.push_back(nv + nbr);
        vtx2vtx.push_back(tmp);
    }

    for(auto obj : s.vtx_bones)
    {
        for(int & vid : obj) vid += nv;
        vtx_bones.push_back(obj);
    }

    for(auto obj : s.seg_bones)
    {
        for(int & vid : obj) vid += ns;
        seg_bones.push_back(obj);
    }
    update_bbox();
}

CINO_INLINE
void Skel::load(const char * filename)
{
    clear();

    std::string s = filename;
    std::string last_three = s.substr(s.size() - 3, 3);
    std::string last_four  = s.substr(s.size() - 4, 4);
    std::string last_five  = s.substr(s.size() - 5, 5);

    if (last_five.compare(".skel") == 0)
    {
        read_Livesu2012(filename, coords, segments, max_spheres);
    }
    else if (last_three.compare(".cg") == 0)
    {
        read_Tagliasacchi2012(filename, coords, segments, max_spheres);
    }
    else if (last_four.compare(".skc") == 0)
    {
        read_DeyandSun2006(filename, coords, segments, max_spheres);
    }
    else if (last_four.compare(".csv") == 0)
    {
        read_CSV(filename, coords, segments, max_spheres);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : not supported skeleton type" << std::endl;
        exit(-1);
    }

    build_adjacency();
    build_bones();
    update_bbox();
}

CINO_INLINE
void Skel::load(const std::vector<double> & coords, const std::vector<int> & segments)
{
    this->coords   = coords;
    this->segments = segments;
    max_spheres.resize(coords.size()/3);
    build_adjacency();
    build_bones();
    update_bbox();
}

CINO_INLINE
void Skel::save(const char * filename) const
{
    write_Livesu2012(filename, coords, max_spheres, vtx2vtx);
}

CINO_INLINE
Skel::Skel(const std::vector<double> & coords, const std::vector<int> & segs)
{
    load(coords, segs);
}

CINO_INLINE
void Skel::build_adjacency()
{
    using namespace std;

    vtx2vtx.clear();
    vtx2vtx.resize(num_vertices());
    for(int bid=0; bid<num_segments(); ++bid)
    {
        seg b = segment(bid);
        vtx2vtx[b.first].push_back(b.second);
        vtx2vtx[b.second].push_back(b.first);
    }

    vtx2seg.clear();
    vtx2seg.resize(num_vertices());
    for(int sid=0; sid<num_segments(); ++sid)
    {
        seg s    = segment(sid);
        int vid0 = s.first;
        int vid1 = s.second;
        vtx2seg[vid0].push_back(sid);
        vtx2seg[vid1].push_back(sid);
    }

    seg2seg.clear();
    seg2seg.resize(num_segments());
    for(int sid=0; sid<num_segments(); ++sid)
    {
        seg s    = segment(sid);
        int vid0 = s.first;
        int vid1 = s.second;

        std::vector<int> segs;
        std::vector<int> adj_segs_v0 = adj_vtx2seg(vid0);
        std::vector<int> adj_segs_v1 = adj_vtx2seg(vid1);
        segs.insert(segs.end(), adj_segs_v0.begin(), adj_segs_v0.end());
        segs.insert(segs.end(), adj_segs_v1.begin(), adj_segs_v1.end());
        for(int i=0; i<(int)segs.size(); ++i)
        {
            if (segs[i] == sid) continue;
            seg2seg[sid].push_back(segs[i]);
        }
    }
}

CINO_INLINE
void Skel::build_bones()
{
    vtx_bone_ids.clear();
    vtx_bone_ids.resize(num_vertices());
    vtx_bones.clear();

    seg_bone_ids.clear();
    seg_bone_ids.resize(num_segments());
    seg_bones.clear();

    std::queue<int> q;
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        if (vertex_is_feature(vid)) q.push(vid);
        set_vertex_bone_id(vid, -1);
    }

    while (!q.empty())
    {
        int fp = q.front();
        q.pop();

        std::vector<int> nbrs = adj_vtx2vtx(fp);
        for(int i=0; i<(int)nbrs.size(); ++i)
        {
            if (vertex_bone_id(nbrs[i]) != -1) continue;

            int curr     = nbrs[i];
            int prev     = fp;
            int fresh_id = vtx_bones.size();

            std::vector<int> v_bone;
            v_bone.push_back(fp);

            while (vertex_is_bone(curr))
            {
                set_vertex_bone_id(curr, fresh_id);
                v_bone.push_back(curr);

                int next = (adj_vtx2vtx(curr,0) == prev) ? adj_vtx2vtx(curr,1) : adj_vtx2vtx(curr,0);

                prev = curr;
                curr = next;
            }
            v_bone.push_back(curr);
            vtx_bones.push_back(v_bone);

            std::vector<int> s_bone;
            for(int i=1; i<(int)v_bone.size(); ++i)
            {
                int curr = v_bone[i];
                int prev = v_bone[i-1];
                std::vector<int> segs = adj_vtx2seg(curr);
                for(int j=0; j<(int)segs.size(); ++j)
                {
                    if (segment_contains_vertex(segs[j], prev))
                    {
                        s_bone.push_back(segs[j]);
                        set_segment_bone_id(segs[j], fresh_id);

                        // consistently orient segments throughout the bone
                        if (segment_vertex_id(segs[j],0) == curr)
                        {
                            segment_switch_order(segs[j]);
                        }
                    }
                }
            }
            seg_bones.push_back(s_bone);

            std::cout << "New Bone\t" << v_bone.size() << " vertices\t" << s_bone.size() << " segments." << std::endl;
        }
    }
    std::cout << vtx_bones.size() << " bone(s) found!" << std::endl;
}

CINO_INLINE
void Skel::update_bbox()
{
    bb.reset();
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        vec3<double> v = vertex(vid);
        bb.min = bb.min.min(v);
        bb.max = bb.max.max(v);
    }
}

CINO_INLINE
void Skel::remove_vertices(std::vector<int> & to_remove)
{
    std::sort(to_remove.begin(), to_remove.end());
    std::reverse(to_remove.begin(), to_remove.end());

    for(int i=0; i<(int)to_remove.size(); ++i)
    {
        remove_vertex(to_remove[i]);
    }
}

CINO_INLINE
void Skel::remove_vertex(int vid_to_remove)
{
    assert(vertex_is_bone(vid_to_remove));

    std::vector<double> tmp_coords;
    std::vector<double> tmp_max_spheres;
    for(int vid=0; vid<num_vertices(); ++vid)
    {
        if (vid == vid_to_remove) continue;

        int vid_ptr = vid * 3;
        CHECK_BOUNDS(coords, vid_ptr+2);
        tmp_coords.push_back(coords[vid_ptr + 0]);
        tmp_coords.push_back(coords[vid_ptr + 1]);
        tmp_coords.push_back(coords[vid_ptr + 2]);
        CHECK_BOUNDS(max_spheres, vid);
        tmp_max_spheres.push_back(max_spheres[vid]);
    }

    std::vector<int> tmp_segments;
    std::vector<int> to_connect;
    for (int sid=0; sid<num_segments(); ++sid)
    {
        int sid_ptr = sid * 2;
        CHECK_BOUNDS(segments, sid_ptr+1);
        int v0 = segments[sid_ptr + 0];
        int v1 = segments[sid_ptr + 1];

        if (v0 == vid_to_remove)
        {
            to_connect.push_back((v1 < vid_to_remove) ? v1 : v1 - 1);
        }
        else if (v1 == vid_to_remove)
        {
            to_connect.push_back((v0 < vid_to_remove) ? v0 : v0 - 1);
        }
        else
        {
            tmp_segments.push_back((v0 < vid_to_remove) ? v0 : v0 - 1);
            tmp_segments.push_back((v1 < vid_to_remove) ? v1 : v1 - 1);
        }
    }
    assert(to_connect.size() == 2);
    tmp_segments.push_back(to_connect[0]);
    tmp_segments.push_back(to_connect[1]);

    clear();

    for(int i=0; i<(int)tmp_coords.size();      ++i) coords.push_back(tmp_coords[i]);
    for(int i=0; i<(int)tmp_segments.size();    ++i) segments.push_back(tmp_segments[i]);
    for(int i=0; i<(int)tmp_max_spheres.size(); ++i) max_spheres.push_back(tmp_max_spheres[i]);

    build_adjacency();
    build_bones();
    update_bbox();
}

CINO_INLINE
int Skel::sample_segment_at(int sid, float q) // q \in [0,1]
{
    int vid_0 = segment_vertex_id(sid, 0);
    int vid_1 = segment_vertex_id(sid, 1);
    int vid_m = num_vertices();

    if (q < 0.01)             return vid_0;
    if (fabs(q - 1.0) < 0.01) return vid_1;

    vec3d p = vertex(vid_0) * (1.0 - q) +
              vertex(vid_1) * q;

    coords.push_back(p.x());
    coords.push_back(p.y());
    coords.push_back(p.z());

    float r = max_sphere_radius(vid_0) * (1.0 - q) +
              max_sphere_radius(vid_1) * q;

    max_spheres.push_back(r);

    int vid = segment_vertex_id(sid, 1);
    set_segment(sid, 1, vid_m);

    segments.push_back(vid);
    segments.push_back(vid_m);

    build_adjacency();
    build_bones();

    return vid_m;
}

CINO_INLINE
vec3d Skel::get_bone_sample(int bid, float q) const // q \in [0,1]
{
    assert(q   >= 0.0);
    assert(q   <= 1.0);
    assert(bid >= 0  );

    //assert(bid <  num_bones());
    while (bid >= num_bones()) bid--; // TEMPORARY! Needs bugfix

    std::vector<int> bone = segment_bone(bid);

    int i   = 0;
    int sid = bone[i];

    double tot_length   = bone_length(bid);
    double split_length = tot_length * q;
    double curr_length  = 0.0;
    double segm_length  = segment_length(sid);

    while (curr_length + segm_length < split_length)
    {
        curr_length += segm_length;
        sid = bone[++i];
        segm_length  = segment_length(sid);
    }

    float residual = split_length - curr_length;

    q = residual / std::max(1e-3, segm_length);

    return get_segment_sample(sid, q);
}

CINO_INLINE
vec3d Skel::get_segment_sample(int sid, float q) const // q \in [0,1]
{
    assert(q   >= 0.0);
    assert(q   <= 1.0);
    assert(sid >= 0  );
    assert(sid <  num_segments());

    int vid_0 = segment_vertex_id(sid, 0);
    int vid_1 = segment_vertex_id(sid, 1);

    if (q < 0.01)             return vertex(vid_0);
    if (fabs(q - 1.0) < 0.01) return vertex(vid_1);

    return (vertex(vid_0) * (1.0 - q) + vertex(vid_1) * q);
}

CINO_INLINE
int Skel::sample_bone_at(int bid, float q) // q \in [0,1]
{
    std::vector<int> bone = segment_bone(bid);

    int i   = 0;
    int sid = bone[i];

    double tot_length   = bone_length(bid);
    double split_length = tot_length * q;
    double curr_length  = 0.0;
    double segm_length  = segment_length(sid);

    while (curr_length + segm_length < split_length)
    {
        curr_length += segm_length;
        sid = bone[++i];
        segm_length  = segment_length(sid);
    }

    float residual = split_length - curr_length;

    q = residual / std::max(1e-3, segm_length);

    return sample_segment_at(sid, q);
}

CINO_INLINE
bool Skel::are_bones_adjacent(const int bone_a, const int bone_b) const
{
    std::vector<int> joints = get_joints();

    for(int fp : joints)
    {
        bool has_l1 = false;
        bool has_l2 = false;
        for(int nbr : adj_vtx2vtx(fp))
        {
            if (vertex_bone_id(nbr) == bone_a) has_l1 = true;
            if (vertex_bone_id(nbr) == bone_b) has_l2 = true;
        }
        if (has_l1 && has_l2) return true;
    }
    return false;
}

CINO_INLINE
int Skel::add_vertex(const vec3d & v)
{
    coords.push_back(v.x());
    coords.push_back(v.y());
    coords.push_back(v.z());

    std::vector<int> empty;
    vtx2vtx.push_back(empty);
    vtx2seg.push_back(empty);

    vtx_bone_ids.push_back(0);
    max_spheres.push_back(0);

    return (num_vertices()/3 - 1);
}

CINO_INLINE
int Skel::add_segment(int vid0, int vid1)
{
    segments.push_back(vid0);
    segments.push_back(vid1);

    std::vector<int> empty;
    seg2seg.push_back(empty);
    seg_bone_ids.push_back(0);

    build_adjacency();
    build_bones();

    return (num_segments()/2 - 1);
}

}
