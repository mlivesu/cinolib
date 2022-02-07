/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#include <cinolib/3d_printing/overhangs.h>
#include <cinolib/parallel_for.h>
#include <cinolib/octree.h>
#include <cinolib/find_intersections.h>
#include <mutex>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void overhangs(const Trimesh<M,V,E,P>  & m,
               const float               thresh, // degrees
               const vec3d             & build_dir,
                     std::vector<uint> & polys_hanging)
{
    std::mutex mutex;
    PARALLEL_FOR(0, m.num_polys(), 1000, [&](const uint pid)
    {
        float ang = build_dir.angle_deg(m.poly_data(pid).normal);
        if(ang-90.f > thresh)
        {
            std::lock_guard<std::mutex> guard(mutex);
            polys_hanging.push_back(pid);
        }
    });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void overhangs(const Trimesh<M,V,E,P>                  & m,
               const float                               thresh, // degrees
               const vec3d                             & build_dir,
                     std::vector<std::pair<uint,uint>> & polys_hanging,
               const Octree                            & octree) // cached
{
    // find overhanging triangles
    std::vector<uint> tmp;
    overhangs(m, thresh, build_dir, tmp);

    // cast a ray from each overhang to find the first triangle below it
    std::mutex mutex;
    PARALLEL_FOR(0, tmp.size(), 1000, [&](const uint i)
    {
        uint pid  = tmp[i];
        auto pair = std::make_pair(pid,pid);
        std::set<std::pair<double,uint>> hits;
        if(octree.intersects_ray(m.poly_centroid(pid), -build_dir, hits))
        {
            auto hit = hits.begin();
            if(hit->second==pid) ++hit; // skip the first hit, it's the starting polygon
            if(hit!=hits.end())
            {
                pair.second = hit->second;
            }
        }
        // critical section
        {
            std::lock_guard<std::mutex> guard(mutex);
            polys_hanging.push_back(pair);
        }
    });
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void overhangs(const Trimesh<M,V,E,P>                  & m,
               const float                               thresh, // degrees
               const vec3d                             & build_dir,
                     std::vector<std::pair<uint,uint>> & polys_hanging)
{
    Octree octree;
    octree.build_from_mesh_polys(m);
    overhangs(m, thresh, build_dir, polys_hanging, octree);
}

}
