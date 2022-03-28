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
#include <cinolib/parallel_for.h>
#include <cinolib/geometry/point.h>
#include <cinolib/geometry/sphere.h>
#include <cinolib/geometry/segment.h>
#include <cinolib/geometry/triangle.h>
#include <cinolib/geometry/tetrahedron.h>
#include <stack>

namespace cinolib
{

CINO_INLINE
OctreeNode::~OctreeNode()
{
    // "in a tree's node destructor, you only need to destroy the children pointers that are manually
    //  allocated by you. You don't need to worry about the deallocation of the node itself."
    //  https://stackoverflow.com/questions/34170164/destructor-for-binary-search-tree
    for(int i=0; i<8; ++i)
    {
        if(children[i]!=nullptr)
        {
            delete children[i];
        }
    }
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
    if(root!=nullptr) delete root;

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
    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    if(items.empty()) return;

    // initialize root with all items, also updating its AABB
    assert(root==nullptr);
    root = new OctreeNode(AABB());
    root->item_indices.resize(items.size());
    std::iota(root->item_indices.begin(),root->item_indices.end(),0);
    for(auto it : items) root->bbox.push(it->aabb);

    root->bbox.scale(1.5); // enlarge bbox to account for queries outside legal area.
                           // this should disappear eventually....

    if(root->item_indices.size()<items_per_leaf || max_depth==1)
    {
        leaves.push_back(root);
        tree_depth = 1;
    }
    else
    {
        subdivide(root);

        if(max_depth==2)
        {
            tree_depth = 2;
            for(int i=0; i<8; ++i) leaves.push_back(root->children[i]);
        }
        else
        {
            // WORK IN PARALLEL ON EACH OCTANT
            // To fully avoid syncrhonization between the threads global information
            // such as vector of leaves and tree depth are duplicated, and will be
            // merged after convergence.

            uint octant_depth[8] = { 2, 2, 2, 2, 2, 2, 2, 2 };
            std::vector<const OctreeNode*> octant_leaves[8];

            std::queue<std::pair<OctreeNode*,uint>> splitlist[8]; // (node, depth)
            for(int i=0; i<8; ++i)
            {
                if(root->children[i]->item_indices.size()>items_per_leaf)
                {
                    splitlist[i].push(std::make_pair(root->children[i],2));
                }
                else octant_leaves[i].push_back(root->children[i]);
            }

            PARALLEL_FOR(0,8,0,[&](uint i)
            {
                while(!splitlist[i].empty())
                {
                    auto pair  = splitlist[i].front();
                    auto node  = pair.first;
                    uint depth = pair.second + 1;
                    splitlist[i].pop();

                    subdivide(node);

                    for(int j=0; j<8; ++j)
                    {
                        if(depth<max_depth && node->children[j]->item_indices.size()>items_per_leaf)
                        {
                            splitlist[i].push(std::make_pair(node->children[j], depth));
                        }
                        else octant_leaves[i].push_back(node->children[j]);
                    }

                    octant_depth[i] = std::max(octant_depth[i], depth);
                }
            });

            // global merge of octant data
            tree_depth = *std::max_element(octant_depth, octant_depth+8);
            for(int i=0; i<8; ++i)
            {
                std::copy(octant_leaves[i].begin(), octant_leaves[i].end(), std::back_inserter(leaves));
            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        double t = how_many_seconds(t0,t1);
        std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
        std::cout << "Octree created (" << t << "s)                      " << std::endl;
        std::cout << "#Items                   : " << items.size()         << std::endl;
        std::cout << "#Leaves                  : " << leaves.size()        << std::endl;
        std::cout << "Max depth                : " << max_depth            << std::endl;
        std::cout << "Depth                    : " << tree_depth           << std::endl;
        std::cout << "Prescribed items per leaf: " << items_per_leaf       << std::endl;
        std::cout << "Max items per leaf       : " << max_items_per_leaf() << std::endl;
        std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::subdivide(OctreeNode * node)
{
    // create children octants
    vec3d min = node->bbox.min;
    vec3d max = node->bbox.max;
    vec3d avg = node->bbox.center();
    node->children[0] = new OctreeNode(AABB(vec3d(min[0], min[1], min[2]), vec3d(avg[0], avg[1], avg[2])));
    node->children[1] = new OctreeNode(AABB(vec3d(avg[0], min[1], min[2]), vec3d(max[0], avg[1], avg[2])));
    node->children[2] = new OctreeNode(AABB(vec3d(avg[0], avg[1], min[2]), vec3d(max[0], max[1], avg[2])));
    node->children[3] = new OctreeNode(AABB(vec3d(min[0], avg[1], min[2]), vec3d(avg[0], max[1], avg[2])));
    node->children[4] = new OctreeNode(AABB(vec3d(min[0], min[1], avg[2]), vec3d(avg[0], avg[1], max[2])));
    node->children[5] = new OctreeNode(AABB(vec3d(avg[0], min[1], avg[2]), vec3d(max[0], avg[1], max[2])));
    node->children[6] = new OctreeNode(AABB(vec3d(avg[0], avg[1], avg[2]), vec3d(max[0], max[1], max[2])));
    node->children[7] = new OctreeNode(AABB(vec3d(min[0], avg[1], avg[2]), vec3d(avg[0], max[1], max[2])));

    for(uint it : node->item_indices)
    {
        bool orphan = true;
        for(int i=0; i<8; ++i)
        {
            assert(node->children[i]!=nullptr);
            if(node->children[i]->bbox.intersects_box(items.at(it)->aabb))
            {
                node->children[i]->item_indices.push_back(it);
                orphan = false;
            }
        }
        assert(!orphan);
    }

    node->item_indices.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::push_point(const uint id, const vec3d & v)
{
    items.push_back(new Point(id,v));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::push_sphere(const uint id, const vec3d & c, const double r)
{
    items.push_back(new Sphere(id,c,r));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::push_segment(const uint id, const std::vector<vec3d> & v)
{
    items.push_back(new Segment(id,v.data()));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::push_triangle(const uint id, const std::vector<vec3d> & v)
{
    items.push_back(new Triangle(id,v.data()));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::push_tetrahedron(const uint id, const std::vector<vec3d> & v)
{
    items.push_back(new Tetrahedron(id,v.data()));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint Octree::max_items_per_leaf() const
{
    uint max=0;
    for(auto l : leaves) max = std::max(max,(uint)l->item_indices.size());
    return max;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Octree::debug_mode(const bool b)
{
    print_debug_info = b;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d Octree::closest_point(const vec3d & p) const
{
    uint   id;
    vec3d  pos;
    double dist;
    closest_point(p, id, pos, dist);
    return pos;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://stackoverflow.com/questions/41306122/nearest-neighbor-search-in-octree
CINO_INLINE
void Octree::closest_point(const vec3d  & p,          // query point
                                 uint   & id,         // id of the item T closest to p
                                 vec3d  & pos,        // point in T closest to p
                                 double & dist) const // distance between pos and p
{
    assert(root != nullptr);

    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    PrioQueue q;
    if(root->is_inner())
    {
        Obj obj;
        obj.node = root;
        obj.dist = root->bbox.dist_sqrd(p);
        q.push(obj);
    }
    else // in case the root is alrady a leaf...
    {
        for(uint index : root->item_indices)
        {
            Obj obj;
            obj.node  = root;
            obj.index = index;
            obj.pos   = items.at(index)->point_closest_to(p);
            obj.dist  = obj.pos.dist_sqrd(p);
            q.push(obj);
        }
    }

    while(q.top().node->is_inner())
    {
        Obj obj = q.top();
        q.pop();

        for(int i=0; i<8; ++i)
        {
            OctreeNode *child = obj.node->children[i];
            if(child->is_inner())
            {
                Obj obj;
                obj.node = child;
                obj.dist = child->bbox.dist_sqrd(p);
                q.push(obj);
            }
            else
            {
                for(uint index : child->item_indices)
                {
                    Obj obj;
                    obj.node  = child;
                    obj.index = index;
                    obj.pos   = items.at(index)->point_closest_to(p);
                    obj.dist  = obj.pos.dist_sqrd(p);
                    q.push(obj);
                }
            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        std::cout << "Closest point\t" << how_many_seconds(t0,t1) << " seconds" << std::endl;
    }

    assert(q.top().index>=0);
    id   = items.at(q.top().index)->id;
    pos  = q.top().pos;
    dist = q.top().dist;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// this query becomes exact if CINOLIB_USES_SHEWCHUK_PREDICATES is defined
CINO_INLINE
bool Octree::contains(const vec3d & p, const bool strict, uint & id) const
{
    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    std::stack<OctreeNode*> lifo;
    if(root && root->bbox.contains(p,strict))
    {
        lifo.push(root);
    }

    while(!lifo.empty())
    {
        OctreeNode *node = lifo.top();
        lifo.pop();
        assert(node->bbox.contains(p, strict));

        if(node->is_inner())
        {
            for(int i=0; i<8; ++i)
            {
                if(node->children[i]->bbox.contains(p,strict)) lifo.push(node->children[i]);
            }
        }
        else
        {
            for(uint i : node->item_indices)
            {
                if(items.at(i)->contains(p,strict))
                {
                    id = items.at(i)->id;
                    if(print_debug_info)
                    {
                        Time::time_point t1 = Time::now();
                        std::cout << "Contains query (first item)\t" << how_many_seconds(t0,t1) << " seconds" << std::endl;
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// this query becomes exact if CINOLIB_USES_SHEWCHUK_PREDICATES is defined
CINO_INLINE
bool Octree::contains(const vec3d & p, const bool strict, std::unordered_set<uint> & ids) const
{
    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    std::stack<OctreeNode*> lifo;
    if(root && root->bbox.contains(p,strict))
    {
        lifo.push(root);
    }

    while(!lifo.empty())
    {
        OctreeNode *node = lifo.top();
        lifo.pop();
        assert(node->bbox.contains(p,strict));

        if(node->is_inner())
        {
            for(int i=0; i<8; ++i)
            {
                if(node->children[i]->bbox.contains(p,strict)) lifo.push(node->children[i]);
            }
        }
        else
        {
            for(uint i : node->item_indices)
            {
                if(items.at(i)->contains(p,strict))
                {
                    ids.insert(items.at(i)->id);
                }
            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        std::cout << "Contains query (all items)\t" << how_many_seconds(t0,t1) << " seconds" << std::endl;
    }

    return !ids.empty();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Octree::intersects_ray(const vec3d & p, const vec3d & dir, double & min_t, uint & id) const
{
    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    vec3d  pos;
    double t=0.0;
    if(root && !root->bbox.intersects_ray(p, dir, t, pos)) return false;
    Obj obj;
    obj.node = root;
    obj.dist = t;

    PrioQueue q;
    q.push(obj);

    while(!q.empty() && q.top().node->is_inner())
    {
        Obj obj = q.top();
        q.pop();

        for(int i=0; i<8; ++i)
        {
            OctreeNode *child = obj.node->children[i];
            if(child->bbox.intersects_ray(p, dir, t, pos))
            {
                if(child->is_inner())
                {
                    Obj obj;
                    obj.node = child;
                    obj.dist = t;
                    q.push(obj);
                }
                else
                {
                    for(uint i : child->item_indices)
                    {
                        if(items.at(i)->intersects_ray(p, dir, t, pos))
                        {
                            Obj obj;
                            obj.node  = child;
                            obj.index = items.at(i)->id;
                            obj.dist  = t;
                            q.push(obj);
                        }
                    }
                }
            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        std::cout << "Intersects ray\t" << how_many_seconds(t0,t1) << " seconds" << std::endl;
    }

    if(q.empty()) return false;
    assert(q.top().index>=0);
    id    = items.at(q.top().index)->id;
    min_t = q.top().dist;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Octree::intersects_ray(const vec3d & p, const vec3d & dir, std::set<std::pair<double,uint>> & all_hits) const
{
    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    vec3d  pos;
    double t=0.0;
    if(root && !root->bbox.intersects_ray(p, dir, t, pos)) return false;
    Obj obj;
    obj.node = root;
    obj.dist = t;

    PrioQueue q;
    q.push(obj);

    while(!q.empty())
    {
        Obj obj = q.top();
        q.pop();

        for(int i=0; i<8; ++i)
        {
            OctreeNode *child = obj.node->children[i];
            if(child->bbox.intersects_ray(p, dir, t, pos))
            {
                if(child->is_inner())
                {
                    Obj obj;
                    obj.node = child;
                    obj.dist = t;
                    q.push(obj);
                }
                else
                {
                    for(uint i : child->item_indices)
                    {
                        if(items.at(i)->intersects_ray(p, dir, t, pos))
                        {
                            all_hits.insert(std::make_pair(t,items.at(i)->id));
                        }
                    }
                }

            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        std::cout << "Intersects ray\t" << how_many_seconds(t0,t1) << " seconds" << std::endl;
    }

    if(all_hits.empty()) return false;
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// this query becomes exact if CINOLIB_USES_SHEWCHUK_PREDICATES is defined
CINO_INLINE
bool Octree::intersects_triangle(const vec3d t[], const bool ignore_if_valid_complex, std::unordered_set<uint> & ids) const
{
    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    std::unordered_set<uint> tmp;
    std::vector<vec3d> list = {t[0],t[1],t[2]};
    intersects_box(AABB(list), tmp);

    for(uint i : tmp)
    {
        if(items.at(i)->intersects_triangle(t, ignore_if_valid_complex))
        {
            ids.insert(items.at(i)->id);
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        std::cout << "Intersects triangle\t" << how_many_seconds(t0,t1) << " seconds" << std::endl;
    }

    return !ids.empty();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// this query becomes exact if CINOLIB_USES_SHEWCHUK_PREDICATES is defined
CINO_INLINE
bool Octree::intersects_segment(const vec3d s[], const bool ignore_if_valid_complex, std::unordered_set<uint> & ids) const
{
    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    std::unordered_set<uint> tmp;
    intersects_box(AABB(s[0],s[1]), tmp);

    for(uint i : tmp)
    {
        if(items.at(i)->intersects_segment(s, ignore_if_valid_complex))
        {
            ids.insert(items.at(i)->id);
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        std::cout << "Intersects segment\t" << how_many_seconds(t0,t1) << " seconds" << std::endl;
    }

    return !ids.empty();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// WARNING: this function may return false positives because it only checks intersection between
// the box b and the AABB of the items in the tree. This is a partial result that it is useful for
// some of the queries above, where a more expensive test between the geometric entity approximated
// by box b and the actual items will be performed
CINO_INLINE
bool Octree::intersects_box(const AABB & b, std::unordered_set<uint> & ids) const
{
    typedef std::chrono::steady_clock Time;
    Time::time_point t0 = Time::now();

    std::stack<OctreeNode*> lifo;
    if(root && root->bbox.intersects_box(b))
    {
        lifo.push(root);
    }

    while(!lifo.empty())
    {        
        OctreeNode *node = lifo.top();
        lifo.pop();
        assert(node->bbox.intersects_box(b));

        if(node->is_inner())
        {            
            for(int i=0; i<8; ++i)
            {
                if(node->children[i]->bbox.intersects_box(b))
                {
                    lifo.push(node->children[i]);
                }
            }
        }
        else
        {
            for(uint i : node->item_indices)
            {
                if(items.at(i)->aabb.intersects_box(b))
                {
                    ids.insert(items.at(i)->id);
                }
            }
        }
    }

    if(print_debug_info)
    {
        Time::time_point t1 = Time::now();
        std::cout << "Intersects box\t" << how_many_seconds(t0,t1) << " seconds" << std::endl;
    }

    return !ids.empty();
}

}
