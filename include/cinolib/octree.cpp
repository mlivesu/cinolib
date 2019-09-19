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
#include <queue>

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

template<typename T>
CINO_INLINE
Octree<T>::Octree(const AbstractPolygonMesh<> & m,
                  const uint                    max_depth,
                  const uint                    items_per_leaf)
    : max_depth(max_depth)
    , items_per_leaf(items_per_leaf)
{
    // grab item list and bboxes from the mesh
    uint np = m.num_polys();
    items.reserve(np);
    boxes.reserve(np);
    for(uint pid=0; pid<np; ++pid)
    {
        items.push_back(pid);
        boxes.push_back(Bbox(m.poly_verts(pid)));
    }
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
Octree<T>::Octree(const std::vector<T>    & items,
                  const std::vector<Bbox> & boxes,
                  const uint                max_depth,
                  const uint                items_per_leaf)
: items(items)
, boxes(boxes)
, max_depth(max_depth)
, items_per_leaf(items_per_leaf)
{
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
Octree<T>::~Octree()
{
    delete root;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void Octree<T>::init()
{
    assert(items.size() == boxes.size());

    root = new OctreeNode(nullptr, Bbox(boxes, 1.5));

    tree_depth = 1;
    num_leaves = 1;

    for(uint id=0; id<items.size(); ++id) add_item(id, root, 0);

    std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
    std::cout << "Octree created                                 " << std::endl;
    std::cout << "Depth             : " << tree_depth              << std::endl;
    std::cout << "#Items            : " << items.size()            << std::endl;
    std::cout << "#Leaves           : " << num_leaves              << std::endl;
    std::cout << "MAX items per leaf: " << max_items_per_leaf()    << std::endl;
    std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void Octree<T>::add_item(const uint id, OctreeNode * node, const uint depth)
{
    assert(node->bbox.intersects(boxes.at(id)));

    if(node->is_inner)
    {
        assert(node->item_ids.empty());
        for(int i=0; i<8; ++i)
        {
            assert(node->children[i]!=nullptr);
            if(node->children[i]->bbox.intersects(boxes.at(id)))
            {
                add_item(id, node->children[i], depth+1);
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
                    if(node->children[i]->bbox.intersects(boxes.at(item)))
                    {
                        add_item(item, node->children[i], d_plus_one);
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

template<typename T>
CINO_INLINE
uint Octree<T>::max_items_per_leaf() const
{
    return max_items_per_leaf(root, 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
uint Octree<T>::max_items_per_leaf(const OctreeNode * node, const uint max) const
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

template<typename T>
CINO_INLINE
T Octree<T>::nearest_neighbor(const vec3d & p) const
{
    // https://stackoverflow.com/questions/41306122/nearest-neighbor-search-in-octree
    struct O
    {
        double      dist = inf_double;
        OctreeNode *node = nullptr;
        int         id   = -1;
    };
    struct Comp { bool operator()(const O & obj1, const O & obj2) {return obj1.dist > obj2.dist; }};
    typedef std::priority_queue<O,std::vector<O>,Comp> Q;

    Q q;
    O obj;
    obj.node = root;
    obj.dist = root->bbox.dist_to_point_sqrd(p);
    q.push(obj);

    while(q.top().node->is_inner)
    {
        O obj = q.top();
        q.pop();
        //std::cout << "pop inner node. dist " << obj.dist << std::endl;

        for(int i=0; i<8; ++i)
        {
            OctreeNode *child = obj.node->children[i];
            if(child->is_inner)
            {
                O obj;
                obj.node = child;
                obj.dist = child->bbox.dist_to_point_sqrd(p);
                q.push(obj);
                //std::cout << "push inner node. dist " << obj.dist << std::endl;
            }
            else for(uint id : child->item_ids)
            {
                // TODO: here I should be used distance to the real object (as opposed to distance to its AABB)
                O obj;
                obj.node = child;
                obj.dist = boxes.at(id).dist_to_point_sqrd(p);
                obj.id   = id;
                q.push(obj);
                //std::cout << "push leaf node. item " << id << ", dist " << obj.dist << std::endl;
            }
        }
    }
    assert(q.top().id>=0);
    return items.at(q.top().id);
}

}
