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

template<typename T>
CINO_INLINE
DrawableOctree<T>::DrawableOctree(const AbstractPolygonMesh<> & m,
                                  const uint                    max_depth,
                                  const uint                    items_per_leaf)
: Octree<T>(m, max_depth, items_per_leaf)
{
    update_aabbs();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
DrawableOctree<T>::DrawableOctree(const std::vector<T>    & items,
                                  const std::vector<Bbox> & bboxes,
                                  const uint                max_depth,
                                  const uint                items_per_leaf)
: Octree<T>(items, bboxes, max_depth, items_per_leaf)
{
    update_aabbs();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void DrawableOctree<T>::draw(const float ) const
{
    for(auto obj : aabbs) obj.draw();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
vec3d DrawableOctree<T>::scene_center() const
{
    assert(this->root!=nullptr);
    return this->root->bbox.center();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
float DrawableOctree<T>::scene_radius() const
{
    assert(this->root!=nullptr);
    return this->root->bbox.diag();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void DrawableOctree<T>::update_aabbs()
{
    assert(this->root!=nullptr);
    update_aabbs(this->root);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void DrawableOctree<T>::update_aabbs(const OctreeNode *node)
{
    aabbs.push_back(DrawableAABB(node->bbox.min, node->bbox.max));
    if(node->is_inner)
    {
        assert(node->item_ids.empty());
        for(int i=0; i<8; ++i) update_aabbs(node->children[i]);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void DrawableOctree<T>::set_color(const Color & c)
{
    for(auto & obj : aabbs) obj.set_color(c);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<typename T>
CINO_INLINE
void DrawableOctree<T>::set_thickness(float t)
{
    for(auto & obj : aabbs) obj.set_thickness(t);
}

}
