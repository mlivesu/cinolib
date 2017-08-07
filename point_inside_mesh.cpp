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
#include <cinolib/point_inside_mesh.h>
#include <cinolib/meshes/tetmesh.h>
#include <cinolib/geometry/tetrahedron.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
PointInsideMeshCache<Mesh>::PointInsideMeshCache(const Mesh & m, const uint octree_depth) : m_ptr(&m)
{
    octree = Octree<uint>(m.bbox().min, m.bbox().max);
    octree.subdivide_n_levels(octree_depth);

    for(uint eid=0; eid<m.num_polys(); ++eid)
    {
        vec3d min = m.poly_vert(eid, 0);
        vec3d max = min;
        for(uint i=1; i<m.verts_per_poly(eid); ++i)
        {
            min = min.min(m.poly_vert(eid, i));
            max = max.max(m.poly_vert(eid, i));
        }
        octree.add_item(eid, min, max);
    }
}


/* WARNING: this will return the *FIRST* element that contains p
 * For correctness this should return *ALL* the elements containing
 * p, and let the caller choose the best element depending on the needs...
*/
template<class Mesh>
CINO_INLINE
void PointInsideMeshCache<Mesh>::locate(const vec3d p, uint & eid, std::vector<double> & wgts) const
{
    std::set<uint> items;
    octree.get_items(p, items);

    for(uint id : items)
    {
        if (m_ptr->poly_bary_coords(id, p, wgts)) // if is inside...
        {
            eid = id;
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

    std::set<std::pair<double,uint>,std::greater<std::pair<double,uint>>> ordered_items;
    for(int item : items)
    {
        ordered_items.insert(std::make_pair(m_ptr->poly_centroid(item).dist(p),item));
    }
    assert(ordered_items.size()>0);
    eid = (*ordered_items.begin()).second;
    wgts = std::vector<double>(m_ptr->verts_per_poly(eid), 1.0/double(m_ptr->verts_per_poly(eid))); // centroid
}


/* WARNING: this will return the *FIRST* element that contains p
 * For correctness this should return *ALL* the elements containing
 * p, and let the caller choose the best element depending on the needs...
*/
template<class Mesh>
CINO_INLINE
vec3d PointInsideMeshCache<Mesh>::locate(const vec3d p, const Mesh & m) const
{
    uint eid;
    std::vector<double> wgts;
    locate(p, eid, wgts);

    vec3d tmp(0,0,0);
    for(uint off=0; off<m.verts_per_poly(eid); ++off)
    {
        tmp += wgts.at(off) * m.poly_vert(eid,off);
    }
    return tmp;
}

}
