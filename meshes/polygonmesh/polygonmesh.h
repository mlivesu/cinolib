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
#ifndef CINO_POLYGONMESH_H
#define CINO_POLYGONMESH_H

#include <vector>
#include <cinolib/cinolib.h>
#include <cinolib/meshes/abstract_surface_mesh.h>
#include <cinolib/meshes/mesh_std_data.h>

namespace cinolib
{

template<class M = Mesh_std_data, // default template arguments
         class V = Vert_std_data,
         class E = Edge_std_data,
         class P = Face_std_data>
class Polygonmesh : public AbstractPolygonMesh<M,V,E,P>
{
    protected:

        std::vector<std::vector<uint>> triangulated_polys; // triangles covering each polygon. Useful for
                                                           // robust normal estimation and rendering
    public:

        Polygonmesh(){}

        Polygonmesh(const char * filename);

        Polygonmesh(const std::vector<vec3d>             & verts,
                    const std::vector<std::vector<uint>> & polys);

        Polygonmesh(const std::vector<double>            & coords,
                    const std::vector<std::vector<uint>> & polys);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const { return POLYGONMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void clear();
        void init();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void operator+=(const Polygonmesh<M,V,E,P> & m);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void update_p_normal(const uint fid);
        void update_poly_tessellation();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        std::vector<uint> get_ordered_boundary_vertices() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double poly_area(const uint) const { assert(false); } // TODO!
};

}

#ifndef  CINO_STATIC_LIB
#include "polygonmesh.cpp"
#endif

#endif // CINO_POLYGONMESH_H
