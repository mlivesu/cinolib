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
#include <cinolib/find_intersections.h>
#include <cinolib/parallel_for.h>
#include <cinolib/octree.h>
#include <mutex>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void find_intersections(const Trimesh<M,V,E,P> & m,
                              std::set<ipair>  & intersections)
{
    auto tris = serialized_vids_from_polys(m.vector_polys());
    find_intersections(m.vector_verts(), tris, intersections);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void find_intersections(const std::vector<vec3d> & verts,
                        const std::vector<uint>  & tris,
                              std::set<ipair>    & intersections)
{
    Octree o(8,1000); // max 1000 elements per leaf, depth permitting
    o.build_from_vectors(verts, tris);

    std::mutex mutex;
    PARALLEL_FOR(0, o.leaves.size(), 1, [&](uint i)
    {        
        auto & leaf = o.leaves.at(i);
        if(leaf->item_indices.empty()) return;
        for(uint j=0;   j<leaf->item_indices.size()-1; ++j)
        for(uint k=j+1; k<leaf->item_indices.size();   ++k)
        {
            uint tid0 = leaf->item_indices.at(j);
            uint tid1 = leaf->item_indices.at(k);
            auto T0 = o.items.at(tid0);
            auto T1 = o.items.at(tid1);
            if(T0->aabb.intersects_box(T1->aabb)) // early reject based on AABB intersection
            {
                const Triangle *t0 = dynamic_cast<Triangle*>(T0);
                const Triangle *t1 = dynamic_cast<Triangle*>(T1);
                if(t0->intersects_triangle(t1->v,true)) // precise check (exact if CINOLIB_USES_SHEWCHUK_PREDICATES is defined)
                {
                    std::lock_guard<std::mutex> guard(mutex);
                    intersections.insert(unique_pair(tid0,tid1));
                }
            }
        }
    });
}

}
