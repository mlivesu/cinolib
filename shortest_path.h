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
#ifndef CINO_SHORTEST_PATH_H
#define CINO_SHORTEST_PATH_H

#include <cinolib/cinolib.h>

#include <float.h>
#include <vector>
#include <set>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
void dijkstra_shortest_path(const Mesh           & m,
                            const int                 source,
                            const int                 dest,
                                  std::vector<int>  & path)
{
    using namespace std;
    assert(path.empty());

    vector<int>   prev(m.num_vertices(), -1);
    vector<float> dist(m.num_vertices(), FLT_MAX);
    dist[source] = 0;

    typedef pair<int,int> item;
    set<item> active_set;

    active_set.insert(make_pair(0,source));

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

        vector<int> nbrs = m.adj_vtx2vtx(vid);
        for(size_t i=0; i<nbrs.size(); ++i)
        {
            int   nbr = nbrs[i];
            float tmp = dist[vid] + 1;

            if (dist[nbr] > tmp)
            {
                if (dist[nbr] < FLT_MAX)
                {
                    set<item>::iterator it = active_set.find(make_pair(dist[nbr], nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist[nbr] = tmp;
                prev[nbr]  = vid;
                active_set.insert(make_pair(dist[nbr], nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge - weird!");
}


template<class Mesh>
CINO_INLINE
void dijkstra_shortest_path(const Mesh           & m,
                            const int                 source,
                            const int                 dest,
                            const std::vector<bool> & mask,
                                  std::vector<int>  & path)
{
    using namespace std;
    assert(path.empty());

    vector<int>   prev(m.num_vertices(), -1);
    vector<float> dist(m.num_vertices(), FLT_MAX);
    dist[source] = 0;

    typedef pair<int,int> item;
    set<item> active_set;

    active_set.insert(make_pair(0,source));

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

        vector<int> nbrs = m.adj_vtx2vtx(vid);
        for(size_t i=0; i<nbrs.size(); ++i)
        {
            int   nbr = nbrs[i];
            float tmp = dist[vid] + 1;

            if (mask[nbr]) continue;

            if (dist[nbr] > tmp)
            {
                if (dist[nbr] < FLT_MAX)
                {
                    set<item>::iterator it = active_set.find(make_pair(dist[nbr], nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist[nbr] = tmp;
                prev[nbr]  = vid;
                active_set.insert(make_pair(dist[nbr], nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge - weird!");
}



template<class Mesh>
CINO_INLINE
void dijkstra_shortest_path(const Mesh              & m,
                            const int                 source,
                            const std::set<int>     & dest,
                            const std::vector<bool> & mask,
                                  std::vector<int>  & path)
{
    using namespace std;
    assert(path.empty());
    assert(mask.size() == m.num_vertices());

    vector<int>   prev(m.num_vertices(), -1);
    vector<float> dist(m.num_vertices(), FLT_MAX);
    dist[source] = 0;

    typedef pair<int,int> item;
    set<item> active_set;

    active_set.insert(make_pair(0,source));

    int vid;

    while (!active_set.empty())
    {
        vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (CONTAINS(dest,vid))
        {
            do
            {
                path.push_back(vid);
                vid = prev[vid];
            }
            while(vid != -1);
            return;
        }

        vector<int> nbrs = m.adj_vtx2vtx(vid);
        for(size_t i=0; i<nbrs.size(); ++i)
        {
            int   nbr = nbrs[i];
            float tmp = dist[vid] + 1;

            if (mask[nbr]) continue;

            if (dist[nbr] > tmp)
            {
                if (dist[nbr] < FLT_MAX)
                {
                    set<item>::iterator it = active_set.find(make_pair(dist[nbr], nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist[nbr] = tmp;
                prev[nbr]  = vid;
                active_set.insert(make_pair(dist[nbr], nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge - weird!");
}

}

#endif // CINO_SHORTEST_PATH_H
