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

