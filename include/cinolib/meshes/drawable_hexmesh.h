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
#ifndef CINO_DRAWABLE_HEXMESH_H
#define CINO_DRAWABLE_HEXMESH_H

#ifdef CINOLIB_USES_OPENGL

#include <cinolib/meshes/hexmesh.h>
#include <cinolib/meshes/abstract_drawable_polyhedralmesh.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class F = Polygon_std_attributes,
         class P = Polyhedron_std_attributes>
class DrawableHexmesh : public AbstractDrawablePolyhedralMesh<Hexmesh<M,V,E,F,P>>
{
    public:

        ~DrawableHexmesh(){}

        explicit DrawableHexmesh() : Hexmesh<M,V,E,F,P>()
        {
            this->init_drawable_stuff();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit DrawableHexmesh(const char * filename) : Hexmesh<M,V,E,F,P>(filename)
        {
            this->init_drawable_stuff();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit DrawableHexmesh(const std::vector<vec3d>             & verts,
                                 const std::vector<std::vector<uint>> & faces,
                                 const std::vector<std::vector<uint>> & polys,
                                 const std::vector<std::vector<bool>> & polys_face_winding)
        : Hexmesh<M,V,E,F,P>(verts, faces, polys, polys_face_winding)
        {
            this->init_drawable_stuff();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit DrawableHexmesh(const std::vector<vec3d> & verts,
                                 const std::vector<uint>  & polys)
        : Hexmesh<M,V,E,F,P>(verts, polys)
        {
            this->init_drawable_stuff();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        explicit DrawableHexmesh(const std::vector<double> & coords,
                                 const std::vector<uint>   & polys)
        : Hexmesh<M,V,E,F,P>(coords, polys)
        {
            this->init_drawable_stuff();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        ObjectType object_type() const { return DRAWABLE_HEXMESH; }
};

}

#endif // #ifdef CINOLIB_USES_OPENGL

#endif // CINO_DRAWABLE_HEXMESH_H
