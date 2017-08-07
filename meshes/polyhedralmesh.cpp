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
#include <cinolib/meshes/polyhedralmesh.h>
#include <cinolib/io/read_write.h>
#include <cinolib/common.h>

#include <algorithm>
#include <cmath>
#include <float.h>
#include <map>
#include <set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Polyhedralmesh<M,V,E,F,P>::Polyhedralmesh(const char * filename)
{
    load(filename);
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Polyhedralmesh<M,V,E,F,P>::Polyhedralmesh(const std::vector<vec3d>             & verts,
                                          const std::vector<std::vector<uint>> & faces,
                                          const std::vector<std::vector<uint>> & polys,
                                          const std::vector<std::vector<bool>> & polys_face_winding)
{
    this->verts = verts;
    this->faces = faces;
    this->polys = polys;
    this->polys_face_winding = polys_face_winding;

    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::load(const char * filename)
{
    this->clear();
    std::vector<double> coords;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-7,7);

    if (filetype.compare(".hybrid") == 0 ||
        filetype.compare(".HYBRID") == 0)
    {
        read_HYBDRID(filename, coords, this->faces, this->polys, this->polys_face_winding);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << endl;
        exit(-1);
    }

    this->verts = vec3d_from_serialized_xyz(coords);

    logger << this->num_verts() << " verts read" << endl;
    logger << this->num_faces() << " faces read" << endl;
    logger << this->num_polys() << " polys read" << endl;

    this->mesh_data().filename = std::string(filename);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::save(const char *) const
{
    assert(false && "TODO!");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::init()
{
    update_face_tessellation();
    AbstractPolyhedralMesh<M,V,E,F,P>::init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::clear()
{
    AbstractPolyhedralMesh<M,V,E,F,P>::clear();
    triangulated_faces.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::update_face_tessellation()
{
    triangulated_faces.resize(this->num_faces());
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

            triangulated_faces.at(fid).push_back(vid0);
            triangulated_faces.at(fid).push_back(vid1);
            triangulated_faces.at(fid).push_back(vid2);

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

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::update_normals()
{
    for(uint fid=0; fid<this->num_faces(); ++fid)
    {
        assert(this->verts_per_face(fid)>2);

        // compute the best fitting plane
        std::vector<vec3d> points;
        for(uint off=0; off<this->verts_per_face(fid); ++off) points.push_back(this->face_vert(fid,off));
        Plane best_fit(points);

        // adjust orientation (n or -n?)
        vec3d v0 = this->face_vert(fid,0);
        vec3d v1 = this->face_vert(fid,1);
        uint  i=2;
        vec3d ccw;
        do { ccw = (v1-v0).cross(this->face_vert(fid,i)-v0); ++i; } while (ccw.length_squared()==0 && i<this->verts_per_face(fid));

        this->face_data(fid).normal = (best_fit.n.dot(ccw) < 0) ? -best_fit.n : best_fit.n;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
std::vector<uint> Polyhedralmesh<M,V,E,F,P>::face_tessellation(const uint fid) const
{
    return triangulated_faces.at(fid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::export_to_hexahedral_mesh(std::vector<vec3d>             & verts,
                                                          std::vector<std::vector<uint>> & hexa) const
{
    assert(false);
    verts = this->vector_verts();
    hexa  = this->export_hex_connectivity();
}

}
