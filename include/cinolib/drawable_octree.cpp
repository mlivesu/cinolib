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
#include "drawable_octree.h"

#ifdef __APPLE__
#include <gl.h>
#include <glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace cinolib
{

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::DrawableOctree(const AbstractPolygonMesh<> &m) : Octree<T,MaxDepth,PrescribedItemsPerLeaf>(m)
{
    update_aabbs();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::DrawableOctree(const std::vector<T> & items, const std::vector<Bbox> & bboxes) : Octree<T,MaxDepth,PrescribedItemsPerLeaf>(items, bboxes)
{
    update_aabbs();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::draw(const float ) const
{
    for(auto obj : aabbs) obj.draw();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
vec3d DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::scene_center() const
{
    assert(this->root!=nullptr);
    return this->root->bbox.center();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
float DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::scene_radius() const
{
    assert(this->root!=nullptr);
    return this->root->bbox.diag();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::update_aabbs()
{
    assert(this->root!=nullptr);
    update_aabbs(this->root);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::update_aabbs(const OctreeNode *node)
{
    aabbs.push_back(DrawableAABB(node->bbox.min, node->bbox.max));
    if(node->is_inner)
    {
        assert(node->item_ids.empty());
        for(int i=0; i<8; ++i) update_aabbs(node->children[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::set_color(const Color & c)
{
    for(auto & obj : aabbs) obj.set_color(c);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T, uint MaxDepth, uint PrescribedItemsPerLeaf>
CINO_INLINE
void DrawableOctree<T,MaxDepth,PrescribedItemsPerLeaf>::set_thickness(float t)
{
    for(auto & obj : aabbs) obj.set_thickness(t);
}

}
