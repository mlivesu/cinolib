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
#include <cinolib/meshes/trimesh.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const char * filename)
{
    this->load(filename);
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<vec3d>              & verts,
                          const std::vector<std::vector<uint>>  & faces)

{
    this->verts = verts;
    this->polys = faces;
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<double>             & coords,
                          const std::vector<std::vector<uint>>  & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->polys = faces;
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<vec3d> & verts,
                          const std::vector<uint>  & faces)
{
    this->verts = verts;
    this->polys = faces_from_serialized_vids(faces,3);
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Trimesh<M,V,E,P>::Trimesh(const std::vector<double> & coords,
                          const std::vector<uint>   & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->polys = faces_from_serialized_vids(faces,3);
    this->init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::update_p_normal(const uint pid)
{
    vec3d A = this->poly_vert(pid,0);
    vec3d B = this->poly_vert(pid,1);
    vec3d C = this->poly_vert(pid,2);
    vec3d n = (B-A).cross(C-A);
    n.normalize();
    this->poly_data(pid).normal = n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int Trimesh<M,V,E,P>::edge_opposite_to(const uint pid, const uint vid) const
{
    assert(this->poly_contains_vert(pid, vid));
    for(uint eid : this->adj_p2e(pid))
    {
        if (this->edge_vert_id(eid,0) != vid &&
            this->edge_vert_id(eid,1) != vid) return eid;
    }
    return -1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::edge_collapse(const uint eid)
{
    // define what to keep and what to remove
    //
    uint vid_keep   = this->edge_vert_id(eid,0);
    uint vid_remove = this->edge_vert_id(eid,1);
    std::set<uint> pid_remove(this->adj_e2p(eid).begin(), this->adj_e2p(eid).end());
    std::set<uint> edg_keep, edg_remove;
    for(uint pid: pid_remove)
    {
        edg_keep.insert(edge_opposite_to(pid, vid_remove));
        edg_remove.insert(edge_opposite_to(pid, vid_keep));
    }
    edg_remove.insert(eid);

    // Pre-processing : check edges
    //
    for(uint eid : this->adj_v2e(vid_remove))
    {
        if (CONTAINS(edg_remove,eid)) continue;

        uint vid0 = this->edge_vert_id(eid,0);
        uint vid1 = this->edge_vert_id(eid,1);
        //
        uint vid0_mod = vid0;
        uint vid1_mod = vid1;
        //
        if (vid0 == vid_remove) vid0_mod = vid_keep; else
        if (vid1 == vid_remove) vid1_mod = vid_keep; else
        assert("Something is off here" && false);

        // check edge
        for(uint eid2=0; eid2<this->num_edges(); ++eid2)
        {
            if (eid2==eid) continue;

            if ((vid0_mod == vid0 && vid1_mod == vid1) ||
                (vid0_mod == vid1 && vid1_mod == vid0))
            {
                logger << "WARNING : duplicared edge " << eid << " : Impossible to perform edge collapse. " << endl;
                return false;
            }
        }
    }

    // Pre-processing : check triangles
    //
    for(uint pid : this->adj_v2p(vid_remove))
    {
        if (CONTAINS(pid_remove, pid)) continue;

        vec3d n_old = this->poly_data(pid).normal;
        uint vid0   = this->poly_vert_id(pid,0);
        uint vid1   = this->poly_vert_id(pid,1);
        uint vid2   = this->poly_vert_id(pid,2);

        if (vid0 == vid_remove) vid0 = vid_keep; else
        if (vid1 == vid_remove) vid1 = vid_keep; else
        if (vid2 == vid_remove) vid2 = vid_keep; else
        assert("Something is off here" && false);

        // check triangle flip
        vec3d v0 = this->vert(vid0);
        vec3d v1 = this->vert(vid1);
        vec3d v2 = this->vert(vid2);
        //
        vec3d u = v1 - v0;    u.normalize();
        vec3d v = v2 - v0;    v.normalize();
        vec3d n = u.cross(v); n.normalize();

        if (n.length() == 0)
        {
            logger << "WARNING : triangle on colinear points " << pid << " : Impossible to perform edge collapse. " << endl;
            return false;
        }

        if (n.dot(n_old) < 0 ) // triangle inversion
        {
            logger << "WARNING : triangle inversion " << pid << " : Impossible to perform edge collapse. " << std::endl;
            return false;
        }
    }

    // Everything is ok
    // The edge can be collapsed

    for(uint eid : this->adj_v2e(vid_remove))
    {
        if (CONTAINS(edg_remove, eid)) continue;

        this->v2e.at(vid_keep).push_back(eid);
        if (this->edge_vert_id(eid,0) == vid_remove) this->edges.at(eid*2+0) = vid_keep; else
        if (this->edge_vert_id(eid,1) == vid_remove) this->edges.at(eid*2+1) = vid_keep; else
        assert("Something is off here" && false);
    }
    //
    for(uint pid : this->adj_v2p(vid_remove))
    {
        if (CONTAINS(pid_remove, pid)) continue;

        this->v2p.at(vid_keep).push_back(pid);
        if (this->poly_vert_id(pid,0) == vid_remove) this->polys.at(pid*3+0) = vid_keep; else
        if (this->poly_vert_id(pid,0) == vid_remove) this->polys.at(pid*3+1) = vid_keep; else
        if (this->poly_vert_id(pid,0) == vid_remove) this->polys.at(pid*3+2) = vid_keep; else
        assert("Something is off here" && false);

        update_p_normal(pid);
    }

    // Migrate references from edge_remove to edge_keep
    //
    for(uint pid : pid_remove)
    {
        int e_take = edge_opposite_to(pid, vid_remove); assert(e_take >= 0);
        int e_give = edge_opposite_to(pid, vid_keep);   assert(e_give >= 0);
        assert(CONTAINS(edg_remove, e_give));

        for(uint inc_f : this->adj_e2p(e_give))
        {
            if (CONTAINS(pid_remove, inc_f)) continue;

            for (uint adj_f : this->adj_e2p(e_take))
            {
                if (CONTAINS(pid_remove, adj_f)) continue;

                this->p2p.at(inc_f).push_back(adj_f);
                this->p2p.at(adj_f).push_back(inc_f);
            }

            this->e2p.at(e_take).push_back(inc_f);
            this->p2e.at(inc_f).push_back(e_take);
        }
    }

    // remove references to vid_remove
    //
    for(uint vid : this->adj_v2v(vid_remove))
    {
        assert(vid!=vid_remove && vid<this->num_verts());

        auto beg = this->v2v.at(vid).begin();
        auto end = this->v2v.at(vid).end();
        this->v2v.at(vid).erase(std::remove(beg, end, vid_remove), end); // Erase-Remove idiom

        if (vid == vid_keep) continue;
        if (!this->verts_are_adjacent(vid_keep,vid))
        {
            this->v2v.at(vid_keep).push_back(vid);
            this->v2v.at(vid).push_back(vid_keep);
        }
    }
    //
    // remove references to any edge in edg_remove.
    //
    for(uint edg_rem : edg_remove)
    {
        assert(edg_rem<this->num_edges());

        for(uint pid : this->adj_e2p(edg_rem))
        {
            assert(pid<this->num_polys());
            auto beg = this->p2e.at(pid).begin();
            auto end = this->p2e.at(pid).end();
            this->p2e.at(pid).erase(std::remove(beg, end, edg_rem), end); // Erase-Remove idiom
        }

        for(uint i=0; i<2; ++i)
        {
            uint vid = this->edge_vert_id(edg_rem, i);
            auto beg = this->v2e.at(vid).begin();
            auto end = this->v2e.at(vid).end();
            this->v2e.at(vid).erase(std::remove(beg, end, edg_rem), end); // Erase-Remove idiom
        }
    }
    //
    // remove references to any triangle in tri_remove.
    //
    for(uint pid_rem : pid_remove)
    {
        for(uint off=0; off<3; ++off)
        {
            uint vid = this->poly_vert_id(pid_rem,off);
            auto beg = this->v2p.at(vid).begin();
            auto end = this->v2p.at(vid).end();
            this->v2p.at(vid).erase(std::remove(beg, end, pid_rem), end); // Erase-Remove idiom
            this->update_v_normal(vid);
        }

        for(uint eid : this->adj_p2e(pid_rem))
        {
            auto beg = this->e2p.at(eid).begin();
            auto end = this->e2p.at(eid).end();
            this->e2p.at(eid).erase(std::remove(beg, end, pid_rem), end); // Erase-Remove idiom
        }

        for(uint pid : this->adj_p2p(pid_rem))
        {
            auto beg = this->p2p.at(pid).begin();
            auto end = this->p2p.at(pid).end();
            this->p2p.at(pid).erase(std::remove(beg, end, pid_rem), end); // Erase-Remove idiom
        }
    }

    // clear
    this->v2v.at(vid_remove).clear();
    this->v2e.at(vid_remove).clear();
    this->v2p.at(vid_remove).clear();

    for(uint eid : edg_remove)
    {
        this->e2p.at(eid).clear();
        this->edges.at(eid*2+0) = INT_MAX;
        this->edges.at(eid*2+1) = INT_MAX;
    }

    for(uint pid : pid_remove)
    {
        this->p2e.at(pid).clear();
        this->p2p.at(pid).clear();
        this->polys.at(pid*3+0) = INT_MAX;
        this->polys.at(pid*3+1) = INT_MAX;
        this->polys.at(pid*3+2) = INT_MAX;
    }

    // Finalize

    vert_remove_unreferenced(vid_remove);

    std::vector<uint> edg_remove_vec(edg_remove.begin(), edg_remove.end());
    std::sort(edg_remove_vec.begin(), edg_remove_vec.end());
    std::reverse(edg_remove_vec.begin(), edg_remove_vec.end());
    for(uint eid : edg_remove_vec) edge_remove_unreferenced(eid);

    std::vector<uint> pid_remove_vec(pid_remove.begin(), pid_remove.end());
    std::sort(pid_remove_vec.begin(), pid_remove_vec.end());
    std::reverse(pid_remove_vec.begin(), pid_remove_vec.end());
    for(uint tid : pid_remove_vec) poly_remove_unreferenced(tid);

    this->update_normals();

    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::edge_add(const uint vid0, const uint vid1)
{
    assert(vid0 < this->num_verts());
    assert(vid1 < this->num_verts());
    //
    uint id = this->num_edges();
    //
    this->edges.push_back(vid0);
    this->edges.push_back(vid1);
    //
    this->e2p.push_back(std::vector<uint>());
    //
    E data;
    this->e_data.push_back(data);
    //
    return id;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::edge_switch_id(const uint eid0, const uint eid1)
{
    for(uint off=0; off<2; ++off) std::swap(this->edges.at(2*eid0+off), this->edges.at(2*eid1+off));

    std::swap(this->e2p.at(eid0), this->e2p.at(eid1));

    for(std::vector<uint> & nbrs : this->v2e)
    for(uint & curr : nbrs)
    {
        if (curr == eid0) curr = eid1; else
        if (curr == eid1) curr = eid0;
    }

    for(std::vector<uint> & nbrs : this->p2e)
    for(uint & curr : nbrs)
    {
        if (curr == eid0) curr = eid1; else
        if (curr == eid1) curr = eid0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::edge_remove_unreferenced(const uint eid)
{
    edge_switch_id(eid, this->num_edges()-1);
    this->edges.resize(this->edges.size()-2);
    this->e_data.pop_back();
    this->e2p.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_bary_coords(const uint pid, const vec3d & p, std::vector<double> & wgts) const
{
    return triangle_barycentric_coords(this->poly_vert(pid,0),
                                       this->poly_vert(pid,1),
                                       this->poly_vert(pid,2),
                                       p, wgts);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_bary_is_vert(const uint pid, const std::vector<double> & wgts, uint & vid, const double tol) const
{
    uint off;
    if (triangle_bary_is_vertex(wgts, off, tol))
    {
        vid = this->poly_vert_id(pid, off);
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_bary_is_edge(const uint pid, const std::vector<double> & wgts, uint & eid, const double tol) const
{
    uint off;
    if (triangle_bary_is_edge(wgts, off, tol))
    {
        eid = poly_edge_id(pid, off);
        return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::poly_edge_id(const uint pid, const uint offset) const
{
    uint vid0 = this->poly_vert_id(pid, TRI_EDGES[offset][0]);
    uint vid1 = this->poly_vert_id(pid, TRI_EDGES[offset][1]);

    for(uint eid : this->adj_p2e(pid))
    {
        if (this->edge_contains_vert(eid,vid0) &&
            this->edge_contains_vert(eid,vid1))
        {
            return eid;
        }
    }
    assert(false && "Something is off here...");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// for a definition of cap and needles, see:
// A Robust Procedure to Eliminate Degenerate Faces from Triangle Meshes
// Mario Botsch, Leif P. Kobbelt
// VMW 2001
//
template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_is_cap(const uint pid, const double angle_thresh_deg) const
{
    for(uint offset=0; offset<this->verts_per_poly(pid); ++offset)
    {
        uint vid = this->poly_vert_id(pid,offset);
        if (this->poly_angle_at_vert(pid,vid,DEG) > angle_thresh_deg)
        {
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// for a definition of cap and needles, see:
// A Robust Procedure to Eliminate Degenerate Faces from Triangle Meshes
// Mario Botsch, Leif P. Kobbelt
// VMW 2001
//
template<class M, class V, class E, class P>
CINO_INLINE
bool Trimesh<M,V,E,P>::poly_is_needle(const uint pid, const double angle_thresh_deg) const
{
    for(uint offset=0; offset<this->verts_per_poly(pid); ++offset)
    {
        uint vid = this->poly_vert_id(pid,offset);
        if (this->poly_angle_at_vert(pid,vid,DEG) < angle_thresh_deg)
        {
            return true;
        }
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::poly_add(const uint vid0, const uint vid1, const uint vid2)
{
    assert(vid0 < this->num_verts());
    assert(vid1 < this->num_verts());
    assert(vid2 < this->num_verts());

    uint pid = this->num_polys();
    //
    std::vector<uint> f;
    f.push_back(vid0);
    f.push_back(vid1);
    f.push_back(vid2);
    this->polys.push_back(f);
    //
    P data;
    this->p_data.push_back(data);
    //
    this->p2e.push_back(std::vector<uint>());
    this->p2p.push_back(std::vector<uint>());
    //

    this->v2p.at(vid0).push_back(pid);
    this->v2p.at(vid1).push_back(pid);
    this->v2p.at(vid2).push_back(pid);
    //
    ipair new_e[3]   = { unique_pair(vid0, vid1), unique_pair(vid1, vid2), unique_pair(vid2, vid0) };
    int   new_eid[3] = { -1, -1, -1 };
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        ipair e = unique_pair(this->edge_vert_id(eid,0), this->edge_vert_id(eid,1));
        for(uint i=0; i<3; ++i) if (e == new_e[i]) new_eid[i] = eid;
    }
    //
    for(uint i=0; i<3; ++i)
    {
        if (new_eid[i] == -1)
        {
            new_eid[i] = edge_add(new_e[i].first, new_e[i].second);
        }
        //
        for(uint nbr : this->e2p.at(new_eid[i]))
        {
            this->p2p.at(nbr).push_back(pid);
            this->p2p.at(pid).push_back(nbr);
        }
        this->e2p.at(new_eid[i]).push_back(pid);
        this->p2e.at(pid).push_back(new_eid[i]);
    }
    //
    this->update_p_normal(pid);
    this->update_v_normal(vid0);
    this->update_v_normal(vid1);
    this->update_v_normal(vid2);

    return pid;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::poly_set(const uint pid, const uint vid0, const uint vid1, const uint vid2)
{
    /* WARNING!!!! This completely screws up edge connectivity!!!!!! */

    assert(vid0 < this->num_verts());
    assert(vid1 < this->num_verts());
    assert(vid2 < this->num_verts());

    this->polys.at(pid).clear();
    this->polys.at(pid).push_back(vid0);
    this->polys.at(pid).push_back(vid1);
    this->polys.at(pid).push_back(vid2);

    this->update_p_normal(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double Trimesh<M,V,E,P>::poly_area(const uint pid) const
{
    vec3d p = this->poly_vert(pid,0);
    vec3d u = this->poly_vert(pid,1) - p;
    vec3d v = this->poly_vert(pid,2) - p;
    double area = 0.5 * u.cross(v).length();
    return area;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::poly_switch_id(const uint pid0, const uint pid1)
{
    for(uint offset=0; offset<3; ++offset)
    {
        std::swap(this->polys.at(3*pid0+offset),this->polys.at(3*pid1+offset));
    }
    std::swap(this->f_data.at(pid0), this->f_data.at(pid1));
    std::swap(this->p2e.at(pid0),    this->p2e.at(pid1));
    std::swap(this->p2p.at(pid0),    this->p2p.at(pid1));

    for(auto & nbrs : this->v2p)
    for(uint & curr : nbrs)
    {
        if (curr == pid0) curr = pid1; else
        if (curr == pid1) curr = pid0;
    }

    for(auto & nbrs : this->e2p)
    for(uint & curr : nbrs)
    {
        if (curr == pid0) curr = pid1; else
        if (curr == pid1) curr = pid0;
    }

    for(auto & nbrs : this->p2p)
    for(uint & curr : nbrs)
    {
        if (curr == pid0) curr = pid1; else
        if (curr == pid1) curr = pid0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::poly_remove_unreferenced(const uint pid)
{
    poly_switch_id(pid, this->num_polys()-1);
    this->polys.resize(this->polys.size()-3);
    this->f_data.pop_back();
    this->p2e.pop_back();
    this->p2p.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::vert_opposite_to(const uint pid, const uint vid0, const uint vid1) const
{
    assert(this->poly_contains_vert(pid, vid0));
    assert(this->poly_contains_vert(pid, vid1));
    for(uint off=0; off<this->verts_per_poly(pid); ++off)
    {
        uint vid = this->poly_vert_id(pid,off);
        if (vid != vid0 && vid != vid1) return vid;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Trimesh<M,V,E,P>::vert_add(const vec3d & pos)
{
    uint vid = this->num_verts();
    //
    this->verts.push_back(pos);
    //
    V data;
    this->v_data.push_back(data);
    //
    this->v2v.push_back(std::vector<uint>());
    this->v2e.push_back(std::vector<uint>());
    this->v2p.push_back(std::vector<uint>());
    //
    this->bb.min = this->bb.min.min(pos);
    this->bb.max = this->bb.max.max(pos);
    //
    return vid;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::vert_weights(const uint vid, const int type, std::vector<std::pair<uint,double>> & wgts) const
{
    switch (type)
    {
        case UNIFORM   : this->vert_weights_uniform(vid, wgts); return;
        case COTANGENT : vert_weights_cotangent(vid, wgts); return;
        default        : assert(false && "Vert weights not supported at this level of the hierarchy!");
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::vert_weights_cotangent(const uint vid, std::vector<std::pair<uint,double>> & wgts) const
{
    wgts.clear();
    for(uint eid : this->adj_v2e(vid))
    {
        assert(this->edge_is_manifold(eid));
        uint   nbr = this->vert_opposite_to(eid, vid);
        double wgt = 0.0;
        for(uint pid : this->adj_e2p(eid))
        {
            double alpha = this->poly_angle_at_vert(pid, vert_opposite_to(pid, vid, nbr));
            wgt += cot(alpha);
        }
        wgt = (this->adj_e2p(eid).size() == 2) ? wgt * 0.5 : wgt;
        wgts.push_back(std::make_pair(nbr,wgt));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::vert_switch_id(const uint vid0, const uint vid1)
{
    std::swap(this->verts.at(vid0),this->verts.at(vid1));
    std::swap(this->v2v.at(vid0),this->v2v.at(vid1));
    std::swap(this->v2e.at(vid0),this->v2e.at(vid1));
    std::swap(this->v2p.at(vid0),this->v2p.at(vid1));

    for(uint & curr : this->edges)
    {
        if (curr == vid0) curr = vid1; else
        if (curr == vid1) curr = vid0;
    }

    for(uint & curr : this->polys)
    {
        if (curr == vid0) curr = vid1; else
        if (curr == vid1) curr = vid0;
    }

    for(auto & nbrs : this->v2v)
    for(uint & curr : nbrs)
    {
        if (curr == vid0) curr = vid1; else
        if (curr == vid1) curr = vid0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Trimesh<M,V,E,P>::vert_remove_unreferenced(const uint vid)
{
    vert_switch_id(vid, this->num_verts()-1);
    this->verts.pop_back();
    this->v2v.pop_back();
    this->v2e.pop_back();
    this->v2p.pop_back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,P>::get_ordered_boundary_vertices() const
{
    // NOTE: assumes the mesh contains exactly ONE simply connected boundary!

    std::vector<uint>  faces;
    std::vector<vec3d> verts = this->vector_verts();
    verts.push_back(this->centroid());

    uint pid = this->num_verts();
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (this->edge_is_boundary(eid))
        {
            uint pid  = this->adj_e2p(eid).front();
            uint vid0 = this->edge_vert_id(eid,0);
            uint vid1 = this->edge_vert_id(eid,1);
            if (this->poly_vert_offset(pid,vid0) > this->poly_vert_offset(pid,vid1))
            {
                std::swap(vid0,vid1);
            }
            faces.push_back(vid0);
            faces.push_back(vid1);
            faces.push_back(pid);
        }
    }

    Trimesh<> tmp(verts,faces);
    return tmp.vert_ordered_vert_ring(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Trimesh<M,V,E,P>::poly_tessellation(const uint pid) const
{
    return this->polys.at(pid);
}

}
