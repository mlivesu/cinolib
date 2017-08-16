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
#include <cinolib/meshes/polygonmesh.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/mesh_attributes.h>
#include <cinolib/meshes/abstract_polyhedralmesh.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class F = Polygon_std_attributes,
         class P = Polyhedron_std_attributes>
class Polyhedralmesh : public AbstractPolyhedralMesh<M,V,E,F,P>
{
    protected:

        std::vector<std::vector<uint>> triangulated_faces; // triangles covering each face (e.g., for rendering)

    public:

        Polyhedralmesh(){}

        Polyhedralmesh(const char * filename);

        Polyhedralmesh(const std::vector<vec3d>             & verts,
                       const std::vector<std::vector<uint>> & faces,
                       const std::vector<std::vector<uint>> & polys,
                       const std::vector<std::vector<bool>> & polys_face_winding);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const { return POLYHEDRALMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init();
        void clear();
        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_face_tessellation();
        void update_normals();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<uint> face_tessellation(const uint fid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double poly_volume(const uint) const { assert(false && "TODO!"); return 1.0; }
};

}

#ifndef  CINO_STATIC_LIB
#include "polyhedralmesh.cpp"
#endif

#endif // CINO_POLYHEDRALMESH_H
