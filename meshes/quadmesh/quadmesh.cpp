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
#include <cinolib/geometry/plane.h>

#include <queue>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Quadmesh<M,V,E,F>::Quadmesh(const char * filename)
{
    this->load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Quadmesh<M,V,E,F>::Quadmesh(const std::vector<vec3d> & verts,
                            const std::vector<uint>  & faces)
{
    this->verts = verts;
    this->faces = faces_from_serialized_vids(faces,4);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Quadmesh<M,V,E,F>::Quadmesh(const std::vector<double> & coords,
                            const std::vector<uint>   & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->faces = faces_from_serialized_vids(faces,4);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Quadmesh<M,V,E,F>::Quadmesh(const std::vector<vec3d>             & verts,
                            const std::vector<std::vector<uint>> & faces)
{
    this->verts = verts;
    this->faces = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
Quadmesh<M,V,E,F>::Quadmesh(const std::vector<double>            & coords,
                            const std::vector<std::vector<uint>> & faces)
{
    this->verts = vec3d_from_serialized_xyz(coords);
    this->faces = faces;
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::clear()
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
void Quadmesh<M,V,E,F>::init()
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
void Quadmesh<M,V,E,F>::update_face_tessellation()
{
    tessellated_faces.resize(this->num_faces());

    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        uint vid0 = this->face_vert_id(fid,0);
        uint vid1 = this->face_vert_id(fid,1);
        uint vid2 = this->face_vert_id(fid,2);
        uint vid3 = this->face_vert_id(fid,3);

        vec3d n1 = (this->vert(vid1)-this->vert(vid0)).cross(this->vert(vid2)-this->vert(vid0));
        vec3d n2 = (this->vert(vid2)-this->vert(vid0)).cross(this->vert(vid3)-this->vert(vid0));

        bool flip = (n1.dot(n2) < 0); // flip diag: t(0,1,2) t(0,2,3) => t(0,1,3) t(1,2,3)

        tessellated_faces.at(fid).push_back(vid0);
        tessellated_faces.at(fid).push_back(vid1);
        tessellated_faces.at(fid).push_back(flip ? vid3 : vid2);
        tessellated_faces.at(fid).push_back(flip ? vid1 : vid0);
        tessellated_faces.at(fid).push_back(vid2);
        tessellated_faces.at(fid).push_back(vid3);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
void Quadmesh<M,V,E,F>::update_f_normal(const uint fid)
{
    // compute the best fitting plane
    std::vector<vec3d> points;
    for(uint off=0; off<this->verts_per_face(fid); ++off) points.push_back(this->face_vert(fid,off));
    Plane best_fit(points);

    // adjust orientation (n or -n?)
    assert(tessellated_faces.at(fid).size()>2);
    vec3d v0 = this->vert(tessellated_faces.at(fid).at(0));
    vec3d v1 = this->vert(tessellated_faces.at(fid).at(1));
    vec3d v2 = this->vert(tessellated_faces.at(fid).at(2));
    vec3d n  = (v1-v0).cross(v2-v0);

    this->face_data(fid).normal = (best_fit.n.dot(n) < 0) ? -best_fit.n : best_fit.n;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::vert_is_singular(const uint vid) const
{
    return (this->vert_valence(vid)!=4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
bool Quadmesh<M,V,E,F>::vert_is_regular(const uint vid) const
{
    return (this->vert_valence(vid)==4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F>
CINO_INLINE
std::vector<uint> Quadmesh<M,V,E,F>::vert_loop(const uint start, const uint next) const
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

template<class M, class V, class E, class F>
CINO_INLINE
double Quadmesh<M,V,E,F>::face_area(const uint /*fid*/) const
{
    assert(false); // TODO!
    return -1.0;
}

}
