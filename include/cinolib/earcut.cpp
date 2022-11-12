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
#include <cinolib/earcut.h>
#include <cinolib/predicates.h>
#include <queue>
#include <numeric>

namespace cinolib
{

template<class point>
CINO_INLINE
bool earcut(const std::vector<point> & poly,
                  std::vector<uint>  & tris,
            const EarSorting           sort) // { SEQUENTIAL, RANDOMIZED, PRIORITIZED }
{
    tris.clear();

    if(sort==EarSorting::RANDOMIZED) srand(1234567);

    // doubly linked list for O(1) polygon update
    uint size = uint(poly.size());
    assert(size>=3);
    std::vector<uint> prev(size);
    std::vector<uint> next(size);
    std::iota(prev.begin(), prev.end(),-1);
    std::iota(next.begin(), next.end(), 1);
    prev.front() = size-1;
    next.back()  = 0;

    // list of the ears to be cut
    // (SEQUENTIAL and RANDOMIZED use ears, PRIORITIZED uses ears_prio)
    std::vector<uint> ears;
    std::priority_queue<std::pair<double,uint>> ears_prio;
    ears.reserve(size);

    // these always have size |poly|, and keep track of ears and concave vertices
    //  - corners that were not ears at the beginning may become so later on...
    //  - corners that were concave at the beginning may become convex/ears so later on....
    std::vector<bool> is_ear(size,false);   
    std::vector<bool> is_concave(size,false);

    // boolean function that performs a local concavity test around vertex v_curr
    auto concave_test = [&](const uint v_curr)
    {
        uint v_prev = prev[v_curr];
        uint v_next = next[v_curr];

        const double *t0 = poly.at(v_prev).ptr();
        const double *t1 = poly.at(v_curr).ptr();
        const double *t2 = poly.at(v_next).ptr();

        return (orient2d(t0,t1,t2)<=0);
    };

    // boolean function that performs a local ear test around vertex v_curr
    auto ear_test = [&](const uint v_curr)
    {
        if(is_concave[v_curr]) return false;

        uint v_prev = prev[v_curr];
        uint v_next = next[v_curr];

        const double *t0 = poly.at(v_prev).ptr();
        const double *t1 = poly.at(v_curr).ptr();
        const double *t2 = poly.at(v_next).ptr();

        // does the ear contain any other front vertex?
        uint beg  = next[v_curr];
        uint end  = prev[v_curr];
        uint test = next[beg];
        while(test!=end)
        {
            // check only concave vertices....
            if(is_concave[test] && point_in_triangle_2d(poly.at(test).ptr(),t0,t1,t2)!=STRICTLY_OUTSIDE)
            {
                return false;
            }
            test = next[test];
        }
        return true;
    };

    // inserts an ear into the ear list. uses the vector for SEQUENTIAL and
    // RANDOMIZED mode, and a prio queue for PRIORITIZED mode
    auto push_ear = [&](const uint v_curr)
    {
        is_ear.at(v_curr) = true;
        if(sort==EarSorting::PRIORITIZED)
        {
            uint  v_prev = prev[v_curr];
            uint  v_next = next[v_curr];
            point u      = poly.at(v_prev) - poly.at(v_curr);
            point v      = poly.at(v_next) - poly.at(v_curr);
            double ang   = u.angle_rad(v);
            ears_prio.push(std::make_pair(-ang,v_curr));
        }
        else ears.emplace_back(v_curr);
    };

    // detect all concave vertices and valid ears in O(n)
    for(uint curr=0; curr<size; ++curr) is_concave[curr] = concave_test(curr);
    for(uint curr=0; curr<size; ++curr) if(ear_test(curr)) push_ear(curr);

    // iteratively triangulate the polygon, also updating the list of ears
    // (a simple polygon with n vertices can be meshed with n-2 triangles)
    for(uint i=0; i<size-2; ++i)
    {
        // something went wrong.... degenerate polygon?
        if(ears.empty() && ears_prio.empty()) return false;

        uint curr;
        if(sort==EarSorting::PRIORITIZED)
        {
            curr = ears_prio.top().second;
            ears_prio.pop();
        }
        else
        {
            if(sort==EarSorting::RANDOMIZED)
            {
                uint off = rand()%ears.size();
                std::swap(ears.at(off),ears.back());
            }
            curr = ears.back();
            ears.pop_back();
        }

        // the ear has already been processed
        if(!is_ear.at(curr))
        {
            --i;
            continue;
        }

        // useful to mark the current ear as already processed
        is_ear.at(curr) = false;

        // make new tri
        tris.push_back(prev[curr]);
        tris.push_back(curr);
        tris.push_back(next[curr]);

        // exclude curr from the front, connecting prev and next
        next[prev[curr]] = next[curr];
        prev[next[curr]] = prev[curr];

        // update concavity info
        is_concave[prev[curr]] = concave_test(prev[curr]);
        is_concave[next[curr]] = concave_test(next[curr]);

        // update prev ear info
        if(ear_test(prev[curr]))
        {            
            push_ear(prev[curr]);
        }
        else if(is_ear.at(prev[curr]))
        {
            // if it was an ear before, mark it as non ear
            is_ear.at(prev[curr]) = false;
        }

        // update next ear
        if(ear_test(next[curr]))
        {
            push_ear(next[curr]);
        }
        else if(is_ear.at(next[curr]))
        {
            // if it was an ear before, mark it as non ear
            is_ear.at(next[curr]) = false;
        }
    }
    return true;
}

}

