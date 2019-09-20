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

#include <cinolib/meshes/trimesh.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/bbox.h>

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

// item type T must implement the following methods:
//     Bbox   T::aabb() const;
//     double T::dist_sqrd(const vec3d & p) const;
//
template<typename T>
class Octree
{

    public:

        explicit Octree(const std::vector<T> & items,
                        const uint             max_depth      = 7,
                        const uint             items_per_leaf = 3);

        virtual ~Octree();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void init();
        void add_item(const uint id, OctreeNode *node, const uint depth);
        uint max_items_per_leaf() const;
        uint max_items_per_leaf(const OctreeNode *node, const uint max) const;

        // QUERIES :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        T    nearest_neighbor_item   (const vec3d & p, const bool print_debug_info) const;
        uint nearest_neighbor_item_id(const vec3d & p, const bool print_debug_info) const;

        // ray tracing (first hit)
        // ray tracing (all hits)
        // range queries (spherical, aabb, ...) [useful?]

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    protected:

        // DATA
        OctreeNode       *root = nullptr;
        std::vector<T>    items;
        std::vector<Bbox> aabbs;

        // PARAMETERS
        uint max_depth;
        uint items_per_leaf;

        // STATISTICAL INFO
        uint tree_depth;
        uint num_leaves;

        // SUPPORT STRUCTURES ::::::::::::::::::::::::::::::::::::::::::::::::::::

        struct PrioQueueEntry
        {
            double      dist = inf_double;
            OctreeNode *node = nullptr;
            int         id   = -1;
        };
        struct Greater
        {
            bool operator()(const PrioQueueEntry & obj1,
                            const PrioQueueEntry & obj2)
            {
                return obj1.dist > obj2.dist;
            }
        };
        typedef std::priority_queue<PrioQueueEntry,std::vector<PrioQueueEntry>,Greater> PrioQueue;
};

}

#ifndef  CINO_STATIC_LIB
#include "octree.cpp"
#endif

#endif // CINO_OCTREE_H
