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
#ifndef CINO_ABSTRACT_VOLUME_MESH_H
#define CINO_ABSTRACT_VOLUME_MESH_H

#include <cinolib/meshes/abstract_mesh.h>

namespace cinolib
{

template<class M, // mesh attributes
         class V, // vert attributes
         class E, // edge attributes
         class F, // face attributes
         class P> // polyhedron attributes
class AbstractVolumeMesh : public AbstractMesh<M,V,E,P>
{
    protected:

        std::vector<std::vector<uint>> faces;               // list of faces (assumed CCW)
        std::vector<std::vector<uint>> triangulated_faces;  // triangles covering each face (e.g., for rendering)
        std::vector<bool>              v_on_srf;            // true if a vertex is exposed on the surface
        std::vector<bool>              e_on_srf;            // true if an edge is exposed on the surface
        std::vector<bool>              f_on_srf;            // true if a face is exposed on the surface

        std::vector<F> f_data;

        std::vector<std::vector<uint>> v2f; // vert to face adjacency
        std::vector<std::vector<uint>> e2f; // edge to face adjacency
        std::vector<std::vector<uint>> f2e; // face to edge adjacency
        std::vector<std::vector<uint>> f2f; // face to face adjacency (through edges)
        std::vector<std::vector<uint>> f2p; // face to poly adjacency
        std::vector<std::vector<uint>> p2v; // poly to edge adjacency
};

}

#ifndef  CINO_STATIC_LIB
#include "abstract_volume_mesh.cpp"
#endif

#endif // CINO_ABSTRACT_VOLUME_MESH_H
