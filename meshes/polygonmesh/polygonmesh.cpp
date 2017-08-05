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
#include <cinolib/meshes/polygonmesh/polygonmesh.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/geometry/plane.h>
#include <map>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Polygonmesh<M,V,E,F>::Polygonmesh(const char * filename)
{
    this->load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Polygonmesh<M,V,E,F>::Polygonmesh(const std::vector<vec3d>             & verts,
                                  const std::vector<std::vector<uint>> & faces)
{
    this->verts = verts;
    this->polys = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Polygonmesh<M,V,E,F>::Polygonmesh(const std::vector<double>            & coords,
                                  const std::vector<std::vector<uint>> & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->polys = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::clear()
{
    AbstractMesh<M,V,E,F>::clear();
    tessellated_faces.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::init()
{
    update_poly_tessellation();
    AbstractPolygonMesh<M,V,E,F>::init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::update_poly_tessellation()
{
    tessellated_faces.resize(this->num_polys());
    std::set<uint> bad_faces;

    for(uint fid=0; fid<this->num_polys(); ++fid)
    {
        // TODO: improve triangulation strategy (this assumes convexity!)
        std::vector<vec3d> n;
        for (uint i=2; i<this->verts_per_poly(fid); ++i)
        {
            uint vid0 = this->polys.at(fid).at( 0 );
            uint vid1 = this->polys.at(fid).at(i-1);
            uint vid2 = this->polys.at(fid).at( i );

            tessellated_faces.at(fid).push_back(vid0);
            tessellated_faces.at(fid).push_back(vid1);
            tessellated_faces.at(fid).push_back(vid2);

            n.push_back((this->vert(vid1)-this->vert(vid0)).cross(this->vert(vid2)-this->vert(vid0)));
        }
        // check for badly tessellated polygons...
        for(uint i=0; i<n.size()-1; ++i) if (n.at(i).dot(n.at(i+1))<0) bad_faces.insert(fid);
    }
    //
    for(uint fid : bad_faces)
    {
        std::cerr << "WARNING : Bad tessellation occurred for non-convex polygon " << fid << std::endl;
    }

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::update_f_normal(const uint fid)
{
    // compute the best fitting plane
    std::vector<vec3d> points;
    for(uint off=0; off<this->verts_per_poly(fid); ++off) points.push_back(this->poly_vert(fid,off));
    Plane best_fit(points);

    // adjust orientation (n or -n?)
    vec3d v0 = this->poly_vert(fid,0);
    vec3d v1 = this->poly_vert(fid,1);
    uint  i=2;
    vec3d ccw;
    do { ccw = (v1-v0).cross(this->poly_vert(fid,i)-v0); ++i; } while (ccw.length_squared()==0 && i<this->verts_per_poly(fid));

    this->poly_data(fid).normal = (best_fit.n.dot(ccw) < 0) ? -best_fit.n : best_fit.n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Polygonmesh<M,V,E,F>::get_ordered_boundary_vertices() const
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
            uint fid  = this->adj_e2p(eid).front();
            uint vid0 = this->edge_vert_id(eid,0);
            uint vid1 = this->edge_vert_id(eid,1);
            if (this->poly_vert_offset(fid,vid0) > this->poly_vert_offset(fid,vid1))
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

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::operator+=(const Polygonmesh<M,V,E,F> & m)
{
    AbstractPolygonMesh<M,V,E,F>::operator +=(m);
    update_poly_tessellation();
}

}
