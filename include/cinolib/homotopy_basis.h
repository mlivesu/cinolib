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
#ifndef CINO_HOMOTOPY_BASIS_H
#define CINO_HOMOTOPY_BASIS_H

#include <cinolib/meshes/abstract_polygonmesh.h>
#include <cinolib/meshes/trimesh.h>

namespace cinolib
{

/* Compute the greedy homotopy basis of a surface mesh M
 * using the algorithm described in:
 *
 *   Dynamic Generators of Topologically Embedded Graphs
 *   David Eppstein
 *   ACM-SIAM Symposium on Discrete algorithms, 2003
 *
 *   Greedy optimal homotopy and homology generators
 *   Jeff Erickson and Kim Whittlesey
 *   ACM-SIAM symposium on Discrete algorithms, 2005
 */

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Mesh refinement strategies
enum
{
    EDGE_SPLIT_STRATEGY,  // Extact (no deviation from surface), but the mesh may grow a lot
    VERT_SPLIT_STRATEGY,  // Approximated (deviates from surface), but the mesh grows less
    MIXED_SPLIT_STRATEGY  // Uses the apprxomated when possible (i.e. tiny surface deviation), and the exact otherwise
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    // BASIS INFO/DATA
    bool  globally_shortest  = false;               // cost for globally shortest is O(n^2 log n). When this is set to true, root will contain the root of the globally shortest basis
    uint  root               = 0;                   // cost for a base centered at root is O(n log n)
    float length             = 0.0;                 // length of the basis

    // MESH REFINEMENT OPTIONS
    bool  detach_loops       = false;               // refine mesh topology to detach loops traversing the same edges
    int   split_strategy     = EDGE_SPLIT_STRATEGY; // sets the splitting strategy
    float coplanarity_thresh = 5;                   // sets the colpanarity threshold for the MIXED_SPLIT_STRATEGY. Faces with dihedral angles lower than 5 degrees will be deemed coplanar

    // AUXILIARY DATA (may be useful for visual inspection/debugging)
    std::vector<bool> tree;                         // one element per edge. True if it is part of the tree, false otherwise
    std::vector<bool> cotree;                       // one element per edge. True if it is part of the cotree, false otherwise
}
HomotopyBasisData;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis(AbstractPolygonMesh<M,V,E,P>   & m,
                    std::vector<std::vector<uint>> & basis,
                    HomotopyBasisData              & data);

}

#ifndef  CINO_STATIC_LIB
#include "homotopy_basis.cpp"
#endif

#endif // CINO_HOMOTOPY_BASIS_H
