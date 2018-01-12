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
#ifndef CINO_TREE_H
#define CINO_TREE_H

#include <sys/types.h>
#include <vector>

/*
 * This class implements a trivial n-ary tree. This is not supposed to be perfect,
 * and there is a lot of stuff missing (e.g., node deletion). The only reason it
 * exists is because the cinolib::Profiler is bassed around this class.
*/

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
struct TreeNode
{
    T    item;
    uint depth  = 0;
    uint father = 0;
    std::vector<uint> children;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class T>
class Tree
{
    public:

        explicit Tree();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        uint add_children(T item, uint father = 0);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        const TreeNode<T> & node(const uint id) const { return tree.at(id); }
              TreeNode<T> & node(const uint id)       { return tree.at(id); }

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void depth_first_traverse(std::vector<T> & items, const uint id = 0) const;

    protected:

        std::vector<TreeNode<T>> tree;
};

}

#ifndef  CINO_STATIC_LIB
#include "tree.cpp"
#endif

#endif // CINO_TREE_H
