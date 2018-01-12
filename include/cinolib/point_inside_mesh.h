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
#ifndef CINO_POINT_INSIDE_MESH_H
#define CINO_POINT_INSIDE_MESH_H

#include <sys/types.h>
#include <vector>
#include <cinolib/octree.h>

namespace cinolib
{

template<class Mesh>
class PointInsideMeshCache
{
    private:

        const Mesh *m_ptr;
        Octree<uint> octree;

    public:

        explicit PointInsideMeshCache(const Mesh & m, const uint octree_depth = 5);

        void  locate(const vec3d p, uint & eid, std::vector<double> & wgts) const;
        vec3d locate(const vec3d p, const Mesh & m) const;
};

}

#ifndef  CINO_STATIC_LIB
#include "point_inside_mesh.cpp"
#endif

#endif // CINO_POINT_INSIDE_MESH_H
