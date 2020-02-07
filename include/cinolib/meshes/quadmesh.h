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
#ifndef CINO_QUADMESH_H
#define CINO_QUADMESH_H

#include <vector>
#include <sys/types.h>
#include <cinolib/meshes/mesh_attributes.h>
#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

template<class M = Mesh_std_attributes, // default template arguments
         class V = Vert_std_attributes,
         class E = Edge_std_attributes,
         class P = Polygon_std_attributes>
class Quadmesh : public AbstractPolygonMesh<M,V,E,P>
{
    public:

        explicit Quadmesh(){}

        explicit Quadmesh(const char * filename);

        explicit Quadmesh(const std::vector<vec3d> & verts,
                          const std::vector<uint>  & polys);

        explicit Quadmesh(const std::vector<double> & coords,
                          const std::vector<uint>   & polys);

        explicit Quadmesh(const std::vector<vec3d>             & verts,
                          const std::vector<std::vector<uint>> & polys);

        explicit Quadmesh(const std::vector<double>            & coords,
                          const std::vector<std::vector<uint>> & polys);

        ~Quadmesh(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const override { return QUADMESH; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint verts_per_poly(const uint) const override { return 4; }
        uint verts_per_poly()           const          { return 4; }
        uint edges_per_poly(const uint) const override { return 4; }
        uint edges_per_poly()           const          { return 4; }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool              vert_is_singular     (const uint vid) const;
        bool              vert_is_regular      (const uint vid) const;
        int               vert_next_along_chain(const uint curr, const uint prev) const;
        std::vector<uint> vert_chain           (const uint start, const uint next) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        int                            edge_next_along_chain(const uint eid, const uint vid) const;
        uint                           edge_opposite_to(const uint pid, const uint eid) const;
        std::vector<uint>              edges_opposite_to(const uint eid) const;
        std::vector<uint>              edge_chain(const uint eid, const uint vid) const; // chain of ADJACENT edges
        std::vector<uint>              edge_parallel_chain(const uint eid) const;        // chain of PARALLEL edges
        std::vector<std::vector<uint>> edge_parallel_chains() const;                     // chain of PARALLEL edges
};

}

#ifndef  CINO_STATIC_LIB
#include "quadmesh.cpp"
#endif

#endif // CINO_QUADMESH_H
