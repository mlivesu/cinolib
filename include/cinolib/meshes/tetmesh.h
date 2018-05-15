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
#ifndef CINO_TETMESH_H
#define CINO_TETMESH_H

#include <sys/types.h>
#include <vector>
#include <cinolib/meshes/abstract_polyhedralmesh.h>
#include <cinolib/meshes/mesh_attributes.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class F = Polygon_std_attributes,
         class P = Polyhedron_std_attributes>
class Tetmesh : public AbstractPolyhedralMesh<M,V,E,F,P>
{
    public:

        explicit Tetmesh(){}

        explicit Tetmesh(const char * filename);

        explicit Tetmesh(const std::vector<double> & coords,
                         const std::vector<uint>   & polys);

        explicit Tetmesh(const std::vector<vec3d> & verts,
                         const std::vector<uint>  & polys);

        explicit Tetmesh(const std::vector<vec3d>             & verts,
                         const std::vector<std::vector<uint>> & polys);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const { return TETMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void load(const char * filename);
        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init_tetmesh(const std::vector<vec3d>             & verts,
                          const std::vector<std::vector<uint>> & polys);
        void init_tetmesh(const std::vector<vec3d>             & verts,
                          const std::vector<std::vector<uint>> & polys,
                          const std::vector<int>               & vert_labels,
                          const std::vector<int>               & poly_labels);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_f_normal(const uint fid);
        void update_tet_quality(const uint pid);
        void update_tet_quality();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_poly(const uint) const { return  4; }
        uint verts_per_poly()           const { return  4; }
        uint edges_per_poly(const uint) const { return  6; }
        uint edges_per_poly()           const { return  6; }
        uint faces_per_poly(const uint) const { return  4; }
        uint faces_per_poly()           const { return  4; }
        uint verts_per_face(const uint) const { return  3; }
        uint verts_per_face()           const { return  3; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void vert_weights          (const uint vid, const int type, std::vector<std::pair<uint,double>> & wgts) const;
        void vert_weights_cotangent(const uint vid, std::vector<std::pair<uint,double>> & wgts) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint edge_split(const uint eid, const vec3d & p);
        uint edge_split(const uint eid, const double lambda = 0.5); // use linear interpolation: e0*(1-lambda) + e1*lambda

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double face_area            (const uint fid) const;
        uint   face_edge_opposite_to(const uint fid, const uint vid) const;
        uint   face_split           (const uint fid, const vec3d & p);
        uint   face_split           (const uint fid, const std::vector<double> & bary = { 1./3., 1./3., 1./3. });

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double            poly_dihedral_angle  (const uint pid, const uint fid0, const uint fid1) const;
        uint              poly_vert_opposite_to(const uint pid, const uint fid) const;
        uint              poly_edge_opposite_to(const uint pid, const uint eid) const;
        uint              poly_edge_opposite_to(const uint pid, const uint vid0, const uint vid1) const;
        uint              poly_face_opposite_to(const uint pid, const uint vid) const;
        std::vector<uint> poly_faces_opposite_to(const uint pid, const uint eid) const;
        int               poly_shared_vert      (const uint pid, const std::vector<uint> & incident_edges) const; // TODO: move to global ids!!!!!!
        bool              poly_bary_coords      (const uint pid, const vec3d & p, std::vector<double> & wgts) const;
        double            poly_volume           (const uint pid) const;
        uint              poly_split            (const uint pid, const vec3d & p);
        uint              poly_split            (const uint pid, const std::vector<double> & bary = { 0.25, 0.25, 0.25, 0.25 });
        void              polys_split           (const std::vector<uint> & pids);
        uint              poly_add              (const std::vector<uint> & vlist); // vertex list

        using  AbstractPolyhedralMesh<M,V,E,F,P>::poly_add; // avoid hiding poly_add(flist,fwinding);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void reorder_p2v(const uint pid);

        /* reorder_p2v() makes sure the p2v adjacency stores vertices
           in a way that uniquely defines per element connectivity:

                    v2
                   /| \
                 /  |   \
               v0---|----v3
                \   |   /
                  \ | /
                   v1
        */
};

}

#ifndef  CINO_STATIC_LIB
#include "tetmesh.cpp"
#endif

#endif // CINO_TETMESH_H
