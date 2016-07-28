/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include "point_inside_mesh.h"
#include "../meshes/tetmesh/tetmesh.h"

namespace cinolib
{

template<>
CINO_INLINE
PointInsideMeshCache<Tetmesh>::PointInsideMeshCache(const Tetmesh & m) : m(m)
{
    octree = Octree<int>(m.bbox().min, m.bbox().max);
    octree.subdivide_n_levels(5);
    for(int tid=0; tid<m.num_tetrahedra(); ++tid)
    {
        vec3d min = m.tet_vertex(tid, 0);
        vec3d max = min;
        for(int i=1; i<4; ++i)
        {
            min = min.min(m.tet_vertex(tid, i));
            max = max.max(m.tet_vertex(tid, i));
        }
        octree.add_item(tid, min, max);
    }
}

template<>
CINO_INLINE
void PointInsideMeshCache<Tetmesh>::locate(const vec3d p, int & tid, double wgts[4]) const
{
    std::set<int> items;
    octree.get_items(p, items);

    if (items.empty())
    {
        std::cerr << "WARNING! point " << p << " not found inside tetmesh! " << std::endl;
        std::cerr << "BBmin: " << m.bbox().min << std::endl;
        std::cerr << "BBmax: " << m.bbox().max << std::endl;
    }
    assert(!items.empty());

    int    best_item = -1;
    double best_qual = -FLT_MAX;
    double best_wgts[4];

    for(int item : items)
    {
        assert(item >= 0 || item < m.num_elements());

        double q = m.barycentric_coordinates(item, p, wgts);

        if (q > best_qual)
        {
            best_item    = item;
            best_qual    = q;
            best_wgts[0] = wgts[0];
            best_wgts[1] = wgts[1];
            best_wgts[2] = wgts[2];
            best_wgts[3] = wgts[3];
        }
    }
    assert(best_item != -1);

    tid = best_item;
    wgts[0] = best_wgts[0];
    wgts[1] = best_wgts[1];
    wgts[2] = best_wgts[2];
    wgts[3] = best_wgts[3];
}


template<>
CINO_INLINE
vec3d PointInsideMeshCache<Tetmesh>::locate(const vec3d p, const Tetmesh & obj) const
{
    int    tid;
    double wgts[4];
    locate(p, tid, wgts);

    assert(tid >= 0 && obj.num_elements() > tid);

    vec3d tmp = wgts[0] * obj.tet_vertex(tid, 0) +
                wgts[1] * obj.tet_vertex(tid, 1) +
                wgts[2] * obj.tet_vertex(tid, 2) +
                wgts[3] * obj.tet_vertex(tid, 3);

    return tmp;
}

}
