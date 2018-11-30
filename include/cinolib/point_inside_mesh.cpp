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
bool PointInsideMeshCache<Mesh>::locate(const vec3d p, uint & pid, std::vector<double> & wgts) const
{
    std::set<uint> items;
    octree.get_items(p, items);

    for(uint id : items)
    {
        if (m_ptr->poly_bary_coords(id, p, wgts)) // if is inside...
        {
            pid = id;
            return true;
        }
    }
    return false;

    //std::cerr << "I'll assign it to its closest mesh element         " << std::endl;
    //std::cerr << "BBmin: " << m_ptr->bbox().min << std::endl;
    //std::cerr << "BBmax: " << m_ptr->bbox().max << std::endl;

    //std::set<std::pair<double,uint>,std::greater<std::pair<double,uint>>> ordered_items;
    //for(int item : items)
    //{
    //    ordered_items.insert(std::make_pair(m_ptr->poly_centroid(item).dist(p),item));
    //}
    //assert(ordered_items.size()>0);
    //pid = (*ordered_items.begin()).second;
    //wgts = std::vector<double>(m_ptr->verts_per_poly(pid), 1.0/double(m_ptr->verts_per_poly(pid))); // centroid
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
    if(locate(p, eid, wgts))
    {
        vec3d tmp(0,0,0);
        for(uint off=0; off<m.verts_per_poly(eid); ++off)
        {
            tmp += wgts.at(off) * m.poly_vert(eid,off);
        }
        return tmp;
    }
    else
    {
        std::cerr << "WARNING! point " << p << " is not inside the mesh! " << std::endl;
        return p;
    }
}

}
