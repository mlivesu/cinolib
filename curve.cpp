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
#include <cinolib/curve.h>

namespace cinolib
{

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Curve::Sample & s)
{
    in << s.pos <<  " \t " << s.eid << " \t ";
    for(double w : s.bary) in << w << "  ";
    in << std::endl;
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const Curve & c)
{
    for(Curve::Sample s : c.samples()) in << s;
    in << std::endl;
    return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Curve::Curve()
{}


CINO_INLINE
Curve::Curve(const std::vector<vec3d> & samples)
{
    for(vec3d p : samples) append_sample(p);
    update_arc_length_param();
}

CINO_INLINE
Curve::Curve(const Skel & skel, const uint bone)
{
    std::vector<vec3d> samples;
    for(int vid : skel.vertex_bone(bone))
    {
        samples.push_back(skel.vertex(vid));
    }
    for(vec3d p : samples) append_sample(p);
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::reverse()
{
    std::reverse(sample_list.begin(), sample_list.end());
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Skel Curve::export_as_skeleton() const
{
    logger.disable();
    Skel s(vector_coords(), vector_segments());
    logger.enable();
    return s;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> Curve::vector_coords() const
{
    std::vector<double> coords;
    for(uint i=0; i<sample_list.size(); ++i)
    {
        coords.push_back(sample_list.at(i).pos.x());
        coords.push_back(sample_list.at(i).pos.y());
        coords.push_back(sample_list.at(i).pos.z());
    }
    return coords;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<int> Curve::vector_segments() const
{
    std::vector<int> segs;
    for(uint i=1; i<sample_list.size(); ++i)
    {
        segs.push_back(i-1);
        segs.push_back( i );
    }
    return segs;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::operator+=(const Curve & c)
{
    for(Sample s : c.samples()) append_sample(s);
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
const Bbox & Curve::bbox() const
{
    return bb;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
const std::vector<Curve::Sample> & Curve::samples() const
{
    return sample_list;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<Curve::Sample> & Curve::samples()
{
    return sample_list;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Curve::size() const
{
    return sample_list.size();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double Curve::length() const
{
    double l = 0.0;
    for(uint i=1; i<sample_list.size(); ++i)
    {
        l += sample_list.at(i).pos.dist(sample_list.at(i-1).pos);
    }
    return l;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::append_sample(const vec3d & s)
{
    Sample sample;
    sample.pos = s;
    append_sample(sample);
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::append_sample(const Sample & s)
{
    sample_list.push_back(s);
    bb.min = bb.min.min(s.pos);
    bb.max = bb.max.max(s.pos);
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::pop_back()
{
    sample_list.pop_back();
    update_arc_length_param();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::pop_front() // inefficient! - do it properly
{
    reverse();
    pop_back();
    reverse();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::update_arc_length_param() // recomputes parameter t for each sample
{
    if (size() < 2) return;

    double tot_length = length();
    double curr_l     = 0.0;
    double curr_t     = 0.0;

    for(uint i=1; i<sample_list.size()-1; ++i)
    {
        double seg_l   = sample_list.at(i-1).pos.dist(sample_list.at(i).pos);
        double delta_t = seg_l / tot_length;

        curr_t += delta_t;
        curr_l += seg_l;

        sample_list.at(i).t = curr_t;
    }

    sample_list.front().t = 0.0;
    sample_list.back().t  = 1.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Curve::last_sample_lower_equal_than(const float t) const
{
    assert(t>=0);
    assert(t<=1);

    for(size_t i=1; i<sample_list.size()-1; ++i)
    {
        if (sample_list.at(i).t > t) return i-1;
    }

    return sample_list.size()-1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Curve::sample_closest_to(const float t) const
{
    assert(t>=0);
    assert(t<=1);

    float best_err = FLT_MAX;
    uint  best_sam = 0;

    for(uint pos=0; pos<sample_list.size()-1; ++pos)
    {
        float err = fabs(sample_list.at(pos).t - t);
        if (err < best_err)
        {
            best_err = err;
            best_sam = pos;
        }
        else if (sample_list.at(pos).t > t) return best_sam;
    }

    return sample_list.size()-1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Curve::sample_curve_at(const float t) const
{
    return sample_curve_at(t, length());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Curve::sample_curve_at(const float t, const double tot_length) const
{
    assert(size() > 1);
    assert(tot_length > 0);
    assert(t>=0.0);
    assert(t<=1.0);

    double curr_l = 0.0;
    double curr_t = 0.0;
    for(uint i=1; i<sample_list.size(); ++i)
    {
        double seg_l   = sample_list.at(i-1).pos.dist(sample_list.at(i).pos);
        double delta_t = seg_l / tot_length;

        if (delta_t == 0) continue;

        if (curr_t + delta_t >= t - 1e-7)
        {
            double alpha = (t-curr_t)/(delta_t);
            return (1.0-alpha)*sample_list.at(i-1).pos + alpha*sample_list.at(i).pos;
        }
        else
        {
            curr_t += delta_t;
            curr_l += seg_l;
        }
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::resample_curve(const uint n_samples)
{
    assert(n_samples >= 2);

    double tot_length = length();
    double delta_t    = 1.0/double(n_samples-1);
    double t          = 0.0;

    if (size()  < 2) return;
    if (length()<=0) return;

    std::vector<Sample> new_samples;
    for(uint i=0; i<n_samples; ++i)
    {
        Sample s;
        s.pos = sample_curve_at(t,tot_length);
        new_samples.push_back(s);
        t += delta_t;
    }
    sample_list = new_samples;

    assert(new_samples.size() == n_samples);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> Curve::select_n_samples(const uint n_samples) const
{
    assert(size() >= n_samples);

    float i    = 0;
    float step = float(size()-1) / float(n_samples-1);

    std::vector<uint> list;

    do
    {
        uint new_pos = std::ceil(i);
        if (new_pos == samples().size()) --new_pos; // this may happen due to roundoff errors...
        list.push_back(new_pos);
        i += step;
    }
    while (list.size() < n_samples);

    return list;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> Curve::tessellate(Trimesh<> & m) const
{
    for(Sample s : sample_list)
    {
        if (s.eid == -1 || s.bary.size() < m.verts_per_elem(s.eid))
        {
            std::cerr << "WARNING: no link between mesh and curve existing." << std::endl;
            assert(false && "Impossible to tessellate the curve");
        }
    }

    // 1) Split all the edges crossed by the curve
    //
    std::vector<uint>   curve_vids;    // ordered list of mesh vertices belonging to the curve
    std::map<uint,uint> new_vids_map;  // for each edge, vid of the new vertex created
    std::set<uint>      faces_to_split; // list of the triangles that will require splitting
    for(uint i=0; i<sample_list.size(); ++i)
    {
        const Sample & s = sample_list.at(i);
        uint off;
        if (triangle_bary_is_edge(s.bary, off))
        {
            uint  eid         = m.face_edge_id(s.eid, off);
            uint  vid0        = m.face_vert_id(s.eid, TRI_EDGES[off][0]);
            uint  vid1        = m.face_vert_id(s.eid, TRI_EDGES[off][1]);
            float f0          = m.vert_data(vid0).uvw[0];
            float f1          = m.vert_data(vid1).uvw[0];
            float f_new       = f0 * s.bary.at(TRI_EDGES[off][0]) + f1 * s.bary.at(TRI_EDGES[off][1]);
            uint fresh_id     = m.vert_add(sample_list.at(i).pos);
            m.vert_data(fresh_id).uvw[0] = f_new;
            new_vids_map[eid] = fresh_id;

            curve_vids.push_back(fresh_id);
            for(uint fid : m.adj_e2f(eid)) faces_to_split.insert(fid);
        }
        else if (triangle_bary_is_vertex(s.bary, off))
        {
            curve_vids.push_back(m.face_vert_id(s.eid, off));
        }
        else assert(false && "Inner samples are not supported for tessellation yet");
    }

    // 2) Subdivide triangles according to the number of incident edges being split
    //
    std::vector<uint> split_list;
    for(uint fid : faces_to_split)
    {
        std::vector<ipair> split_edges;
        for(int eid : m.adj_f2e(fid))
        {
            auto query = new_vids_map.find(eid);
            if (query != new_vids_map.end())
            {
                split_edges.push_back(std::make_pair(eid, query->second));
            }
        }

        switch (split_edges.size())
        {
            case 1 : split_in_2(m, fid, split_edges.front(), split_list); break;
            case 2 : split_in_3(m, fid, split_edges.front(), split_edges.back(), split_list); break;
            default: assert(false && "Unsupported tessellation configuration! (to be added, probably)");
        }
    }

    tessellate(m, split_list);
    return curve_vids;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::tessellate(Trimesh<> &m, const std::vector<uint> & split_list) const
{
    // Serialized triangle split. Structure:
    //
    //      tid | # of sub-triangles | first tri (three entries each) | next tris (three entries each)
    //
    //      the first tri will be generated using the set_triangle() routine
    //      the next tris will be generated using the add_triangle() routine
    //
    for(uint i=0; i<split_list.size();)
    {
        uint fid     = split_list.at(i);
        uint subtris = split_list.at(i+1);
        uint v0      = split_list.at(i+2);
        uint v1      = split_list.at(i+3);
        uint v2      = split_list.at(i+4);
        vec3d n_og  = m.face_data(fid).normal;
        vec3d n     = triangle_normal(m.vert(v0), m.vert(v1), m.vert(v2));
        bool flip   = (n_og.dot(n) < 0); // if the normal is opposite flip them all...

        if (flip) std::swap(v1,v2);
        m.face_set(fid, v0, v1, v2);

        uint base = i+5;
        for(uint j=0; j<subtris-1; ++j)
        {
            v0 = split_list.at(base+3*j);
            v1 = split_list.at(base+3*j+1);
            v2 = split_list.at(base+3*j+2);
            if (flip) std::swap(v1,v2);
            m.face_add(v0, v1, v2);
        }

        i+=2+3*subtris;
    }

    // sanitize edge connectivity...
    logger.disable();
    m.update_adjacency();
    logger.enable();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::split_in_2(const Trimesh<>         & m,
                       const uint                fid,
                       const ipair             & edg,
                             std::vector<uint> & split_list) const
{
    uint eid   = edg.first;
    uint newv  = edg.second;
    uint old0  = m.edge_vert_id(eid,0);
    uint old1  = m.edge_vert_id(eid,1);
    uint pivot = m.vert_opposite_to(fid, old0, old1);

    split_list.push_back(fid);  // triangle to split
    split_list.push_back(2);    // # of sub-triangles to create
    split_list.push_back(old0);  split_list.push_back(pivot); split_list.push_back(newv); // t0
    split_list.push_back(old1);  split_list.push_back(newv);  split_list.push_back(pivot); // t1
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Curve::split_in_3(const Trimesh<>         & m,
                       const uint                fid,
                       const ipair             & edg0,
                       const ipair             & edg1,
                             std::vector<uint> & split_list) const
{
    uint eid0  = edg0.first;
    uint eid1  = edg1.first;
    uint newv0 = edg0.second;
    uint newv1 = edg1.second;
    uint pivot = m.vert_shared(eid0, eid1);
    uint old0  = m.edge_vert_id(eid0,0); if (old0==pivot) old0 = m.edge_vert_id(eid0,1);
    uint old1  = m.edge_vert_id(eid1,0); if (old1==pivot) old1 = m.edge_vert_id(eid1,1);

    split_list.push_back(fid);  // triangle to split
    split_list.push_back(3);    // # of sub-triangles to create
    split_list.push_back(newv0); split_list.push_back(newv1);  split_list.push_back(old0); // t0
    split_list.push_back(old0);  split_list.push_back(newv1);  split_list.push_back(old1); // t1
    split_list.push_back(pivot); split_list.push_back(newv1);  split_list.push_back(newv0); // t2
}

}
