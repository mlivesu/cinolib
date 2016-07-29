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
#include <cinolib/bfs.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
void bfs_exahustive(const Mesh          & m,
                    const int             source,
                          std::set<int> & visited)
{
    assert(visited.empty());

    std::set<int> active_set;
    active_set.insert(source);

    int vid;
    while (!active_set.empty())
    {
        vid = *active_set.begin();
        active_set.erase(active_set.begin());

        visited.insert(vid);

        for(int nbr : m.adj_vtx2vtx(vid))
        {
            if (DOES_NOT_CONTAIN(visited,nbr))
            {
                active_set.insert(nbr);
            }
        }
    }
}


// shortest path on unweighted graph, essentially dijkstra with constaint weights.
//
template<class Mesh>
CINO_INLINE
void bfs(const Mesh             & m,
         const int                source,
         const int                dest,
               std::vector<int> & path)

{
    assert(path.empty());

    int inf_dist = m.num_edges()+1;
    std::vector<int> prev(m.num_vertices(), -1);
    std::vector<int> dist(m.num_vertices(), inf_dist);
    dist[source] = 0;

    std::set<ipair> active_set;
    active_set.insert(std::make_pair(0, source));

    int vid;
    while (!active_set.empty())
    {
        vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (vid == dest)
        {
            do
            {
                path.push_back(vid);
                vid = prev[vid];
            }
            while(vid != -1);
            return;
        }

        for(int nbr : m.adj_vtx2vtx(vid))
        {
            float tmp = dist[vid] + 1;

            if (dist[nbr] > tmp)
            {
                if (dist[nbr] < inf_dist)
                {
                    auto it = active_set.find(std::make_pair(dist[nbr], nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist[nbr] = tmp;
                prev[nbr]  = vid;
                active_set.insert(std::make_pair(dist[nbr], nbr));
            }
        }
    }
    assert(false && "BFS did not converge!");
}


// shortest path (with barriers) on unweighted graph.
// The path cannot pass throuh vertices for which mask[v] = true
//
template<class Mesh>
CINO_INLINE
void bfs(const Mesh              & m,
         const int                 source,
         const int                 dest,
         const std::vector<bool> & mask,
               std::vector<int>  & path)
{
    assert(path.empty());
    assert(mask.size() == m.num_vertices());

    int inf_dist = m.num_edges()+1;
    std::vector<int> prev(m.num_vertices(), -1);
    std::vector<int> dist(m.num_vertices(), inf_dist);
    dist[source] = 0;

    std::set<ipair> active_set;
    active_set.insert(std::make_pair(0, source));

    int vid;
    while (!active_set.empty())
    {
        vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (dest == vid)
        {
            do
            {
                path.push_back(vid);
                vid = prev[vid];
            }
            while(vid != -1);
            return;
        }

        for(int nbr : m.adj_vtx2vtx(vid))
        {
            float tmp = dist[vid] + 1;

            if (mask[nbr]) continue;
            if (dist[nbr] > tmp)
            {
                if (dist[nbr] < inf_dist)
                {
                    auto it = active_set.find(std::make_pair(dist[nbr], nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist[nbr] = tmp;
                prev[nbr]  = vid;
                active_set.insert(std::make_pair(dist[nbr], nbr));
            }
        }
    }
    assert(false && "BFS did not converge!");
}


// shortest path (with barriers and multiple destinatins) on unweighted graph.
// The path cannot pass throuh vertices for which mask[v] = true
// The algorithm stops as soon as it reaches of of the destinations
//
template<class Mesh>
CINO_INLINE
void bfs(const Mesh              & m,
         const int                 source,
         const std::set<int>     & dest,
         const std::vector<bool> & mask,
               std::vector<int>  & path)
{
    assert(path.empty());
    assert(mask.size() == (size_t)m.num_vertices());

    int inf_dist = m.num_edges()+1;
    std::vector<int> prev(m.num_vertices(), -1);
    std::vector<int> dist(m.num_vertices(), inf_dist);
    dist[source] = 0;

    std::set<ipair> active_set;
    active_set.insert(std::make_pair(0, source));

    int vid;
    while (!active_set.empty())
    {
        vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (CONTAINS(dest, vid))
        {
            do
            {
                path.push_back(vid);
                vid = prev[vid];
            }
            while(vid != -1);
            return;
        }

        for(int nbr : m.adj_vtx2vtx(vid))
        {
            float tmp = dist[vid] + 1;

            if (mask[nbr]) continue;
            if (dist[nbr] > tmp)
            {
                if (dist[nbr] < inf_dist)
                {
                    auto it = active_set.find(std::make_pair(dist[nbr], nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist[nbr] = tmp;
                prev[nbr]  = vid;
                active_set.insert(std::make_pair(dist[nbr], nbr));
            }
        }
    }
    assert(false && "BFS did not converge!");
}


}
