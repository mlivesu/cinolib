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
#include <cinolib/meshes/quadmesh/quadmesh.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/geometry/plane.h>

#include <queue>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const char * filename) : AbstractPolygonMesh<M,V,E,P>()
{
    this->load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const std::vector<vec3d> & verts,
                            const std::vector<uint>  & faces) : AbstractPolygonMesh<M,V,E,P>()
{
    this->verts = verts;
    this->polys = faces_from_serialized_vids(faces,4);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & faces) : AbstractPolygonMesh<M,V,E,P>()
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->polys = faces_from_serialized_vids(faces,4);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & faces) : AbstractPolygonMesh<M,V,E,P>()
{
    this->verts = verts;
    this->polys = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const std::vector<double>            & coords,
                            const std::vector<std::vector<uint>> & faces) : AbstractPolygonMesh<M,V,E,P>()
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->polys = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Quadmesh<M,V,E,P>::clear()
{
    AbstractMesh<M,V,E,P>::clear();
    triangulated_polys.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Quadmesh<M,V,E,P>::init()
{
    update_poly_tessellation();
    AbstractPolygonMesh<M,V,E,P>::init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Quadmesh<M,V,E,P>::update_poly_tessellation()
{
    triangulated_polys.clear();
    triangulated_polys.resize(this->num_polys());

    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        uint vid0 = this->poly_vert_id(pid,0);
        uint vid1 = this->poly_vert_id(pid,1);
        uint vid2 = this->poly_vert_id(pid,2);
        uint vid3 = this->poly_vert_id(pid,3);

        vec3d n1 = (this->vert(vid1)-this->vert(vid0)).cross(this->vert(vid2)-this->vert(vid0));
        vec3d n2 = (this->vert(vid2)-this->vert(vid0)).cross(this->vert(vid3)-this->vert(vid0));

        bool flip = (n1.dot(n2) < 0); // flip diag: t(0,1,2) t(0,2,3) => t(0,1,3) t(1,2,3)

        triangulated_polys.at(pid).push_back(vid0);
        triangulated_polys.at(pid).push_back(vid1);
        triangulated_polys.at(pid).push_back(flip ? vid3 : vid2);
        triangulated_polys.at(pid).push_back(flip ? vid1 : vid0);
        triangulated_polys.at(pid).push_back(vid2);
        triangulated_polys.at(pid).push_back(vid3);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Quadmesh<M,V,E,P>::update_p_normal(const uint pid)
{
    // compute the best fitting plane
    std::vector<vec3d> points;
    for(uint off=0; off<this->verts_per_poly(pid); ++off) points.push_back(this->poly_vert(pid,off));
    Plane best_fit(points);

    // adjust orientation (n or -n?)
    assert(triangulated_polys.at(pid).size()>2);
    vec3d v0 = this->vert(triangulated_polys.at(pid).at(0));
    vec3d v1 = this->vert(triangulated_polys.at(pid).at(1));
    vec3d v2 = this->vert(triangulated_polys.at(pid).at(2));
    vec3d n  = (v1-v0).cross(v2-v0);

    this->poly_data(pid).normal = (best_fit.n.dot(n) < 0) ? -best_fit.n : best_fit.n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Quadmesh<M,V,E,P>::vert_is_singular(const uint vid) const
{
    return (this->vert_valence(vid)!=4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Quadmesh<M,V,E,P>::vert_is_regular(const uint vid) const
{
    return (this->vert_valence(vid)==4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,P>::vert_chain(const uint start, const uint next) const
{
    assert(this->verts_are_adjacent(start,next));
    assert(this->vert_is_regular(start)); // if there is a singularity along there will be no loop!

    uint curr = next;
    uint prev = start;

    std::vector<uint> loop;
    loop.push_back(prev);

    do
    {
        assert(this->vert_is_regular(curr));
        loop.push_back(curr);

        std::vector<uint> v_ring = this->vert_ordered_vert_ring(curr);
        auto it  = std::find(v_ring.begin(), v_ring.end(), prev); assert(it != v_ring.end());
        uint pos = std::distance(v_ring.begin(),it);

        prev = curr;
        curr = v_ring.at((pos+4)%v_ring.size());

        assert(loop.size() < this->num_verts()); // sanity check for inifnite loops...
    }
    while(curr != start);

    return loop;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,P>::edges_opposite_to(const uint eid) const
{
    std::vector<uint> res;
    for(uint pid : this->adj_e2p(eid))
    {
        res.push_back(edge_opposite_to(pid,eid));
    }
    return res;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Quadmesh<M,V,E,P>::edge_opposite_to(const uint pid, const uint eid) const
{
    assert(this->poly_contains_edge(pid,eid));

    uint vid0 = this->edge_vert_id(eid,0);
    uint vid1 = this->edge_vert_id(eid,1);

    for(uint e : this->adj_p2e(pid))
    {
        if (!this->edge_contains_vert(e,vid0) &&
            !this->edge_contains_vert(e,vid1))
        {
            return e;
        }
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,P>::edge_chain(const uint eid) const
{
    std::vector<uint> chain;
    std::set<uint>    visited;
    std::queue<uint>  q;
    q.push(eid);
    visited.insert(eid);

    while(!q.empty())
    {
        uint curr = q.front();
        q.pop();

        chain.push_back(curr);

        for(uint e : edges_opposite_to(curr))
        {
            if (DOES_NOT_CONTAIN(visited,e))
            {
                visited.insert(e);
                q.push(e);
            }
        }
    }
    assert(visited.size() == chain.size());

    return chain;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<std::vector<uint>> Quadmesh<M,V,E,P>::edge_chains() const
{
    std::set<uint> visited;
    std::vector<std::vector<uint>> chains;
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (DOES_NOT_CONTAIN(visited,eid))
        {
            std::vector<uint> chain = edge_chain(eid);
            for(uint e : chain) visited.insert(e);
            chains.push_back(chain);
        }
    }
    assert(visited.size()==this->num_edges());
    return chains;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,P>::get_ordered_boundary_vertices() const
{
    // NOTE: assumes the mesh contains exactly ONE simply connected boundary!

    std::vector<double> coords = this->vector_coords();
    std::vector<uint>   tris;

    vec3d c   = this->centroid();
    uint  cid = this->num_verts();
    coords.push_back(c.x());
    coords.push_back(c.y());
    coords.push_back(c.z());

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
            tris.push_back(vid0);
            tris.push_back(vid1);
            tris.push_back(cid);
        }
    }

    logger.disable();
    Trimesh<> tmp(coords,tris);
    logger.enable();
    return tmp.vert_ordered_vert_ring(cid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Quadmesh<M,V,E,P>::operator+=(const Quadmesh<M,V,E,P> & m)
{
    AbstractPolygonMesh<M,V,E,P>::operator +=(m);
    update_poly_tessellation();
}

}
