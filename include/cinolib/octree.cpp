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
#include <cinolib/octree.h>
#include <cinolib/how_many_seconds.h>
#include <stack>

namespace cinolib
{

CINO_INLINE
OctreeNode::~OctreeNode()
{
    // "in a tree's node destructor, you only need to destroy the children pointers that are manually
    //  allocated by you. You don't need to worry about the deallocation of the node itself."
    //  https://stackoverflow.com/questions/34170164/destructor-for-binary-search-tree
    for(int i=0; i<8; ++i) delete children[i];
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Octree::Octree(const uint max_depth,
               const uint items_per_leaf)
: max_depth(max_depth)
, items_per_leaf(items_per_leaf)
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
Octree::~Octree()
{
    // delete Octree
    delete root;

    // delete item list
    while(!items.empty())
    {
        delete items.back();
        items.pop_back();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::build()
{
    typedef std::chrono::high_resolution_clock Time;
    Time::time_point t0 = Time::now();

    // make AABBs for each item
    aabbs.reserve(items.size());
    for(const auto item : items) aabbs.push_back(item->aabb());
    assert(items.size() == aabbs.size());

    // build the tree root
    assert(root==nullptr);
    root = new OctreeNode(nullptr, Bbox(aabbs, 1.5)); // enlarge it a bit to make sure queries don't fall outside

    tree_depth = 1;
    num_leaves = 1;

    // populate the tree
    for(uint id=0; id<items.size(); ++id) build_item(id, root, 0);

    Time::time_point t1 = Time::now();
    double t = how_many_seconds(t0,t1);

    std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
    std::cout << "Octree created (" << t << "s)                      " << std::endl;
    std::cout << "#Items                   : " << items.size()         << std::endl;
    std::cout << "#Leaves                  : " << num_leaves           << std::endl;
    std::cout << "Max depth                : " << max_depth            << std::endl;
    std::cout << "Depth                    : " << tree_depth           << std::endl;
    std::cout << "Prescribed items per leaf: " << items_per_leaf       << std::endl;
    std::cout << "Max items per leaf       : " << max_items_per_leaf() << std::endl;
    std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::build_item(const uint id, OctreeNode * node, const uint depth)
{
    assert(node->bbox.intersects_box(aabbs.at(id)));

    if(node->is_inner)
    {
        assert(node->item_ids.empty());
        for(int i=0; i<8; ++i)
        {
            assert(node->children[i]!=nullptr);
            if(node->children[i]->bbox.intersects_box(aabbs.at(id)))
            {
                build_item(id, node->children[i], depth+1);
            }
        }
    }
    else
    {
        node->item_ids.push_back(id);

        // if the node contains more elements than allowed, and the depth
        // of the tree is lower than max depth: split the node into 8 octants
        // and move all its items downwards
        //
        if(node->item_ids.size()>items_per_leaf && depth<max_depth)
        {
            node->is_inner = true;

            auto items_to_move_down = node->item_ids;
            node->item_ids.clear();

            // create children octants
            vec3d min = node->bbox.min;
            vec3d max = node->bbox.max;
            vec3d avg = node->bbox.center();
            node->children[0] = new OctreeNode(node, Bbox(vec3d(min[0], min[1], min[2]), vec3d(avg[0], avg[1], avg[2])));
            node->children[1] = new OctreeNode(node, Bbox(vec3d(avg[0], min[1], min[2]), vec3d(max[0], avg[1], avg[2])));
            node->children[2] = new OctreeNode(node, Bbox(vec3d(avg[0], avg[1], min[2]), vec3d(max[0], max[1], avg[2])));
            node->children[3] = new OctreeNode(node, Bbox(vec3d(min[0], avg[1], min[2]), vec3d(avg[0], max[1], avg[2])));
            node->children[4] = new OctreeNode(node, Bbox(vec3d(min[0], min[1], avg[2]), vec3d(avg[0], avg[1], max[2])));
            node->children[5] = new OctreeNode(node, Bbox(vec3d(avg[0], min[1], avg[2]), vec3d(max[0], avg[1], max[2])));
            node->children[6] = new OctreeNode(node, Bbox(vec3d(avg[0], avg[1], avg[2]), vec3d(max[0], max[1], max[2])));
            node->children[7] = new OctreeNode(node, Bbox(vec3d(min[0], avg[1], avg[2]), vec3d(avg[0], max[1], max[2])));

            // mode items downwards in the tree
            // NOTE: items that span across multiple octants will be added to each node they intersect)
            uint d_plus_one = depth+1;
            for(uint item : items_to_move_down)
            {
                bool found_octant = false;
                for(int i=0; i<8; ++i)
                {
                    assert(node->children[i]!=nullptr);
                    if(node->children[i]->bbox.intersects_box(aabbs.at(item)))
                    {
                        build_item(item, node->children[i], d_plus_one);
                        found_octant = true;
                    }
                }
                assert(found_octant);
            }

            // remove items from current node            
            num_leaves += 7; // 8 children minus the current node
            tree_depth = std::max(depth, d_plus_one);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Octree::max_items_per_leaf() const
{
    return max_items_per_leaf(root, 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Octree::max_items_per_leaf(const OctreeNode * node, const uint max) const
{
    if(node->is_inner)
    {
        uint tmp[8];
        for(int i=0; i<8; ++i)
        {
            tmp[i] = max_items_per_leaf(node->children[i], max);
        }
        return *std::max_element(tmp, tmp+8);
    }
    else
    {
        return std::max((uint)node->item_ids.size(), max);
    }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::debug_mode(const bool b)
{
    print_debug_info = b;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::print_query_info(const std::string & s,
                                 const double        t,
                                 const uint          aabb_queries,
                                 const uint          item_queries) const
{
    std::cout << s << "\n\t" << t  << " seconds\n\t"
              << aabb_queries << " AABB queries\n\t"
              << item_queries << " item queries" << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://stackoverflow.com/questions/41306122/nearest-neighbor-search-in-octree
CINO_INLINE
void Octree::closest_point(const vec3d  & p,          // query point
                                 uint   & id,         // id of the item T closest to p
                                 vec3d  & pos,        // point in T closest to p
                                 double & dist) const // distance between pos and p
{
    typedef std::chrono::high_resolution_clock Time;
    Time::time_point t0 = Time::now();

    Obj obj;
    obj.node = root;
    obj.dist = root->bbox.dist_sqrd(p);

    PrioQueue q;
    q.push(obj);

    uint aabb_queries = 1;
    uint item_queries = 0;

    while(q.top().node->is_inner)
    {
        Obj obj = q.top();
        q.pop();

        for(int i=0; i<8; ++i)
        {
            OctreeNode *child = obj.node->children[i];
            if(child->is_inner)
            {
                Obj obj;
                obj.node = child;
                obj.dist = child->bbox.dist_sqrd(p);
                q.push(obj);
                if(print_debug_info) ++aabb_queries;
            }
            else
            {
                for(uint id : child->item_ids)
                {
                    Obj obj;
                    obj.node = child;
                    obj.id   = id;
                    obj.pos  = items.at(id)->point_closest_to(p);
                    obj.dist = obj.pos.dist_squared(p);
                    q.push(obj);
                    if(print_debug_info) ++item_queries;
                }
            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        print_query_info("Closest point query", how_many_seconds(t0,t1), aabb_queries, item_queries);
    }

    assert(q.top().id>=0);
    id   = q.top().id;
    pos  = q.top().pos;
    dist = q.top().dist;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Octree::contains(const vec3d & p, uint & id, const double eps) const
{
    typedef std::chrono::high_resolution_clock Time;
    Time::time_point t0 = Time::now();

    uint aabb_queries = 0;
    uint item_queries = 0;

    std::stack<OctreeNode*> lifo;
    lifo.push(root);

    while(!lifo.empty())
    {
        OctreeNode *node = lifo.top();
        lifo.pop();
        assert(node->bbox.contains(p,eps));

        if(node->is_inner)
        {
            for(int i=0; i<8; ++i)
            {
                if(print_debug_info) ++aabb_queries;
                if(node->children[i]->bbox.contains(p,eps)) lifo.push(node->children[i]);
            }
        }
        else
        {
            for(uint it_id : node->item_ids)
            {
                if(print_debug_info) ++item_queries;
                if(items.at(it_id)->contains(p,eps))
                {
                    id = it_id;
                    if(print_debug_info)
                    {
                        Time::time_point t1 = Time::now();
                        print_query_info("Contains query (first item)", how_many_seconds(t0,t1), aabb_queries, item_queries);
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Octree::contains(const vec3d & p, std::unordered_set<uint> & ids, const double eps) const
{
    typedef std::chrono::high_resolution_clock Time;
    Time::time_point t0 = Time::now();

    uint aabb_queries = 0;
    uint item_queries = 0;

    ids.clear();

    std::stack<OctreeNode*> lifo;
    lifo.push(root);

    while(!lifo.empty())
    {
        OctreeNode *node = lifo.top();
        lifo.pop();
        assert(node->bbox.contains(p,eps));

        if(node->is_inner)
        {
            for(int i=0; i<8; ++i)
            {
                if(print_debug_info) ++aabb_queries;
                if(node->children[i]->bbox.contains(p,eps)) lifo.push(node->children[i]);
            }
        }
        else
        {
            for(uint it_id : node->item_ids)
            {
                if(print_debug_info) ++item_queries;
                if(items.at(it_id)->contains(p,eps))
                {
                    ids.insert(it_id);
                }
            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        print_query_info("Contains query (all items)", how_many_seconds(t0,t1), aabb_queries, item_queries);
    }

    return !ids.empty();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Octree::intersects_ray(const vec3d & p, const vec3d & dir, double & min_t, uint & id) const
{
    typedef std::chrono::high_resolution_clock Time;
    Time::time_point t0 = Time::now();

    vec3d  pos;
    double t;
    if(!root->bbox.intersects_ray(p, dir, t, pos)) return false;
    Obj obj;
    obj.node = root;
    obj.dist = t;

    PrioQueue q;
    q.push(obj);

    uint aabb_queries = 1;
    uint item_queries = 0;

    while(!q.empty() && q.top().node->is_inner)
    {
        Obj obj = q.top();
        q.pop();

        for(int i=0; i<8; ++i)
        {
            OctreeNode *child = obj.node->children[i];
            if(child->bbox.intersects_ray(p, dir, t, pos))
            {
                if(child->is_inner)
                {
                    Obj obj;
                    obj.node = child;
                    obj.dist = t;
                    q.push(obj);
                }
                else
                {
                    for(uint id : child->item_ids)
                    {
                        if(items.at(id)->intersects_ray(p, dir, t, pos))
                        {
                            Obj obj;
                            obj.node = child;
                            obj.id   = id;
                            obj.dist = t;
                            q.push(obj);
                        }
                        if(print_debug_info) ++item_queries;
                    }
                }
                if(print_debug_info) ++aabb_queries;
            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        print_query_info("Intersects ray query", how_many_seconds(t0,t1), aabb_queries, item_queries);
    }

    if(q.empty()) return false;
    assert(q.top().id>=0);
    id    = q.top().id;
    min_t = q.top().dist;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Octree::intersects_ray(const vec3d & p, const vec3d & dir, std::set<std::pair<double,uint>> & all_hits) const
{
    typedef std::chrono::high_resolution_clock Time;
    Time::time_point t0 = Time::now();

    vec3d  pos;
    double t;
    if(!root->bbox.intersects_ray(p, dir, t, pos)) return false;
    Obj obj;
    obj.node = root;
    obj.dist = t;

    PrioQueue q;
    q.push(obj);

    uint aabb_queries = 1;
    uint item_queries = 0;

    while(!q.empty())
    {
        Obj obj = q.top();
        q.pop();

        for(int i=0; i<8; ++i)
        {
            OctreeNode *child = obj.node->children[i];
            if(child->bbox.intersects_ray(p, dir, t, pos))
            {
                if(child->is_inner)
                {
                    Obj obj;
                    obj.node = child;
                    obj.dist = t;
                    q.push(obj);
                }
                else
                {
                    for(uint id : child->item_ids)
                    {
                        if(items.at(id)->intersects_ray(p, dir, t, pos))
                        {
                            all_hits.insert(std::make_pair(t,id));
                        }
                        if(print_debug_info) ++item_queries;
                    }
                }

            }
            if(print_debug_info) ++aabb_queries;
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        print_query_info("Intersects ray query", how_many_seconds(t0,t1), aabb_queries, item_queries);
    }

    if(all_hits.empty()) return false;
    return true;
}

}
