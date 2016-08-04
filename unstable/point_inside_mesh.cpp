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
#include <cinolib/unstable/point_inside_mesh.h>
#include <cinolib/meshes/tetmesh/tetmesh.h>
#include <cinolib/geometry/tetrahedron.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
PointInsideMeshCache<Mesh>::PointInsideMeshCache(const Mesh & m, const int octree_depth) : m_ptr(&m)
{
    octree = Octree<int>(m.bbox().min, m.bbox().max);
    octree.subdivide_n_levels(octree_depth);

    for(int tid=0; tid<m.num_elements(); ++tid)
    {
        vec3d min = m.elem_vertex(tid, 0);
        vec3d max = min;
        for(int i=1; i<m.verts_per_element; ++i)
        {
            min = min.min(m.elem_vertex(tid, i));
            max = max.max(m.elem_vertex(tid, i));
        }
        octree.add_item(tid, min, max);
    }
}


/* WARNING: this will return the *FIRST* element that contains p
 * For correctness this should return *ALL* the elements containing
 * p, and let the caller choose the best element depending on the needs...
*/
template<class Mesh>
CINO_INLINE
void PointInsideMeshCache<Mesh>::locate(const vec3d p, int & tid, std::vector<double> & wgts) const
{
    std::set<int> items;
    octree.get_items(p, items);

    for(int tet : items)
    {
        if (m_ptr->barycentric_coordinates(tet, p, wgts)) // if is inside...
        {
            tid = tet;
            return;
        }
    }

    /* this should not be here... it's a matter of the application, not of the data structure
     * to handle corner cases...
    */
    std::cerr << "WARNING! point " << p << " is not inside the mesh! " << std::endl;
    std::cerr << "I'll assign it to its closest mesh element         " << std::endl;
    std::cerr << "BBmin: " << m_ptr->bbox().min << std::endl;
    std::cerr << "BBmax: " << m_ptr->bbox().max << std::endl;

    std::set< std::pair<double,int>,std::greater< std::pair<double,int> > > ordered_items;
    for(int item : items)
    {
        ordered_items.insert(std::make_pair(m_ptr->element_barycenter(item).dist(p),item));
    }
    tid = (*ordered_items.begin()).second;
    wgts = std::vector<double>(m_ptr->verts_per_element, 1.0/double(m_ptr->verts_per_element)); // centroid
}


/* WARNING: this will return the *FIRST* element that contains p
 * For correctness this should return *ALL* the elements containing
 * p, and let the caller choose the best element depending on the needs...
*/
template<class Mesh>
CINO_INLINE
vec3d PointInsideMeshCache<Mesh>::locate(const vec3d p, const Mesh & m) const
{
    int tid;
    std::vector<double> wgts(m.verts_per_element);
    locate(p, tid, wgts);

    vec3d tmp(0,0,0);
    for(int i=0; i<m.verts_per_element; ++i)
    {
        tmp += wgts.at(i) * m.elem_vertex(tid,i);
    }
    return tmp;
}

}
