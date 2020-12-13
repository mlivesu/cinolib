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
#include <cinolib/non_manifold_edge_geometric_sorting.h>
#include <cinolib/predicates.h>

namespace cinolib
{

CINO_INLINE
void non_manifold_edge_geometric_sorting(const vec3d              & e0,
                                         const vec3d              & e1,
                                         const std::vector<vec3d> & tris,
                                               std::vector<uint>  & ordering,
                                         const uint                 first_elem)
{
    ordering.clear();
    uint curr = first_elem;
    do
    {
        ordering.push_back(curr);

        // find all elements in the positive half space w.r.t. curr
        std::vector<uint> next_pool;
        for(uint i=0; i<tris.size(); ++i)
        {
            if(i!=curr && orient3d(e0,e1,tris.at(curr),tris.at(i))>0)
            {
                next_pool.push_back(i);
            }
        }

        // find element in next_pool that has all other elements in its positive half space
        uint next = curr;
        for(uint j=0; j<next_pool.size(); ++j)
        {
            bool leftmost = true;
            for(uint k=0; k<next_pool.size(); ++k)
            {
                if(j==k) continue;
                if(orient3d(e0,e1,tris.at(next_pool.at(j)),tris.at(next_pool.at(k)))<0)
                {
                    leftmost = false;
                    break;
                }
            }
            if(leftmost)
            {
                next = next_pool.at(j);
                break;
            }
        }
        assert(next!=curr);
        curr = next;
    }
    while(curr!=first_elem);
}

}
