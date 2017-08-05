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
#include <cinolib/bfs.h>
#include <queue>

namespace cinolib
{


// floodfill (for general graphs - i.e. not meshes)
//
CINO_INLINE
void bfs_exahustive(const std::vector<std::vector<uint>> & nodes_adjacency,
                    const uint                             source,
                          std::set<uint>                 & visited)
{
    assert(visited.empty());

    std::set<uint> active_set;
    active_set.insert(source);

    uint vid;
    while (!active_set.empty())
    {
        vid = *active_set.begin();
        active_set.erase(active_set.begin());

        visited.insert(vid);

        for(uint nbr : nodes_adjacency.at(vid))
        {
            if (DOES_NOT_CONTAIN(visited,nbr))
            {
                active_set.insert(nbr);
            }
        }
    }
}



template<class Mesh>
CINO_INLINE
void bfs_exahustive(const Mesh           & m,
                    const uint             source,
                          std::set<uint> & visited)
{
    assert(visited.empty());

    std::set<uint> active_set;
    active_set.insert(source);

    uint vid;
    while (!active_set.empty())
    {
        vid = *active_set.begin();
        active_set.erase(active_set.begin());

        visited.insert(vid);

        for(uint nbr : m.adj_v2v(vid))
        {
            if (DOES_NOT_CONTAIN(visited,nbr))
            {
                active_set.insert(nbr);
            }
        }
    }
}


// floodfill (with barriers) on the dual mesh (faces instead of vertices)
// The path cannot pass through faces for which mask[f] = true
//
template<class Mesh>
CINO_INLINE
void bfs_exahustive_on_dual(const Mesh              & m,
                            const uint                source,
                            const std::vector<bool> & mask,
                                  std::set<uint>    & visited)
{
    assert(visited.empty());

    std::set<uint> active_set;
    active_set.insert(source);

    uint cid;
    while (!active_set.empty())
    {
        cid = *active_set.begin();
        active_set.erase(active_set.begin());

        visited.insert(cid);

        for(uint nbr : m.adj_poly2poly(cid))
        {
            if (mask.at(nbr)) continue;
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
void bfs(const Mesh              & m,
         const uint                source,
         const uint                dest,
               std::vector<uint> & path)

{
    assert(path.empty());

    uint inf_dist = m.num_edges()+1;
    std::vector<int> prev(m.num_verts(), -1);
    std::vector<uint> dist(m.num_verts(), inf_dist);
    dist[source] = 0;

    std::set<ipair> active_set;
    active_set.insert(std::make_pair(0,source));

    int vid;
    while (!active_set.empty())
    {
        vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (vid == (int)dest)
        {
            do
            {
                path.push_back(vid);
                vid = prev[vid];
            }
            while(vid != -1);
            return;
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            float tmp = dist[vid] + 1;

            if (dist[nbr] > tmp)
            {
                if (dist[nbr] < inf_dist)
                {
                    auto it = active_set.find(std::make_pair(dist[nbr],nbr));
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
         const uint                source,
         const uint                dest,
         const std::vector<bool> & mask,
               std::vector<uint> & path)
{
    assert(path.empty());
    assert(mask.size() == m.num_verts());

    uint inf_dist = m.num_edges()+1;
    std::vector<int>  prev(m.num_verts(), -1);
    std::vector<uint> dist(m.num_verts(), inf_dist);
    dist[source] = 0;

    std::set<ipair> active_set;
    active_set.insert(std::make_pair(0,source));

    int vid;
    while (!active_set.empty())
    {
        vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if ((int)dest == vid)
        {
            do
            {
                path.push_back(vid);
                vid = prev[vid];
            }
            while(vid != -1);
            return;
        }

        for(uint nbr : m.adj_v2v(vid))
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
         const uint                source,
         const std::set<uint>    & dest,
         const std::vector<bool> & mask,
               std::vector<uint> & path)
{
    assert(path.empty());
    assert(mask.size() == (size_t)m.num_verts());

    uint inf_dist = m.num_edges()+1;
    std::vector<int>  prev(m.num_verts(), -1);
    std::vector<uint> dist(m.num_verts(), inf_dist);
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

        for(uint nbr : m.adj_v2v(vid))
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


template<class Mesh>
CINO_INLINE
void bfs_exahustive_on_dual(const Mesh                & m,
                            const uint                  source,
                                  std::vector<double> & dist) // elem id + dist
{
    dist = std::vector<double>(m.num_polys(), FLT_MAX);
    std::queue<uint> q;

    q.push(source);
    dist.at(source) = 0.0;

    while(!q.empty())
    {
        uint fid = q.front();
        q.pop();

        for(const uint & nbr : m.adj_poly2poly(fid))
        {
            double c = dist.at(fid) + m.poly_centroid(fid).dist(m.poly_centroid(nbr));
            if (dist.at(nbr) > c)
            {
                dist.at(nbr) = c;
                q.push(nbr);
            }
        }
    }
}


}
