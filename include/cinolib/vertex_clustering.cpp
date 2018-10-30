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
#include <cinolib/vertex_clustering.h>
#include <cinolib/bfs.h>

namespace cinolib
{

template<class Vertex>
CINO_INLINE
void vertex_clustering(const std::vector<Vertex>             & points,
                       const double                            proximity_thresh,
                       std::vector<std::unordered_set<uint>> & clusters)
{
    // build v2v connectivity based on point proximity
    std::vector<std::vector<uint>> v2v(points.size());
    for(uint vid0=0;      vid0<points.size()-1; ++vid0)
    for(uint vid1=vid0+1; vid1<points.size();   ++vid1)
    {
        if (points.at(vid0).dist(points.at(vid1)) < proximity_thresh)
        {
            v2v.at(vid0).push_back(vid1);
            v2v.at(vid1).push_back(vid0);
        }
    }

    // visit the resulting graph with BFS to
    // isolate clusters of adjacent vertices
    uint nv   = points.size();
    uint seed = 0;
    std::vector<bool> visited(nv, false);
    do
    {
        std::unordered_set<uint> cluster;
        bfs(v2v, seed, cluster);

        clusters.push_back(cluster);
        for(uint vid : cluster) visited.at(vid) = true;

        seed = 0;
        while (seed < nv && visited.at(seed)) ++seed;
    }
    while (seed < nv);
}


}

