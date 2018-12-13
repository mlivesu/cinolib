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
#include <cinolib/bfs.h>
//
#include <cinolib/stl_container_utilities.h>
#include <queue>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void bfs(const std::vector<std::vector<uint>> & nodes_adjacency,
         const uint                             source,
               std::unordered_set<uint>       & visited)
{
    visited.clear();

    std::queue<uint> q;
    q.push(source);

    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        visited.insert(vid);

        for(uint nbr : nodes_adjacency.at(vid))
        {
            if (DOES_NOT_CONTAIN(visited,nbr))
            {
                q.push(nbr);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void bfs(const AbstractMesh<M,V,E,P>    & m,
         const uint                       source,
               std::unordered_set<uint> & visited)
{
    visited.clear();
    visited.insert(source);

    std::queue<uint> q;
    q.push(source);

    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        for(uint nbr : m.adj_v2v(vid))
        {
            if (DOES_NOT_CONTAIN(visited,nbr))
            {
                visited.insert(nbr);
                q.push(nbr);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void bfs(const AbstractMesh<M,V,E,P>    & m,
         const uint                       source,
         const std::vector<bool>        & mask, // if mask[vid] = true, path cannot pass through vertex vid
               std::unordered_set<uint> & visited)
{
    visited.clear();
    visited.insert(source);

    std::queue<uint> q;
    q.push(source);

    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        for(uint nbr : m.adj_v2v(vid))
        {
            if (!mask.at(nbr) && DOES_NOT_CONTAIN(visited,nbr))
            {
                visited.insert(nbr);
                q.push(nbr);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void bfs_srf_only(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                  const uint                                source,
                  const std::vector<bool>                 & mask, // if mask[vid] = true, path cannot pass through vertex vid
                  std::unordered_set<uint>                & visited)
{
    visited.clear();
    visited.insert(source);

    std::queue<uint> q;
    q.push(source);

    while(!q.empty())
    {
        uint vid = q.front();
        q.pop();

        for(uint eid : m.adj_v2e(vid))
        {
            if(m.edge_is_on_srf(eid))
            {
                uint nbr = m.vert_opposite_to(eid,vid);
                if (!mask.at(nbr) && DOES_NOT_CONTAIN(visited,nbr))
                {
                    visited.insert(nbr);
                    q.push(nbr);
                }
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void bfs_on_dual(const AbstractMesh<M,V,E,P>    & m,
                 const uint                       source,
                 const std::vector<bool>        & mask, // if mask[p] = true, path cannot pass through it
                       std::unordered_set<uint> & visited)
{
    visited.clear();
    visited.insert(source);

    std::queue<uint> q;
    q.push(source);

    while(!q.empty())
    {
        uint pid = q.front();
        q.pop();

        for(uint nbr : m.adj_p2p(pid))
        {
            if (!mask.at(nbr) && DOES_NOT_CONTAIN(visited,nbr))
            {
                visited.insert(nbr);
                q.push(nbr);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// floodfill (with barriers on edges) [just for surface meshes]
//
template<class M, class V, class E, class P>
CINO_INLINE
void bfs_on_dual_w_edge_barriers(const AbstractPolygonMesh<M,V,E,P> & m,
                                 const uint                           source,
                                 const std::vector<bool>            & mask_edges, // if mask[e] = true, bfs cannot expand through edge e
                                 std::unordered_set<uint>           & visited)
{
    visited.clear();
    visited.insert(source);

    std::queue<uint> q;
    q.push(source);

    while(!q.empty())
    {
        uint pid = q.front();
        q.pop();

        for(uint nbr : m.adj_p2p(pid))
        {
            uint eid = m.edge_shared(pid,nbr);
            if (!mask_edges.at(eid) && DOES_NOT_CONTAIN(visited,nbr))
            {
                visited.insert(nbr);
                q.push(nbr);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// floodfill (with barriers on faces)
//
template<class M, class V, class E, class F, class P>
CINO_INLINE
void bfs_on_dual_w_face_barriers(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                                 const uint                                source,
                                 const std::vector<bool>                 & mask_faces, // if mask[f] = true, bfs cannot expand through face f
                                 std::unordered_set<uint>                & visited)
{
    visited.clear();
    visited.insert(source);

    std::queue<uint> q;
    q.push(source);

    while(!q.empty())
    {
        uint pid = q.front();
        q.pop();

        for(uint fid : m.adj_p2f(pid))
        {
            int nbr = (m.poly_adj_through_face(pid,fid));
            if (nbr>=0 && !mask_faces.at(fid) && DOES_NOT_CONTAIN(visited,nbr))
            {
                visited.insert(nbr);
                q.push(nbr);
            }
        }
    }
}

}
