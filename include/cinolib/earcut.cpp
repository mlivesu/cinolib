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

namespace cinolib
{

// Implementation of the classical earcut algorithm, following the guidelines
// contained in:
//
//      Triangulation by ear clipping
//      Geometric Tools (2008)
//      D.Eberly
//
// NOTE: using the templated class point, I can feed this routine with
// 3d points and use just the XY component without performing any cast
template<class point>
CINO_INLINE
void earcut(const std::vector<point> & poly,
                  std::vector<uint>  & tris)
{
    tris.clear();

    // doubly linked list for O(1) polygon update
    uint size = poly.size();
    assert(size>=3);
    std::vector<uint> prev(size);
    std::vector<uint> next(size);
    std::iota(prev.begin(), prev.end(),-1);
    std::iota(next.begin(), next.end(), 1);
    prev.front() = size-1;
    next.back()  = 0;

    // list of the ears to be cut
    std::vector<uint> ears;
    ears.reserve(size);

    // this always has size |poly|, and keeps track of ears
    // (corners that were not ears at the beginning may become so later on)
    std::vector<bool> is_ear(size,false);

    // boolean function that performs a local ear test around vertex v_curr
    auto ear_test = [&](const uint v_curr)
    {
        uint v_prev = prev[v_curr];
        uint v_next = next[v_curr];

        const double *t0 = poly.at(v_prev).ptr();
        const double *t1 = poly.at(v_curr).ptr();
        const double *t2 = poly.at(v_next).ptr();

        // is the vertex convex?
        if(orient2d(t0,t1,t2)<=0) return false;

        // does the ear contain any other front vertex?
        // NOTE: this could be made faster by storing (and checking) only concave vertices
        uint beg  = next[v_curr];
        uint end  = prev[v_curr];
        uint test = next[beg];
        while(test!=end)
        {
            if(point_in_triangle_2d(poly.at(test).ptr(),t0,t1,t2)!=STRICTLY_OUTSIDE)
            {
                return false;
            }
            test = next[test];
        }
        return true;
    };

    // detect all initial ears in O(n)
    for(uint curr=0; curr<size; ++curr)
    {
        if(ear_test(curr))
        {
            ears.emplace_back(curr);
            is_ear.at(curr) = true;
        }
    }

    // iteratively triangulate the polygon, also updating the list of ears
    // (a simple polygon with n vertices can be meshed with n-2 triangles)
    for(uint i=0; i<size-2; ++i)
    {
        uint curr = ears.back();
        ears.pop_back();

        // make new tri
        tris.push_back(prev[curr]);
        tris.push_back(curr);
        tris.push_back(next[curr]);

        // exclude curr from the front, connecting prev and next
        next[prev[curr]] = next[curr];
        prev[next[curr]] = prev[curr];

        // update prev ear
        if(ear_test(prev[curr]))
        {
            // if it was not an ear before, append it
            if(!is_ear.at(prev[curr]))
            {
                ears.emplace_back(prev[curr]);
                is_ear.at(prev[curr]) = true;
            }
        }
        else if(is_ear.at(prev[curr]))
        {
            // if it was an ear before, remove it
            // NOTE: this is linear in the size of vector ears. is it worth optimizing?
            REMOVE_FROM_VEC(ears, prev[curr]);
            is_ear.at(prev[curr]) = false;
        }

        // update next ear
        if(ear_test(next[curr]))
        {
            // if it was not an ear before, append it
            if(!is_ear.at(next[curr]))
            {
                ears.emplace_back(next[curr]);
                is_ear.at(next[curr]) = true;
            }
        }
        else if(is_ear.at(next[curr]))
        {
            // if it was an ear before, remove it
            // NOTE: this is linear in the size of vector ears. is it worth optimizing?
            REMOVE_FROM_VEC(ears, next[curr]);
            is_ear.at(next[curr]) = false;
        }
    }
}

}

