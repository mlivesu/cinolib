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
#include <cinolib/meshes/polygonmesh.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/geometry/polygon.h>
#include <cinolib/vector_serialization.h>
#include <numeric>
#include <map>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Polygonmesh<M,V,E,P>::Polygonmesh(const char * filename)
{
    this->load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Polygonmesh<M,V,E,P>::Polygonmesh(const std::vector<vec3d> & verts)
{
    this->verts = verts;

    std::vector<uint> p(verts.size());
    std::iota(p.begin(), p.end(), 0);
    this->polys.push_back(p);

    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Polygonmesh<M,V,E,P>::Polygonmesh(const std::vector<vec3d>             & verts,
                                  const std::vector<std::vector<uint>> & faces)
{
    this->verts = verts;
    this->polys = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Polygonmesh<M,V,E,P>::Polygonmesh(const std::vector<double>            & coords,
                                  const std::vector<std::vector<uint>> & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->polys = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::clear()
{
    AbstractMesh<M,V,E,P>::clear();
    triangulated_polys.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::init()
{
    update_poly_tessellation();
    AbstractPolygonMesh<M,V,E,P>::init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::update_poly_tessellation()
{
    triangulated_polys.clear();
    triangulated_polys.resize(this->num_polys());

    for(uint pid=0; pid<this->num_polys(); ++pid)
    {
        update_poly_tessellation(pid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::update_poly_tessellation(const uint pid)
{
    triangulated_polys.at(pid).clear();

    std::vector<vec3d> poly3d = this->poly_vlist(pid);
    std::vector<vec2d> poly2d;
    polygon_flatten(poly3d, poly2d);

    if (!polygon_is_convex(poly2d))
    {
        // Apply ear cut algorithm
        //
        // NOTE: the triangulation is constructed on a proxy polygon obtained
        // projecting the actual polygon onto the best fitting plane. Bad things
        // can still happen for highly non-planar polygons
        //
        std::vector<uint> tris;
        if (polygon_triangulate(poly2d, tris))
        {
            for(uint off : tris) triangulated_polys.at(pid).push_back(this->poly_vert_id(pid,off));
            return;
        }
    }

    // iff polygon is convex (or ear cut failed), apply trivial triangulation
    //
    for (uint i=2; i<this->verts_per_poly(pid); ++i)
    {
        uint vid0 = this->polys.at(pid).at( 0 );
        uint vid1 = this->polys.at(pid).at(i-1);
        uint vid2 = this->polys.at(pid).at( i );

        triangulated_polys.at(pid).push_back(vid0);
        triangulated_polys.at(pid).push_back(vid1);
        triangulated_polys.at(pid).push_back(vid2);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::update_p_normal(const uint pid)
{
    // compute the best fitting plane
    std::vector<vec3d> points;
    for(uint off=0; off<this->verts_per_poly(pid); ++off) points.push_back(this->poly_vert(pid,off));
    this->poly_data(pid).normal = polygon_normal(points);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<uint> Polygonmesh<M,V,E,P>::get_ordered_boundary_vertices() const
{
    // NOTE: assumes the mesh contains exactly ONE simply connected boundary!

    std::vector<double> coords = this->vector_coords();
    std::vector<uint>   tris;

    vec3d c   = this->centroid();
    uint  pid = this->num_verts();
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
            tris.push_back(pid);
        }
    }

    Trimesh<M,V,E,P> tmp(coords,tris);
    return tmp.vert_ordered_vert_ring(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::vert_switch_id(const uint vid0, const uint vid1)
{
    AbstractPolygonMesh<M,V,E,P>::vert_switch_id(vid0, vid1);

    std::unordered_set<uint> polys_to_update;
    polys_to_update.insert(this->adj_v2p(vid0).begin(), this->adj_v2p(vid0).end());
    polys_to_update.insert(this->adj_v2p(vid1).begin(), this->adj_v2p(vid1).end());

    for(uint pid : polys_to_update)
    {
        for(uint & vid : this->triangulated_polys.at(pid))
        {
            if (vid == vid0) vid = vid1; else
            if (vid == vid1) vid = vid0;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::operator+=(const Polygonmesh<M,V,E,P> & m)
{
    AbstractPolygonMesh<M,V,E,P>::operator +=(m);
    update_poly_tessellation();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
const std::vector<uint> & Polygonmesh<M,V,E,P>::poly_tessellation(const uint pid) const
{
    return triangulated_polys.at(pid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint Polygonmesh<M,V,E,P>::poly_add(const std::vector<uint> & p)
{
    uint new_id = AbstractPolygonMesh<M,V,E,P>::poly_add(p);
    triangulated_polys.push_back(std::vector<uint>(0));
    update_poly_tessellation(new_id);
    return new_id;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::poly_switch_id(const uint pid0, const uint pid1)
{
    AbstractPolygonMesh<M,V,E,P>::poly_switch_id(pid0, pid1);
    std::swap(triangulated_polys.at(pid0), triangulated_polys.at(pid1));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void Polygonmesh<M,V,E,P>::poly_remove_unreferenced(const uint pid)
{
    AbstractPolygonMesh<M,V,E,P>::poly_remove_unreferenced(pid);
    triangulated_polys.pop_back();
}

}
