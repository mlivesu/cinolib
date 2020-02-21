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
#include <cinolib/tetrahedralization.h>
#include <cinolib/ipair.h>
#include <set>

namespace cinolib
{

// Subdivides a prism with triangular base into 3 tets.
// Prism vertices are assumed in the following order:
//
//      v2             v5
//    /    \         /    \
//  v0 --- v1      v3 --- v4
//
//   bot base       top base
//
CINO_INLINE
void prism_to_tets(const std::vector<uint> & prism,
                         std::vector<uint> & tets)
{
    std::set<ipair> ordered_vids;
    for(uint i=0; i<6; ++i) ordered_vids.insert(ipair(prism[i],i));

    // see Table 2 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    uint ref_prism[6];
    switch ((*ordered_vids.begin()).second)
    {
        case 0: ref_prism[0] = prism[0];
                ref_prism[1] = prism[1];
                ref_prism[2] = prism[2];
                ref_prism[3] = prism[3];
                ref_prism[4] = prism[4];
                ref_prism[5] = prism[5];
                break;

        case 1: ref_prism[0] = prism[1];
                ref_prism[1] = prism[2];
                ref_prism[2] = prism[0];
                ref_prism[3] = prism[4];
                ref_prism[4] = prism[5];
                ref_prism[5] = prism[3];
                break;

        case 2: ref_prism[0] = prism[2];
                ref_prism[1] = prism[0];
                ref_prism[2] = prism[1];
                ref_prism[3] = prism[5];
                ref_prism[4] = prism[3];
                ref_prism[5] = prism[4];
                break;

        case 3: ref_prism[0] = prism[3];
                ref_prism[1] = prism[5];
                ref_prism[2] = prism[4];
                ref_prism[3] = prism[0];
                ref_prism[4] = prism[2];
                ref_prism[5] = prism[1];
                break;

        case 4: ref_prism[0] = prism[4];
                ref_prism[1] = prism[3];
                ref_prism[2] = prism[5];
                ref_prism[3] = prism[1];
                ref_prism[4] = prism[0];
                ref_prism[5] = prism[2];
                break;

        case 5: ref_prism[0] = prism[5];
                ref_prism[1] = prism[4];
                ref_prism[2] = prism[3];
                ref_prism[3] = prism[2];
                ref_prism[4] = prism[1];
                ref_prism[5] = prism[0];
                break;

        default: assert(false);
    }

    // see Table 3 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    if(std::min(ref_prism[1],ref_prism[5]) < std::min(ref_prism[2],ref_prism[4]))
    {
        // t0
        tets.push_back(ref_prism[0]);
        tets.push_back(ref_prism[1]);
        tets.push_back(ref_prism[2]);
        tets.push_back(ref_prism[5]);
        // t1
        tets.push_back(ref_prism[0]);
        tets.push_back(ref_prism[1]);
        tets.push_back(ref_prism[5]);
        tets.push_back(ref_prism[4]);
    }
    else
    {
        // t0
        tets.push_back(ref_prism[0]);
        tets.push_back(ref_prism[1]);
        tets.push_back(ref_prism[2]);
        tets.push_back(ref_prism[4]);
        // t1
        tets.push_back(ref_prism[0]);
        tets.push_back(ref_prism[4]);
        tets.push_back(ref_prism[2]);
        tets.push_back(ref_prism[5]);
    }
    // t2
    tets.push_back(ref_prism[0]);
    tets.push_back(ref_prism[4]);
    tets.push_back(ref_prism[5]);
    tets.push_back(ref_prism[3]);
}

}
