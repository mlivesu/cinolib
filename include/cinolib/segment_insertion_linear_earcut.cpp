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
#include <cinolib/segment_insertion_linear_earcut.h>
#include <cinolib/predicates.h>

namespace cinolib
{

template<class vec>
CINO_INLINE
void segment_insertion_linear_earcut(const std::vector<vec>  & poly,
                                           std::vector<uint> & tris)
{
    assert(poly.size()>=3);

    // doubly linked list for fast polygon inspection
    uint size = poly.size();
    std::vector<uint> prev(size);
    std::vector<uint> next(size);
    std::iota(prev.begin(), prev.end(),-1);
    std::iota(next.begin(), next.end(), 1);
    prev.front() = size-1;
    next.back()  = 0;

    // keep a list of the ears to be cut
    std::vector<uint> ears;
    ears.reserve(size);

    // this always has size |poly|, and keeps track of ears
    // (corners that were not ears at the beginning may become so later on)
    std::vector<bool> is_ear(size,false);

    // detect all safe ears in O(n).
    // This amounts to finding all convex vertices but the endpoints of the constrained edge
    for(uint curr=1; curr<size-1; ++curr)
    {
        // NOTE: the polygon may contain dangling edges,
        // clause prev!=next avoids to even do the ear test for them
        if(prev!=next && orient2d(poly.at(prev[curr]).ptr(),
                                  poly.at(curr).ptr(),
                                  poly.at(next[curr]).ptr())>0)
        {
            ears.emplace_back(curr);
            is_ear.at(curr) = true;
        }
    }

    // progressively delete all ears, also updating the data structure
    uint length = size;
    while(true)
    {
        uint curr = ears.back();
        ears.pop_back();

        // make new tri
        tris.push_back(prev[curr]);
        tris.push_back(curr);
        tris.push_back(next[curr]);

        // exclude curr from the polygon, connecting prev and next
        next[prev[curr]] = next[curr];
        prev[next[curr]] = prev[curr];

        // last triangle?
        if(--length<3) return;

        // check if prev and next have become new ears
        if(!is_ear.at(prev[curr]) && prev[curr]!=0)
        {
            if(prev[prev[curr]]!=next[curr] && orient2d(poly.at(prev[prev[curr]]).ptr(),
                                                        poly.at(prev[curr]).ptr(),
                                                        poly.at(next[curr]).ptr())>0)
            {
                ears.emplace_back(prev[curr]);
                is_ear.at(prev[curr]) = true;
            }
        }
        if(!is_ear.at(next[curr]) && next[curr]<size-1)
        {
            if(next[next[curr]]!=prev[curr] && orient2d(poly.at(prev[curr]).ptr(),
                                                        poly.at(next[curr]).ptr(),
                                                        poly.at(next[next[curr]]).ptr())>0)
            {
                ears.emplace_back(next[curr]);
                is_ear.at(next[curr]) = true;
            }
        }
    }
}

}
