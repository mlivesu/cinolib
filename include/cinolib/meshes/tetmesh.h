/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
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

        ~Tetmesh() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const override { return TETMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void load(const char * filename) override;
        void save(const char * filename) const override;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_f_normal(const uint fid)  override;
        void update_tet_quality(const uint pid);
        void update_tet_quality();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_poly(const uint) const override { return  4; }
        uint verts_per_poly()           const          { return  4; }
        uint edges_per_poly(const uint) const override { return  6; }
        uint edges_per_poly()           const          { return  6; }
        uint faces_per_poly(const uint) const override { return  4; }
        uint faces_per_poly()           const          { return  4; }
        uint verts_per_face(const uint) const override { return  3; }
        uint verts_per_face()           const          { return  3; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void vert_weights          (const uint vid, const int type, std::vector<std::pair<uint,double>> & wgts) const override;
        void vert_weights_cotangent(const uint vid, std::vector<std::pair<uint,double>> & wgts) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint edge_split                       (const uint eid, const vec3d & p);
        uint edge_split                       (const uint eid, const double lambda = 0.5); // use linear interpolation: e0*(1-lambda) + e1*lambda
        int  edge_collapse                    (const uint eid, const double lambda = 0.5, const double topologic_check = true, const double geometric_check = true);
        int  edge_collapse                    (const uint eid, const vec3d & p, const double topologic_check = true, const double geometric_check = true);
        bool edge_is_collapsible              (const uint eid, const vec3d & p) const;
        bool edge_is_collapsible              (const uint eid, const double lambda) const;
        bool edge_is_geometrically_collapsible(const uint eid, const vec3d & p) const;
        bool edge_is_topologically_collapsible(const uint eid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double face_area            (const uint fid) const;
        uint   face_edge_opposite_to(const uint fid, const uint vid) const;
        uint   face_vert_opposite_to(const uint fid, const uint eid) const;
        uint   face_split           (const uint fid, const vec3d & p);
        uint   face_split           (const uint fid, const std::vector<double> & bc = { 1./3., 1./3., 1./3. });

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double            poly_dihedral_angle  (const uint pid, const uint fid0, const uint fid1) const;
        uint              poly_vert_opposite_to(const uint pid, const uint fid) const;
        uint              poly_edge_opposite_to(const uint pid, const uint eid) const;
        uint              poly_edge_opposite_to(const uint pid, const uint vid0, const uint vid1) const;
        uint              poly_face_opposite_to(const uint pid, const uint vid) const;
        std::vector<uint> poly_faces_opposite_to(const uint pid, const uint eid) const;
        int               poly_shared_vert      (const uint pid, const std::vector<uint> & incident_edges) const; // TODO: move to global ids!!!!!!
        void              poly_bary_coords      (const uint pid, const vec3d & p, double bc[]) const;
        double            poly_volume           (const uint pid) const override;
        uint              poly_split            (const uint pid, const vec3d & p);
        uint              poly_split            (const uint pid, const std::vector<double> & bc = { 0.25, 0.25, 0.25, 0.25 });
        void              polys_split           (const std::vector<uint> & pids);
        uint              poly_add              (const std::vector<uint> & vlist) override; // vertex list

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
