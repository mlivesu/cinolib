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
#ifndef CINO_POLYHEDRALMESH_H
#define CINO_POLYHEDRALMESH_H

#include <sys/types.h>
#include <vector>

#include <cinolib/cinolib.h>
#include <cinolib/bbox.h>
#include <cinolib/meshes/polygonmesh/polygonmesh.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/mesh_std_data.h>
#include <cinolib/meshes/abstract_volume_mesh.h>

namespace cinolib
{

template<class M = Mesh_min_attributes, // default template arguments
         class V = Vert_min_attributes,
         class E = Edge_min_attributes,
         class F = Polygon_min_attributes,
         class P = Polyhedron_min_attributes>
class Polyhedralmesh : public AbstractPolyhedralMesh<M,V,E,F,P>
{
    public:

        Polyhedralmesh(){}

        Polyhedralmesh(const char * filename);

        Polyhedralmesh(const std::vector<vec3d>             & verts,
                       const std::vector<std::vector<uint>> & faces,
                       const std::vector<std::vector<uint>> & polys,
                       const std::vector<std::vector<bool>> & polys_face_winding);

    public:

        void init();
        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_adjacency();
        void update_face_tessellation();
        void update_f_normals();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool  edge_is_on_srf(const uint eid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d face_vert     (const uint fid, const uint off) const;
        uint  face_vert_id  (const uint fid, const uint off) const;
        bool  face_is_on_srf(const uint fid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool  poly_is_on_surf (const uint pid) const;
        uint  poly_face_id    (const uint pid, const uint off) const;
        bool  poly_face_is_CCW(const uint pid, const uint off) const;
        bool  poly_face_is_CW (const uint pid, const uint off) const;
        uint  poly_face_offset(const uint pid, const uint fid) const;
        vec3d poly_centroid   (const uint pid) const;
        void  poly_show_all();
};

}

#ifndef  CINO_STATIC_LIB
#include "polyhedralmesh.cpp"
#endif

#endif // CINO_POLYHEDRALMESH_H
