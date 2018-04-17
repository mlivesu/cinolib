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
#include <cinolib/meshes/quadmesh.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/geometry/polygon.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/stl_container_utilities.h>
#include <cinolib/vector_serialization.h>
#include <queue>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const char * filename)
{
    this->load(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const std::vector<vec3d> & verts,
                            const std::vector<uint>  & polys)
{
    this->init(verts, polys_from_serialized_vids(polys,4));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & polys)
{
    this->init(vec3d_from_serialized_xyz(coords), polys_from_serialized_vids(polys,4));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & polys)
{
    this->init(verts, polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Quadmesh<M,V,E,P>::Quadmesh(const std::vector<double>            & coords,
                            const std::vector<std::vector<uint>> & polys)
{
    this->init(vec3d_from_serialized_xyz(coords), polys);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Quadmesh<M,V,E,P>::vert_is_singular(const uint vid) const
{
    if (this->vert_is_boundary(vid))
    {
        return (this->vert_valence(vid)!=3);
    }
    return (this->vert_valence(vid)!=4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool Quadmesh<M,V,E,P>::vert_is_regular(const uint vid) const
{    
    return (!this->vert_is_singular(vid));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int Quadmesh<M,V,E,P>::vert_next_along_chain(const uint curr, const uint prev) const
{
    if(vert_is_singular(curr)) return -1; // walking through a singular vertex is ambiguous...

    int e1 = this->edge_id(curr, prev);
    assert(e1>=0);

    for(uint e2 : this->adj_v2e(curr))
    {
        if (!this->edges_share_poly(e1,e2)) return this->vert_opposite_to(e2,curr);
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,P>::vert_chain(const uint curr, const uint prev) const
{
    assert(this->verts_are_adjacent(curr,prev));

    std::vector<uint> chain = { prev, curr };
    int vid;
    do
    {
        if ((vid = vert_next_along_chain(curr, prev)) >= 0)
        {
            chain.push_back(vid);
        }
        else return chain; // ended up in a singular vertex
    }
    while (vid != (int)chain.front()); // if loop, terminate
    return chain;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
int Quadmesh<M,V,E,P>::edge_next_along_chain(const uint eid, const uint vid) const
{
    if(vert_is_singular(vid)) return -1; // walking through a singular vertex is ambiguous...

    for(uint nbr : this->adj_v2e(vid))
    {
        if (!this->edges_share_poly(eid,nbr)) return nbr;
    }
    assert(false);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,P>::edge_chain(const uint eid, const uint vid) const
{
    std::vector<uint> chain = { eid };
    int curr_vid = vid;
    int curr_eid = eid;
    do
    {
        if ((curr_eid = edge_next_along_chain(curr_eid, curr_vid)) >= 0)
        {
            chain.push_back(curr_eid);
            curr_vid = this->vert_opposite_to(curr_eid, curr_vid);
        }
        else return chain; // ended up in a singular vertex
    }
    while (curr_eid != (int)chain.front()); // if loop, terminate
    return chain;
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
std::vector<uint> Quadmesh<M,V,E,P>::edge_parallel_chain(const uint eid) const
{
    // NOTE: this is a chain of PARALLEL edges

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
std::vector<std::vector<uint>> Quadmesh<M,V,E,P>::edge_parallel_chains() const
{
    // NOTE: these are chains of PARALLEL edges

    std::set<uint> visited;
    std::vector<std::vector<uint>> chains;
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (DOES_NOT_CONTAIN(visited,eid))
        {
            std::vector<uint> chain = edge_parallel_chain(eid);
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
    // NOTE #1: assumes the mesh contains exactly ONE simply connected boundary!
    // NOTE #2: although this method is duplicated both in Quadmesh and Polygonmesh,
    //          I cannot move it up to the AbstractPolygonMesh level because it uses
    //          a method of trimesh :(

    std::vector<uint>  polys;
    std::vector<vec3d> verts = this->vector_verts();
    verts.push_back(this->centroid());

    uint cid = this->num_verts();
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if (this->edge_is_boundary(eid))
        {
            uint pid  = this->adj_e2p(eid).front();
            uint vid0 = this->edge_vert_id(eid,0);
            uint vid1 = this->edge_vert_id(eid,1);
            if (!this->poly_verts_are_CCW(pid, vid1, vid0)) std::swap(vid0,vid1);
            polys.push_back(vid0);
            polys.push_back(vid1);
            polys.push_back(cid);
        }
    }

    Trimesh<> tmp(verts,polys);
    return tmp.vert_ordered_vert_ring(cid);
}
}
