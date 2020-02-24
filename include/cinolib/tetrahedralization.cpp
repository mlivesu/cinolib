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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Transforms a hexahedral mesh into a conforming tetrahedral
// mesh, splitting all hexahedra into five or six tetrahedra
template<class M, class V, class E, class F, class P>
CINO_INLINE
void hex_to_tets(const Hexmesh<M,V,E,F,P> & hm,
                       Tetmesh<M,V,E,F,P> & tm)
{
    for(uint vid=0; vid<hm.num_verts(); ++vid)
    {
        tm.vert_add(hm.vert(vid));
    }

    for(uint pid=0; pid<hm.num_polys(); ++pid)
    {
        std::vector<uint> tets;
        hex_to_tets(hm.poly_verts_id(pid),tets);

        auto t = polys_from_serialized_vids(tets,4);
        for(auto tet : t) tm.poly_add(tet);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Subdivides a hexahedron either into 5 tets or into 6 tets
// (according to four poossible schemes). Split schemes are
// chosen in order to grant a global conforming tetrahedral
// mesh, where diagonals of quad faces are compatible across
// face-adjacent hexahedra
CINO_INLINE
void hex_to_tets(const std::vector<uint> & hex,
                       std::vector<uint> & tets)
{
    assert(hex.size()==8);
    tets.clear();

    // see Table 4 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    uint ref_h[8];
    switch(INDEX_OF(hex, std::min_element(hex.begin(),hex.end())))
    {
        case 0: ref_h[0] = hex[0];
                ref_h[1] = hex[1];
                ref_h[2] = hex[2];
                ref_h[3] = hex[3];
                ref_h[4] = hex[4];
                ref_h[5] = hex[5];
                ref_h[6] = hex[6];
                ref_h[7] = hex[7];
                break;

        case 1: ref_h[0] = hex[1];
                ref_h[1] = hex[0];
                ref_h[2] = hex[4];
                ref_h[3] = hex[5];
                ref_h[4] = hex[2];
                ref_h[5] = hex[3];
                ref_h[6] = hex[7];
                ref_h[7] = hex[6];
                break;

        case 2: ref_h[0] = hex[2];
                ref_h[1] = hex[1];
                ref_h[2] = hex[5];
                ref_h[3] = hex[6];
                ref_h[4] = hex[3];
                ref_h[5] = hex[0];
                ref_h[6] = hex[4];
                ref_h[7] = hex[7];
                break;

        case 3: ref_h[0] = hex[3];
                ref_h[1] = hex[0];
                ref_h[2] = hex[1];
                ref_h[3] = hex[2];
                ref_h[4] = hex[7];
                ref_h[5] = hex[4];
                ref_h[6] = hex[5];
                ref_h[7] = hex[6];
                break;

        case 4: ref_h[0] = hex[4];
                ref_h[1] = hex[0];
                ref_h[2] = hex[3];
                ref_h[3] = hex[7];
                ref_h[4] = hex[5];
                ref_h[5] = hex[1];
                ref_h[6] = hex[2];
                ref_h[7] = hex[6];
                break;

        case 5: ref_h[0] = hex[5];
                ref_h[1] = hex[1];
                ref_h[2] = hex[0];
                ref_h[3] = hex[4];
                ref_h[4] = hex[6];
                ref_h[5] = hex[2];
                ref_h[6] = hex[3];
                ref_h[7] = hex[7];
                break;

        case 6: ref_h[0] = hex[6];
                ref_h[1] = hex[2];
                ref_h[2] = hex[1];
                ref_h[3] = hex[5];
                ref_h[4] = hex[7];
                ref_h[5] = hex[3];
                ref_h[6] = hex[0];
                ref_h[7] = hex[4];
                break;

        case 7: ref_h[0] = hex[7];
                ref_h[1] = hex[3];
                ref_h[2] = hex[2];
                ref_h[3] = hex[6];
                ref_h[4] = hex[4];
                ref_h[5] = hex[0];
                ref_h[6] = hex[1];
                ref_h[7] = hex[5];
                break;

        default: assert(false);
    }

    // putting the min vertex in ref_h[0] fixes the diagonal for the three faces incident to it.
    // there are still three quad faces not incident to ref_h[0] that can be diagonalized in two
    // alternative ways, for a total of 8 alternative configurations. The code below applies a
    // topological rotation of the vertices in ref_h[], reducing the configurations to 4

    uint n = 0; // # of quad diagonals incident at v6
    std::bitset<3> bits = { 0b000 };
    if(std::min(ref_h[1],ref_h[6]) < std::min(ref_h[2],ref_h[5])) { ++n; bits.set(2); }
    if(std::min(ref_h[3],ref_h[6]) < std::min(ref_h[2],ref_h[7])) { ++n; bits.set(1); }
    if(std::min(ref_h[4],ref_h[6]) < std::min(ref_h[5],ref_h[7])) { ++n; bits.set(0); }

    if(bits==0b001 || bits==0b110) // rotate 120° around v0-v6
    {
        auto tmp = ref_h[1];
        ref_h[1] = ref_h[4];
        ref_h[4] = ref_h[3];
        ref_h[3] = tmp;

             tmp = ref_h[5];
        ref_h[5] = ref_h[7];
        ref_h[7] = ref_h[2];
        ref_h[2] = tmp;
    }

    if(bits==0b010 || bits==0b101) // rotate 240° around v0-v6
    {
        auto tmp = ref_h[1];
        ref_h[1] = ref_h[3];
        ref_h[3] = ref_h[4];
        ref_h[4] = tmp;

             tmp = ref_h[5];
        ref_h[5] = ref_h[2];
        ref_h[2] = ref_h[7];
        ref_h[7] = tmp;
    }

    // see Table 6 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    switch(n)
    {
        case 0: // t0
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[1]);
                tets.push_back(ref_h[2]);
                tets.push_back(ref_h[5]);
                // t1
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[2]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[5]);
                // t2
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[2]);
                tets.push_back(ref_h[3]);
                tets.push_back(ref_h[7]);
                // t3
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[5]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[4]);
                // t4
                tets.push_back(ref_h[2]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[5]);
                tets.push_back(ref_h[6]);
                break;

        case 1: // t0
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[5]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[4]);
                // t1
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[1]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[5]);
                // t2
                tets.push_back(ref_h[1]);
                tets.push_back(ref_h[6]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[5]);
                // t3
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[2]);
                tets.push_back(ref_h[3]);
                // t4
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[1]);
                tets.push_back(ref_h[2]);
                // t5
                tets.push_back(ref_h[1]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[6]);
                tets.push_back(ref_h[2]);
                break;

        case 2: // t0
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[4]);
                tets.push_back(ref_h[5]);
                tets.push_back(ref_h[6]);
                // t1
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[3]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[6]);
                // t2
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[4]);
                tets.push_back(ref_h[6]);
                // t3
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[1]);
                tets.push_back(ref_h[2]);
                tets.push_back(ref_h[5]);
                // t4
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[3]);
                tets.push_back(ref_h[6]);
                tets.push_back(ref_h[2]);
                // t5
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[6]);
                tets.push_back(ref_h[5]);
                tets.push_back(ref_h[2]);
                break;

        case 3: // t0
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[2]);
                tets.push_back(ref_h[3]);
                tets.push_back(ref_h[6]);
                // t1
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[3]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[6]);
                // t2
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[7]);
                tets.push_back(ref_h[4]);
                tets.push_back(ref_h[6]);
                // t3
                tets.push_back(ref_h[0]);
                tets.push_back(ref_h[5]);
                tets.push_back(ref_h[6]);
                tets.push_back(ref_h[4]);
                // t4
                tets.push_back(ref_h[1]);
                tets.push_back(ref_h[5]);
                tets.push_back(ref_h[6]);
                tets.push_back(ref_h[0]);
                // t5
                tets.push_back(ref_h[1]);
                tets.push_back(ref_h[6]);
                tets.push_back(ref_h[2]);
                tets.push_back(ref_h[0]);
                break;

        default: assert(false);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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
    assert(prism.size()==6);
    tets.clear();

    // see Table 2 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    uint ref_p[6];
    switch(INDEX_OF(prism, std::min_element(prism.begin(),prism.end())))
    {
        case 0: ref_p[0] = prism[0];
                ref_p[1] = prism[1];
                ref_p[2] = prism[2];
                ref_p[3] = prism[3];
                ref_p[4] = prism[4];
                ref_p[5] = prism[5];
                break;

        case 1: ref_p[0] = prism[1];
                ref_p[1] = prism[2];
                ref_p[2] = prism[0];
                ref_p[3] = prism[4];
                ref_p[4] = prism[5];
                ref_p[5] = prism[3];
                break;

        case 2: ref_p[0] = prism[2];
                ref_p[1] = prism[0];
                ref_p[2] = prism[1];
                ref_p[3] = prism[5];
                ref_p[4] = prism[3];
                ref_p[5] = prism[4];
                break;

        case 3: ref_p[0] = prism[3];
                ref_p[1] = prism[5];
                ref_p[2] = prism[4];
                ref_p[3] = prism[0];
                ref_p[4] = prism[2];
                ref_p[5] = prism[1];
                break;

        case 4: ref_p[0] = prism[4];
                ref_p[1] = prism[3];
                ref_p[2] = prism[5];
                ref_p[3] = prism[1];
                ref_p[4] = prism[0];
                ref_p[5] = prism[2];
                break;

        case 5: ref_p[0] = prism[5];
                ref_p[1] = prism[4];
                ref_p[2] = prism[3];
                ref_p[3] = prism[2];
                ref_p[4] = prism[1];
                ref_p[5] = prism[0];
                break;

        default: assert(false);
    }

    // see Table 3 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    if(std::min(ref_p[1],ref_p[5]) < std::min(ref_p[2],ref_p[4]))
    {
        // t0
        tets.push_back(ref_p[0]);
        tets.push_back(ref_p[1]);
        tets.push_back(ref_p[2]);
        tets.push_back(ref_p[5]);
        // t1
        tets.push_back(ref_p[0]);
        tets.push_back(ref_p[1]);
        tets.push_back(ref_p[5]);
        tets.push_back(ref_p[4]);
    }
    else
    {
        // t0
        tets.push_back(ref_p[0]);
        tets.push_back(ref_p[1]);
        tets.push_back(ref_p[2]);
        tets.push_back(ref_p[4]);
        // t1
        tets.push_back(ref_p[0]);
        tets.push_back(ref_p[4]);
        tets.push_back(ref_p[2]);
        tets.push_back(ref_p[5]);
    }
    // t2
    tets.push_back(ref_p[0]);
    tets.push_back(ref_p[4]);
    tets.push_back(ref_p[5]);
    tets.push_back(ref_p[3]);
}

}
