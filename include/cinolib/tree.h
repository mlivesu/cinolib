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
