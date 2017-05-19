/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
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
#include <cinolib/vertex_clustering.h>
#include <cinolib/bfs.h>

namespace cinolib
{

template<class Vertex>
CINO_INLINE
void vertex_clustering(const std::vector<Vertex>   & points,
                       const double                  proximity_thresh,
                       std::vector<std::set<uint>> & clusters)
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
        std::set<uint> cluster;
        bfs_exahustive(v2v, seed, cluster);

        clusters.push_back(cluster);
        for(uint vid : cluster) visited.at(vid) = true;

        seed = 0;
        while (seed < nv && visited.at(seed)) ++seed;
    }
    while (seed < nv);
}


}

