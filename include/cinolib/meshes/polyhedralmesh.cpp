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
#include <cinolib/bbox.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/vector_serialization.h>

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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Polyhedralmesh<M,V,E,F,P>::Polyhedralmesh(const std::vector<vec3d>             & verts,
                                          const std::vector<std::vector<uint>> & faces,
                                          const std::vector<std::vector<uint>> & polys,
                                          const std::vector<std::vector<bool>> & polys_face_winding)
{
    this->init(verts, faces, polys, polys_face_winding);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::load(const char * filename)
{
    this->clear();
    this->mesh_data().filename = std::string(filename);

    std::vector<vec3d>             tmp_verts;
    std::vector<std::vector<uint>> tmp_faces;
    std::vector<std::vector<uint>> tmp_polys;
    std::vector<std::vector<bool>> tmp_polys_face_winding;

    std::string str(filename);
    std::string filetype = str.substr(str.size()-6,6);

    if (filetype.compare("hybrid") == 0 ||
        filetype.compare("HYBRID") == 0)
    {
        read_HYBDRID(filename, tmp_verts, tmp_faces, tmp_polys, tmp_polys_face_winding);
    }
    else if (filetype.compare(".hedra") == 0 ||
             filetype.compare(".HEDRA") == 0)
    {
        read_HEDRA(filename, tmp_verts, tmp_faces, tmp_polys, tmp_polys_face_winding);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : file format not supported yet " << std::endl;
        exit(-1);
    }

    this->init(tmp_verts, tmp_faces, tmp_polys, tmp_polys_face_winding);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::save(const char * filename) const
{
    std::string str(filename);
    std::string filetype = str.substr(str.size()-6,6);

    if (filetype.compare(".hedra") == 0 ||
        filetype.compare(".HEDRA") == 0)
    {
        write_HEDRA(filename, this->verts, this->faces, this->polys, this->polys_face_winding);
    }
    else
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : write() : file format not supported yet " << std::endl;
        exit(-1);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void Polyhedralmesh<M,V,E,F,P>::update_f_normal(const uint fid)
{
    assert(this->verts_per_face(fid)>2);
    std::vector<vec3d> points;
    for(uint off=0; off<this->verts_per_face(fid); ++off) points.push_back(this->face_vert(fid,off));
    this->face_data(fid).normal = polygon_normal(points);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_hexahedron(const uint pid) const
{
    if(this->verts_per_poly(pid)!=8) return false;
    if(this->faces_per_poly(pid)!=6) return false;
    for(uint fid : this->adj_p2f(pid))
    {
        if(!this->face_is_quad(fid)) return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_prism(const uint pid) const
{
    // test all possible bases for a prism
    //
    for(uint fid : this->adj_p2f(pid))
    {
        if(this->poly_is_prism(pid,fid)) return true;
    }
    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
bool Polyhedralmesh<M,V,E,F,P>::poly_is_prism(const uint pid, const uint fid) const
{
    assert(this->poly_contains_face(pid,fid));

    // a prism base has one incident quad per edge and
    // an a twin n-gon at the opposite base

    int nf = this->faces_per_poly(pid);

    if((int)this->edges_per_face(fid)!=nf-2) return false;

    std::set<uint> f_visited;
    f_visited.insert(fid);
    for(uint eid : this->adj_f2e(fid))
    {
        uint nbr = this->face_adj_through_edge(fid, eid, pid);
        if (!this->face_is_quad(nbr)) return false;
        f_visited.insert(nbr);
    }
    //assert((int)f_visited.size()==nf-1);

    for(uint nbr : this->adj_p2f(pid))
    {
        if(CONTAINS(f_visited,nbr)) continue;
        if(this->verts_per_face(nbr)==this->verts_per_face(fid)) return true;
    }
    return false;
}

}
