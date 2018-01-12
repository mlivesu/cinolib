/*********************************************************************************
*  Copyright(C) 2016: Marco Livesu                                               *
*  All rights reserved.                                                          *
*                                                                                *
*  This file is part of CinoLib                                                  *
*                                                                                *
*  CinoLib is dual-licensed:                                                     *
*                                                                                *
*   - For non-commercial use you can redistribute it and/or modify it under the  *
*     terms of the GNU General Public License as published by the Free Software  *
*     Foundation; either version 3 of the License, or (at your option) any later *
*     version.                                                                   *
*                                                                                *
*   - If you wish to use it as part of a commercial software, a proper agreement *
*     with the Author(s) must be reached, based on a proper licensing contract.  *
*                                                                                *
*  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE       *
*  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     *
*                                                                                *
*  Author(s):                                                                    *
*                                                                                *
*     Marco Livesu (marco.livesu@gmail.com)                                      *
*     http://pers.ge.imati.cnr.it/livesu/                                        *
*                                                                                *
*     Italian National Research Council (CNR)                                    *
*     Institute for Applied Mathematics and Information Technologies (IMATI)     *
*     Via de Marini, 6                                                           *
*     16149 Genoa,                                                               *
*     Italy                                                                      *
**********************************************************************************/
#include <cinolib/tree.h>
#include <cinolib/cino_inline.h>
#include <assert.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
Tree<T>::Tree()
{
    // add a fake root (to make sure that the graph has always a single root)
    tree.push_back(TreeNode<T>());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
uint Tree<T>::add_children(T item, uint father)
{
    assert(father >= 0 && father < tree.size());

    uint fresh_id = tree.size();
    node(father).children.push_back(fresh_id);

    TreeNode<T> n;
    n.item   = item;
    n.father = father;
    n.depth  = node(father).depth + 1;

    tree.push_back(n);
    return fresh_id;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
CINO_INLINE
void Tree<T>::depth_first_traverse(std::vector<T> & items, const uint id) const
{
    if (id > 0) items.push_back(node(id).item);

    for(const uint c : node(id).children) depth_first_traverse(items, c);
}

}
