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

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
Octree<T,MaxDepth,PrescribedItemsPerLeaf>::Octree(const AbstractPolygonMesh<>  & m)
{
    // grab item list and bboxes from the mesh
    uint np = m.num_polys();
    items.reserve(np);
    bboxes.reserve(np);
    for(uint pid=0; pid<np; ++pid)
    {
        items.push_back(pid);
        bboxes.push_back(Bbox(m.poly_verts(pid)));
    }
    init();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
Octree<T,MaxDepth,PrescribedItemsPerLeaf>::Octree(const std::vector<T>    & items,
                  const std::vector<Bbox> & bboxes) : items(items), bboxes(bboxes)
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
    assert(items.size() == bboxes.size());

    root = new Node(nullptr, Bbox(bboxes, 2.0));

    tree_depth = 1;
    num_leaves = 1;
    max_items_per_leaf = items.size();

    for(uint id=0; id<items.size(); ++id) add_item(id, root, 0);

    std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
    std::cout << "Octree created                                 " << std::endl;
    std::cout << "Depth             : " << tree_depth              << std::endl;
    std::cout << "#Items            : " << items.size()            << std::endl;
    std::cout << "#Leaves           : " << num_leaves              << std::endl;
    std::cout << "Max items per leaf: " << max_items_per_leaf << std::endl;
    std::cout << ":::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void Octree<T,MaxDepth,PrescribedItemsPerLeaf>::add_item(const uint id, Node * node, const uint depth)
{
    if(node->bbox.intersects(bboxes.at(id)))
    {
        if(node->is_inner)
        {
            for(int i=0; i<8; ++i)
            {
                if(node->children[i]!=nullptr) add_item(id, node->children[i], depth+1);
            }
        }
        else // non empty leaf
        {
            node->item_ids.push_back(id);

            // if the node contains more elements than allowed, and the depth
            // of the tree is lower than max depth: split the node into 8 octants
            // and move all its items downwards
            //
            if(node->item_ids.size()>PrescribedItemsPerLeaf && depth<MaxDepth)
            {
                node->is_inner = true;
                std::cout << "reached max number of elements per leaf. split into 8 octants" << std::endl;

                // create children octants
                vec3d min = node->bbox.min;
                vec3d max = node->bbox.min;
                vec3d avg = node->bbox.center();
                node->children[0] = new Node(node, Bbox(vec3d(min[0], min[1], min[2]), vec3d(avg[0], avg[1], avg[2])));
                node->children[1] = new Node(node, Bbox(vec3d(avg[0], min[1], min[2]), vec3d(max[0], avg[1], avg[2])));
                node->children[2] = new Node(node, Bbox(vec3d(avg[0], avg[1], min[2]), vec3d(max[0], max[1], avg[2])));
                node->children[3] = new Node(node, Bbox(vec3d(min[0], avg[1], min[2]), vec3d(avg[0], max[1], avg[2])));
                node->children[4] = new Node(node, Bbox(vec3d(min[0], min[1], avg[2]), vec3d(avg[0], avg[1], max[2])));
                node->children[5] = new Node(node, Bbox(vec3d(avg[0], min[1], avg[2]), vec3d(max[0], avg[1], max[2])));
                node->children[6] = new Node(node, Bbox(vec3d(avg[0], avg[1], avg[2]), vec3d(max[0], max[1], max[2])));
                node->children[7] = new Node(node, Bbox(vec3d(min[0], avg[1], avg[2]), vec3d(avg[0], max[1], max[2])));

                // mode items downwards in the tree
                // NOTE: items that span across multiple octants will be added to each node they intersect)
                uint d_plus_one = depth+1;
                for(uint item : items)
                {
                    add_item(item, node->children[0], d_plus_one);
                    add_item(item, node->children[1], d_plus_one);
                    add_item(item, node->children[2], d_plus_one);
                    add_item(item, node->children[3], d_plus_one);
                    add_item(item, node->children[4], d_plus_one);
                    add_item(item, node->children[5], d_plus_one);
                    add_item(item, node->children[6], d_plus_one);
                    add_item(item, node->children[7], d_plus_one);
                }
                items.clear();

                // remove items from current node
                num_leaves += 7; // 8 children minus the current node
                tree_depth = std::max(tree_depth, d_plus_one);
            }
            else
            {
                max_items_per_leaf = std::max((uint)node->item_ids.size(), max_items_per_leaf);
            }
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
void Octree<T,MaxDepth,PrescribedItemsPerLeaf>::get_items(const Node * node, const vec3d & p, std::vector<T> & res) const
{
    assert(node!=nullptr);
    if(node->bbox.contains(p))
    {
        if(node->is_inner)
        {
            for(int i=0; i<8; ++i)
            {
                if(node->children[i]!=nullptr) get_items(node->children[i], p, res);
            }
        }
        else
        {
            // WARNING: this will likely duplicate items spanning across multiple nodes!
            for(uint id : node->item_ids) res.push_back(items.at(id));
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
Octree<T,MaxDepth,PrescribedItemsPerLeaf>::Node::~Node()
{
    // "in a tree's node destructor, you only need to destroy the children pointers that are manually
    //  allocated by you. You don't need to worry about the deallocation of the node itself."
    //  https://stackoverflow.com/questions/34170164/destructor-for-binary-search-tree
    for(int i=0; i<8; ++i) delete children[i];
}

}
