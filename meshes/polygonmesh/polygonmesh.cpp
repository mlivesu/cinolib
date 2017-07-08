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
    this->faces = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Polygonmesh<M,V,E,F>::Polygonmesh(const std::vector<double>            & coords,
                                  const std::vector<std::vector<uint>> & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->faces = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::clear()
{
    this->bb.reset();
    //
    this->verts.clear();
    this->edges.clear();
    this->faces.clear();
    //
    M std_M_data;
    this->m_data = std_M_data;
    this->v_data.clear();
    this->e_data.clear();
    this->f_data.clear();
    //
    this->v2v.clear();
    this->v2e.clear();
    this->v2f.clear();
    this->e2f.clear();
    this->f2e.clear();
    this->f2f.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::init()
{
    update_face_tessellation();

    this->update_adjacency();
    this->update_bbox();

    this->v_data.resize(this->num_verts());
    this->e_data.resize(this->num_edges());
    this->f_data.resize(this->num_faces());

    this->update_normals();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::update_face_tessellation()
{
    tessellated_faces.resize(this->num_faces());
    std::set<uint> bad_faces;

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        // TODO: improve triangulation strategy (this assumes convexity!)
        std::vector<vec3d> n;
        for (uint i=2; i<this->verts_per_face(fid); ++i)
        {
            uint vid0 = this->faces.at(fid).at( 0 );
            uint vid1 = this->faces.at(fid).at(i-1);
            uint vid2 = this->faces.at(fid).at( i );

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
double Polygonmesh<M,V,E,F>::face_area(const uint /*fid*/) const
{
    assert(false); // TODO!
    return -1.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::update_f_normal(const uint fid)
{
    // compute the best fitting plane
    std::vector<vec3d> points;
    for(uint off=0; off<this->verts_per_face(fid); ++off) points.push_back(this->face_vert(fid,off));
    Plane best_fit(points);

    // adjust orientation (n or -n?)
    vec3d v0 = this->face_vert(fid,0);
    vec3d v1 = this->face_vert(fid,1);
    uint  i=2;
    vec3d ccw;
    do { ccw = (v1-v0).cross(this->face_vert(fid,i)-v0);  ++i; } while (ccw.length_squared()==0 && i<this->verts_per_face(fid));

    this->face_data(fid).normal = (best_fit.n.dot(ccw) < 0) ? -best_fit.n : best_fit.n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
int Polygonmesh<M,V,E,F>::face_opposite_to(const uint eid, const uint fid) const
{
    assert(face_contains_edge(fid,eid));
    assert(this->edge_is_manifold(eid));
    assert(!this->adj_e2f(eid).empty());

    if (this->edge_is_boundary(eid)) return -1;
    if (this->adj_e2f(eid).front() != fid) return this->adj_e2f(eid).front();
    return this->adj_e2f(eid).back();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Polygonmesh<M,V,E,F>::face_contains_edge(const uint fid, const uint eid) const
{
    for(uint e : this->adj_f2e(fid)) if (e == eid) return true;
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Polygonmesh<M,V,E,F>::elem_centroid(const uint fid) const
{
    return this->face_centroid(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Polygonmesh<M,V,E,F>::elem_show_all()
{
    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        this->face_data(fid).visible = true;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Polygonmesh<M,V,E,F>::elem_vert_id(const uint fid, const uint offset) const
{
    return this->face_vert_id(fid,offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
vec3d Polygonmesh<M,V,E,F>::elem_vert(const uint fid, const uint offset) const
{
    return this->face_vert(fid,offset);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
uint Polygonmesh<M,V,E,F>::elem_vert_offset(const uint fid, const uint vid) const
{
    return this->face_vert_offset(fid,vid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
double Polygonmesh<M,V,E,F>::elem_mass(const uint fid) const
{
    return this->face_area(fid);
}

}
