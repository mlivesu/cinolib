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

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
Octree<T,MaxDepth,PrescribedItemsPerLeaf>::Octree(const AbstractPolygonMesh<>  & m)
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

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
Octree<T,MaxDepth,PrescribedItemsPerLeaf>::Octree(const std::vector<T> & items,
                                                  const std::vector<Bbox> & boxes)
: items(items), boxes(boxes)
{
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
Octree<T,MaxDepth,PrescribedItemsPerLeaf>::~Octree()
{
    delete root;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void Octree<T,MaxDepth,PrescribedItemsPerLeaf>::init()
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

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void Octree<T,MaxDepth,PrescribedItemsPerLeaf>::add_item(const uint id, OctreeNode * node, const uint depth)
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
        if(node->item_ids.size()>PrescribedItemsPerLeaf && depth<MaxDepth)
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
            tree_depth = std::max(tree_depth, d_plus_one);
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void Octree<T,MaxDepth,PrescribedItemsPerLeaf>::get_items(const vec3d & p, std::vector<T> & res) const
{
    assert(root!=nullptr);
    if(!root->bbox.contains(p))
    {
        std::cout << "WARNING: query point falls outside the octree. Return all items" << std::endl;
        res = items;
        return;
    }
    res.clear();
    get_items(root, p, res);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void Octree<T,MaxDepth,PrescribedItemsPerLeaf>::get_items(const OctreeNode * node, const vec3d & p, std::vector<T> & res) const
{
    assert(node!=nullptr);
    if(node->is_inner) // go down in the tree
    {
        for(int i=0; i<8; ++i)
        {
            assert(node->children[i]!=nullptr);
            if(node->children[i]->bbox.contains(p)) get_items(node->children[i], p, res);
        }

        // if the query point doesn't fall inside any non empty leave, return all elements at this level of the tree
        if(res.empty())
        {
            for(int i=0; i<8; ++i)
            {
                std::copy(node->children[i]->item_ids.begin(), node->children[i]->item_ids.begin(), std::back_inserter(res));
            }
        }
    }
    else // grab items
    {
        assert(!node->item_ids.empty());
        // WARNING: this will likely duplicate items spanning across multiple nodes!
        std::copy(node->item_ids.begin(), node->item_ids.begin(), std::back_inserter(res));
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
uint Octree<T,MaxDepth,PrescribedItemsPerLeaf>::max_items_per_leaf() const
{
    return max_items_per_leaf(root, 0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
uint Octree<T,MaxDepth,PrescribedItemsPerLeaf>::max_items_per_leaf(const OctreeNode * node, const uint max) const
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

}
