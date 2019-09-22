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
#ifndef CINO_OCTREE_H
#define CINO_OCTREE_H

#include <cinolib/meshes/abstract_mesh.h>
#include <queue>

namespace cinolib
{

class OctreeNode
{
    public:
        OctreeNode(const OctreeNode * father, const Bbox & bbox) : father(father), bbox(bbox) {}
       ~OctreeNode();
        const OctreeNode *father;
        OctreeNode       *children[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        bool              is_inner = false;
        Bbox              bbox;
        std::vector<uint> item_ids; // index Octree::items, avoiding to store a copy of the same object multiple times in each node it appears
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// item type T must implement the following three methods:
//     Bbox  T::aabb() const;
//     vec3d T::point_closest_to(const vec3d & p) const;
//
template<typename T>
class Octree
{

    public:

        explicit Octree(const uint max_depth      = 7,
                        const uint items_per_leaf = 3);

        explicit Octree(const std::vector<T> & items,
                        const uint             max_depth      = 7,
                        const uint             items_per_leaf = 3);

        virtual ~Octree();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void build();
        void build_item(const uint id, OctreeNode *node, const uint depth);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        template<class M, class V, class E, class P>
        void build_from_mesh_polys(const AbstractMesh<M,V,E,P> & m)
        {
            assert(items.empty());
            items.reserve(m.num_polys());
            for(uint pid=0; pid<m.num_polys(); ++pid)
            {
                // T here is expected to be either Triangle (for surface meshes) or Tetrahedron (for volume meshes)
                items.push_back(T(m.poly_verts(pid)));
            }
            build();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        template<class M, class V, class E, class P>
        void build_from_mesh_edges(const AbstractMesh<M,V,E,P> & m)
        {
            assert(items.empty());
            items.reserve(m.num_edges());
            for(uint eid=0; eid<m.num_edges(); ++eid)
            {
                // T here is expected to be Segment
                items.push_back(T(m.edge_verts(eid)));
            }
            build();
        }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint max_items_per_leaf() const;
        uint max_items_per_leaf(const OctreeNode *node, const uint max) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void debug_mode(const bool & b);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void print_query_info(const std::string & s,
                              const double        t,
                              const uint          aabb_queries,
                              const uint          item_queries) const;

        // QUERIES :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // returns pos, id and distance of the item that is closest to query point p
        const T & closest_point(const vec3d & p, uint & id, vec3d & pos, double & dist) const;

        // returns respectively the first item and the full list of items containing query point p
        bool contains(const vec3d & p, uint & id) const;
        bool contains(const vec3d & p, std::unordered_set<uint> & ids) const;

        // returns the item/id in the octree nearest to ball (p,radius)
        //T    nearest_neighbor   (const vec3d & p, const float radius) const;
        //uint nearest_neighbor_id(const vec3d & p, const float radius) const;

        // returns the item/id in the octree containing point p (if there are more, it will return only one of them)
        //T    contains   (const vec3d & p) const;
        //int  contains_id(const vec3d & p) const; // -1: no item found

        // returns the item/id in the octree that firstly intersect ray (p,dir)
        //T    ray_first_hit   (const vec3d & p, const vec3d & dir) const;
        //int  ray_first_hit_id(const vec3d & p, const vec3d & dir) const;  // -1: no item found

        // returns all the items/ids in the octree that intersect ray (p,dir)
        //std::vector<T>    ray_hits   (const vec3d & p, const vec3d & dir) const;
        //std::vector<uint> ray_hits_id(const vec3d & p, const vec3d & dir) const;  // -1: no item found

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    protected:

        // DATA
        OctreeNode       *root = nullptr;
        std::vector<T>    items;
        std::vector<Bbox> aabbs;            // per item axis aligned bunding boxes
        uint              max_depth;        // maximum allowed depth of the tree
        uint              items_per_leaf;   // prescribed number of items per leaf (can't go deeper than max_depth anyways)
        uint              tree_depth;       // actual depth of the tree
        uint              num_leaves;
        bool              print_debug_info = false;

        // SUPPORT STRUCTURES ::::::::::::::::::::::::::::::::::::::::::::::::::::

        struct Obj
        {
            double      dist = inf_double;
            OctreeNode *node = nullptr;
            int         id   = -1;
            vec3d       pos; // closest point
        };
        struct Greater
        {
            bool operator()(const Obj & obj1,
                            const Obj & obj2)
            {
                return obj1.dist > obj2.dist;
            }
        };
        typedef std::priority_queue<Obj,std::vector<Obj>,Greater> PrioQueue;
};

}

#ifndef  CINO_STATIC_LIB
#include "octree.cpp"
#endif

#endif // CINO_OCTREE_H
