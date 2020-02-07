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
#include <cinolib/dijkstra.h>
#include <cinolib/min_max_inf.h>
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
                               std::vector<double>   & dist)
{
    dist = std::vector<double>(m.num_verts(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        for(uint nbr : m.adj_v2v(vid))
        {
            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive(const AbstractMesh<M,V,E,P> & m,
                         const std::vector<uint>     & sources,
                               std::vector<double>   & dist)
{
    dist = std::vector<double>(m.num_verts(), inf_double);
    for(uint vid : sources) dist.at(vid) = 0.0;

    std::set<std::pair<double,uint>> q;
    for(uint vid : sources) q.insert(std::make_pair(0.0,vid));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        for(uint nbr : m.adj_v2v(vid))
        {
            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void dijkstra_exhaustive_srf_only(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                                  const std::vector<uint>                 & sources,
                                        std::vector<double>               & dist)
{
    dist = std::vector<double>(m.num_verts(), inf_double);
    for(uint vid : sources) dist.at(vid) = 0.0;

    std::set<std::pair<double,uint>> q;
    for(uint vid : sources) q.insert(std::make_pair(0.0,vid));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        for(uint eid : m.adj_v2e(vid))
        {
            if(m.edge_is_on_srf(eid))
            {
                uint   nbr      = m.vert_opposite_to(eid,vid);
                double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

                if(dist.at(nbr) > new_dist)
                {
                    if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                    {
                        auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                        assert(it!=q.end());
                        q.erase(it);
                    }
                    dist.at(nbr) = new_dist;
                    q.insert(std::make_pair(new_dist,nbr));
                }
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra(const AbstractMesh<M,V,E,P> & m,
                const uint                    source,
                const uint                    dest,
                      std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_verts(), -1);

    std::vector<double> dist(m.num_verts(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(dest);
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Shortest path (with barriers). The path cannot
// pass throuh vertices for which mask[v] = true
//
template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra(const AbstractMesh<M,V,E,P> & m,
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

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(dest);
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            if(mask.at(nbr)) continue;

            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Shortest path (with barriers on edges). The path cannot
// pass throuh edges for which mask[e] = true
//
template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_mask_on_edges(const AbstractMesh<M,V,E,P> & m,
                              const uint                    source,
                              const uint                    dest,
                              const std::vector<bool>     & mask, // if mask[e] = true, path cannot pass through edge e
                                    std::vector<uint>     & path)
{
    path.clear();
    assert(mask.size() == m.num_edges());

    std::vector<int> prev(m.num_verts(), -1);

    std::vector<double> dist(m.num_verts(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(dest);
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            int eid = m.edge_id(vid,nbr);
            assert(eid>=0);

            if(mask.at(eid)) continue;

            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Shortest path (with barriers and multiple destinations). The path
// cannot pass throuh vertices for which mask[v] = true. The algorithm
// stops as soon as it reaches one of the destinations
//
template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra(const AbstractMesh<M,V,E,P> & m,
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

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(CONTAINS(dest,vid))
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(vid);
        }

        for(uint nbr : m.adj_v2v(vid))
        {
            if(mask.at(nbr)) continue;

            double new_dist = dist.at(vid) + m.vert(vid).dist(m.vert(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive_on_dual(const AbstractMesh<M,V,E,P> & m,
                                 const uint                    source,
                                       std::vector<double>   & dist)
{
    dist = std::vector<double>(m.num_polys(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        for(uint nbr : m.adj_p2p(vid))
        {
            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dijkstra_exhaustive_on_dual(const AbstractMesh<M,V,E,P> & m,
                                 const std::vector<uint>     & sources,
                                       std::vector<double>   & dist)
{
    dist = std::vector<double>(m.num_polys(), inf_double);

    std::set<std::pair<double,uint>> q;

    for(uint s : sources)
    {
        dist.at(s) = 0.0;
        q.insert(std::make_pair(0.0,s));
    }

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        for(uint nbr : m.adj_p2p(vid))
        {
            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                        const uint                    source,
                        const uint                    dest,
                              std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_polys(), -1);

    std::vector<double> dist(m.num_polys(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(dest);
        }

        for(uint nbr : m.adj_p2p(vid))
        {
            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                        const uint                    source,
                        const uint                    dest,
                        const std::vector<bool>     & mask,
                              std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_polys(), -1);

    std::vector<double> dist(m.num_polys(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(vid==dest)
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(dest);
        }

        for(uint nbr : m.adj_p2p(vid))
        {
            if(mask.at(nbr)) continue;

            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                        const uint                    source,
                        const std::set<uint>        & dest,
                        const std::vector<bool>     & mask,
                              std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_polys(), -1);

    std::vector<double> dist(m.num_polys(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(CONTAINS(dest,vid))
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(vid);
        }

        for(uint nbr : m.adj_p2p(vid))
        {
            if(mask.at(nbr)) continue;

            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double dijkstra_on_dual(const AbstractMesh<M,V,E,P> & m,
                        const uint                    source,
                        const std::set<uint>        & dest,
                              std::vector<uint>     & path)
{
    path.clear();

    std::vector<int> prev(m.num_polys(), -1);

    std::vector<double> dist(m.num_polys(), inf_double);
    dist.at(source) = 0.0;

    std::set<std::pair<double,uint>> q;
    q.insert(std::make_pair(0.0,source));

    while(!q.empty())
    {
        uint vid = q.begin()->second;
        q.erase(q.begin());

        if(CONTAINS(dest,vid))
        {
            int tmp = vid;
            do { path.push_back(tmp); tmp = prev.at(tmp); } while (tmp != -1);
            std::reverse(path.begin(), path.end());
            return dist.at(vid);
        }

        for(uint nbr : m.adj_p2p(vid))
        {
            double new_dist = dist.at(vid) + m.poly_centroid(vid).dist(m.poly_centroid(nbr));

            if(dist.at(nbr) > new_dist)
            {
                if(dist.at(nbr) < inf_double) // otherwise it won't be found (one order of magnitude faster than initializing the queue with all the elements with inf dist)
                {
                    auto it = q.find(std::make_pair(dist.at(nbr),nbr));
                    assert(it!=q.end());
                    q.erase(it);
                }
                dist.at(nbr) = new_dist;
                prev.at(nbr) = vid;
                q.insert(std::make_pair(new_dist,nbr));
            }
        }
    }
    assert(false && "Dijkstra did not converge!");
    return 0.0;
}

}
