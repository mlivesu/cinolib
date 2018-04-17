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
#ifndef CINO_HEXMESH_H
#define CINO_HEXMESH_H

#include <sys/types.h>
#include <vector>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/quadmesh.h>
#include <cinolib/meshes/mesh_attributes.h>
#include <cinolib/meshes/abstract_polyhedralmesh.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class F = Polygon_std_attributes,
         class P = Polyhedron_std_attributes>
class Hexmesh : public AbstractPolyhedralMesh<M,V,E,F,P>
{
    public:

        explicit Hexmesh(){}

        explicit Hexmesh(const char * filename);

        explicit Hexmesh(const std::vector<double> & coords,
                         const std::vector<uint>   & polys);

        explicit Hexmesh(const std::vector<vec3d> & verts,
                         const std::vector<uint>  & polys);

        explicit Hexmesh(const std::vector<vec3d>             & verts,
                         const std::vector<std::vector<uint>> & polys);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const { return HEXMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init_hexmesh(const std::vector<vec3d>             & verts,
                          const std::vector<std::vector<uint>> & polys);
        void init_hexmesh(const std::vector<vec3d>             & verts,
                          const std::vector<std::vector<uint>> & polys,
                          const std::vector<int>               & vert_labels,
                          const std::vector<int>               & poly_labels);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_f_normal(const uint fid);
        void update_hex_quality(const uint pid);
        void update_hex_quality();
        void print_quality(const bool list_folded_elements = false);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_poly(const uint) const { return  8; }
        uint verts_per_poly()           const { return  8; }
        uint edges_per_poly(const uint) const { return 12; }
        uint edges_per_poly()           const { return 12; }
        uint faces_per_poly(const uint) const { return  6; }
        uint faces_per_poly()           const { return  6; }
        uint verts_per_face(const uint) const { return  4; }
        uint verts_per_face()           const { return  4; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool  vert_is_singular(const uint vid) const;
        bool  vert_is_regular (const uint vid) const;
        vec3d verts_average   (const std::vector<uint> & vids) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool  edge_is_singular(const uint eid) const;
        bool  edge_is_regular (const uint eid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<uint> face_sheet(const uint fid) const; // stop at singular edges

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint   poly_face_opposite_to(const uint pid, const uint fid) const;
        void   poly_subdivide       (const std::vector<std::vector<std::vector<uint>>> & split_scheme);
        double poly_volume          (const uint pid) const;
        uint   poly_add             (const std::vector<uint> & vlist); // vertex list

        using  AbstractPolyhedralMesh<M,V,E,F,P>::poly_add; // avoid hiding poly_add(flist,fwinding);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void reorder_p2v(const uint pid);

        // reorder_p2v() makes sure the p2v adjacency stores vertices
        // in a way that uniquely defines per element connectivity:
        //
        //       v7------v6
        //      / |     / |
        //    v4------v5  |
        //    |   |    |  |
        //    |  v3----|--v2
        //    | /      | /
        //    v0------v1
};

}

#ifndef  CINO_STATIC_LIB
#include "hexmesh.cpp"
#endif

#endif // CINO_HEXMESH_H
