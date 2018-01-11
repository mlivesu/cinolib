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
#include <cinolib/dijkstra.h>
#include <cinolib/inf.h>
#include <cinolib/stl_container_utilities.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// LITTLE NOTE ON MY DIJKSTRA IMPLEMENTATIONS: why std::set and not
// std::unordered_set or std::priority_queue?
//
// The reason why I am using std::set is that Dijkstra requires priority
// update, which is supported by none of the STL containers. Options are:
//
//  1) remove an element and re-add it with updated priority. In this
//     case std::set has the best remove performances
//
//  2) leave "dead" copies of an element in the container, and just add a
//     new copy each time its priority needs an update. Regardless the waste
//     of memory, which is an issue by itself, in this case at each iteration
//     you need a mechanism that allows you to discard the current element
//     if it is not the one with the most recent priority...
//
// I therefore decided to go for (1).
// See also:
// https://stackoverflow.com/questions/649640/how-to-do-an-efficient-priority-update-in-stl-priority-queue

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive(const AbstractMesh<M,V,E,P> & m,
                         const uint                    source,
                               std::vector<double>   & distances)
{
    distances = std::vector<double>(m.num_verts(), inf_double);
    distances.at(source) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    active_set.insert(std::make_pair(0.0,source));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        for(uint nbr : m.adj_v2v(vid))
        {
            double new_dist = distances.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if (distances.at(nbr) > new_dist)
            {
                if (distances.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(distances.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                distances.at(nbr) = new_dist;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive(const AbstractMesh<M,V,E,P> & m,
                         const std::vector<uint>     & sources,
                               std::vector<double>   & distances)
{
    distances = std::vector<double>(m.num_verts(), inf_double);
    for(uint vid : sources) distances.at(vid) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    for(uint vid : sources) active_set.insert(std::make_pair(0.0,vid));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        for(uint nbr : m.adj_v2v(vid))
        {
            double new_dist = distances.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if (distances.at(nbr) > new_dist)
            {
                if (distances.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(distances.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                distances.at(nbr) = new_dist;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra(const AbstractMesh<M,V,E,P> & m,
              const uint                    source,
              const uint                    dest,
                    std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_verts(), -1);

    std::vector<double> dist(m.num_verts(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    active_set.insert(std::make_pair(0.0,source));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return;
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if (dist.at(nbr) > new_dist)
            {
                if (dist.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Shortest path (with barriers). The path cannot
// pass throuh vertices for which mask[v] = true
//
template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra(const AbstractMesh<M,V,E,P> & m,
              const uint                    source,
              const uint                    dest,
              const std::vector<bool>     & mask,
                    std::vector<uint>     & path)
{
    path.clear();
    assert(mask.size() == m.num_verts());

    std::vector<int> prev(m.num_verts(), -1);

    std::vector<double> dist(m.num_verts(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    active_set.insert(std::make_pair(0.0,source));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return;
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            if (mask.at(nbr)) continue;

            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if (dist.at(nbr) > new_dist)
            {
                if (dist.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Shortest path (with barriers and multiple destinations). The path
// cannot pass throuh vertices for which mask[v] = true. The algorithm
// stops as soon as it reaches one of the destinations
//
template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra(const AbstractMesh<M,V,E,P> & m,
              const uint                    source,
              const std::set<uint>        & dest,
              const std::vector<bool>     & mask,
                    std::vector<uint>     & path)
{
    path.clear();
    assert(mask.size() == m.num_verts());

    std::vector<int> prev(m.num_verts(), -1);

    std::vector<double> dist(m.num_verts(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    active_set.insert(std::make_pair(0.0,source));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (CONTAINS(dest,vid))
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return;
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            if (mask.at(nbr)) continue;

            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if (dist.at(nbr) > new_dist)
            {
                if (dist.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive_on_dual(const AbstractMesh<M,V,E,P> & m,
                                 const uint                    source,
                                       std::vector<double>   & distances)
{
    distances = std::vector<double>(m.num_polys(), inf_double);
    distances.at(source) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    active_set.insert(std::make_pair(0.0,source));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        for(uint nbr : m.adj_p2p(vid))
        {
            double new_dist = distances.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if (distances.at(nbr) > new_dist)
            {
                if (distances.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(distances.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                distances.at(nbr) = new_dist;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive_on_dual(const AbstractMesh<M,V,E,P> & m,
                                 const std::vector<uint>     & sources,
                                       std::vector<double>   & distances)
{
    distances = std::vector<double>(m.num_polys(), inf_double);

    std::set<std::pair<double,uint>> active_set;

    for(uint s : sources)
    {
        distances.at(s) = 0.0;
        active_set.insert(std::make_pair(0.0,s));
    }

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        for(uint nbr : m.adj_p2p(vid))
        {
            double new_dist = distances.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if (distances.at(nbr) > new_dist)
            {
                if (distances.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(distances.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                distances.at(nbr) = new_dist;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                      const uint                    source,
                      const uint                    dest,
                            std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_polys(), -1);

    std::vector<double> dist(m.num_polys(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    active_set.insert(std::make_pair(0.0,source));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return;
        }

        for(uint nbr : m.adj_p2p(vid))
        {
            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if (dist.at(nbr) > new_dist)
            {
                if (dist.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                      const uint                    source,
                      const uint                    dest,
                      const std::vector<bool>     & mask,
                            std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_polys(), -1);

    std::vector<double> dist(m.num_polys(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    active_set.insert(std::make_pair(0.0,source));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return;
        }

        for(uint nbr : m.adj_p2p(vid))
        {
            if (mask.at(nbr)) continue;

            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if (dist.at(nbr) > new_dist)
            {
                if (dist.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                      const uint                    source,
                      const std::set<uint>        & dest,
                      const std::vector<bool>     & mask,
                            std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_polys(), -1);

    std::vector<double> dist(m.num_polys(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> active_set;
    active_set.insert(std::make_pair(0.0,source));

    while(!active_set.empty())
    {
        uint vid = active_set.begin()->second;
        active_set.erase(active_set.begin());

        if (CONTAINS(dest,vid))
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return;
        }

        for(uint nbr : m.adj_p2p(vid))
        {
            if (mask.at(nbr)) continue;

            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if (dist.at(nbr) > new_dist)
            {
                if (dist.at(nbr) < inf_double) // otherwise it won't be found
                {
                    auto it = active_set.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=active_set.end());
                    active_set.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                active_set.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
}

}
