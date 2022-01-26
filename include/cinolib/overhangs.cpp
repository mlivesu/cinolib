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
*                                                                               *
*     Fabrizio Corda (cordafab@gmail.com)                                       *
*     http://www.fabriziocorda.com                                              *
*                                                                               *
*     University of Cagliari                                                    *
*     Via Ospedale, 72                                                          *
*     09124 Cagliari,                                                           *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/overhangs.h>
#include <cinolib/parallel_for.h>
#include <cinolib/octree.h>
#include <cinolib/find_intersections.h>
#include <mutex>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void overhangs(const Trimesh<M,V,E,P>  & m,
               const float               thresh,                    // degrees
                     std::vector<uint> & polys_hanging,             // IDs of hanging polys
                     std::vector<uint> & polys_below_hanging_polys, // IDs of polys lying below some overhang
               const mat3d             & R)                         // global orientation matrix
{
    static const vec3d build_dir = R * vec3d(0,0,1);

    std::mutex mutex;
    PARALLEL_FOR(0, m.num_polys(), 1000, [&](const uint pid)
    {
        float ang = build_dir.angle_deg(m.poly_data(pid).normal);
        if(ang-90.f > thresh)
        {
            {
                // critical section
                std::lock_guard<std::mutex> guard(mutex);
                polys_hanging.push_back(pid);
            }
        }
    });

    // for each hanging polygon, throw a ray opposite to the build direction to
    // discover whether the support structures will emanate from the build
    // platform or from some triangle below
    Octree octree;
    octree.build_from_mesh_polys(m);
    PARALLEL_FOR(0, polys_hanging.size(), 1000, [&](const uint i)
    {
        uint pid = polys_hanging[i];
        std::set<std::pair<double,uint>> hits;
        if(octree.intersects_ray(m.poly_centroid(pid), -build_dir, hits))
        {
            auto h = hits.begin();
            if(h->second == pid) ++h; // skip the first hit, it's the starting polygon
            if(h!=hits.end())
            {
                std::lock_guard<std::mutex> guard(mutex);
                polys_below_hanging_polys.push_back(h->second);
            }
        }
    });
}

}
