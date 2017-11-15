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
#ifndef CINO_DRAWABLE_POLYHEDRALMESH_H
#define CINO_DRAWABLE_POLYHEDRALMESH_H

#ifdef CINOLIB_USES_OPENGL

#include <cinolib/meshes/polyhedralmesh.h>
#include <cinolib/meshes/abstract_drawable_polyhedralmesh.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class F = Polygon_std_attributes,
         class P = Polyhedron_std_attributes>
class DrawablePolyhedralmesh : public AbstractDrawablePolyhedralMesh<Polyhedralmesh<M,V,E,F,P>>
{
    public:

        explicit DrawablePolyhedralmesh() : Polyhedralmesh<M,V,E,F,P>()
        {
            this->init_drawable_stuff();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit DrawablePolyhedralmesh(const char * filename) : Polyhedralmesh<M,V,E,F,P>(filename)
        {
            this->init_drawable_stuff();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit DrawablePolyhedralmesh(const std::vector<vec3d>             & verts,
                                        const std::vector<std::vector<uint>> & faces,
                                        const std::vector<std::vector<uint>> & polys,
                                        const std::vector<std::vector<bool>> & polys_face_winding)
        : Polyhedralmesh<M,V,E,F,P>(verts, faces, polys, polys_face_winding)
        {
            this->init_drawable_stuff();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type() const { return DRAWABLE_POLYHEDRALMESH; }
};

}

#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_DRAWABLE_POLYHEDRALMESH_H
