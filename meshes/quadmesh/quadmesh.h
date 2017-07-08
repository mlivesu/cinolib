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
#ifndef CINO_QUADMESH_H
#define CINO_QUADMESH_H

#include <cinolib/cinolib.h>
#include <cinolib/meshes/mesh_std_data.h>
#include <cinolib/meshes/abstract_surface_mesh.h>

#include <map>
#include <set>

namespace cinolib
{

static const uint QUAD_EDGES[4][2] =
{
    { 0, 1 }, // 0
    { 1, 2 }, // 1
    { 2, 3 }, // 2
    { 3, 0 }, // 3
};

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class F = Face_std_data>
class Quadmesh : public AbstractSurfaceMesh<M,V,E,F>
{
    protected:

        std::vector<std::vector<uint>> tessellated_faces; // triangles covering each quad. Useful for
                                                          // robust normal estimation and rendering

    public:

        Quadmesh(){}

        Quadmesh(const char * filename);

        Quadmesh(const std::vector<vec3d> & verts,
                 const std::vector<uint>  & faces);

        Quadmesh(const std::vector<double> & coords,
                 const std::vector<uint>   & faces);

        Quadmesh(const std::vector<vec3d>             & verts,
                 const std::vector<std::vector<uint>> & faces);

        Quadmesh(const std::vector<double>            & coords,
                 const std::vector<std::vector<uint>> & faces);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const { return QUADMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void clear();
        void init();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_f_normal(const uint fid);
        void update_face_tessellation();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_face(const uint) const { return 4; }
        uint verts_per_elem(const uint) const { return 4; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint num_elems() const { return this->faces.size();     } // elem == face!!

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const std::vector<uint> & adj_elem2elem(const uint fid) const { return this->f2f.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const F & elem_data(const uint fid) const { return this->f_data.at(fid); } // elem == face!!
              F & elem_data(const uint fid)       { return this->f_data.at(fid); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool              vert_is_singular     (const uint vid) const;
        bool              vert_is_regular      (const uint vid) const;        
        bool              vert_is_saddle       (const uint vid, const int tex_coord = U_param) const;
        bool              vert_is_critical_p   (const uint vid, const int tex_coord = U_param) const;
        std::vector<uint> vert_loop            (const uint start, const uint next) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double face_area (const uint fid) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d  elem_centroid(const uint fid) const;
        void   elem_show_all();
        vec3d  elem_vert       (const uint eid, const uint offset) const;
        uint   elem_vert_id    (const uint eid, const uint offset) const;
        uint   elem_vert_offset(const uint eid, const uint vid) const;
        double elem_mass       (const uint eid) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "quadmesh.cpp"
#endif

#endif // CINO_QUADMESH_H
