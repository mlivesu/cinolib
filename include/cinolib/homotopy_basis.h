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
 *
 * This file also contains tools to refine a greedy homotopy basis in
 * order to fully detach its loops and permit the realization of a
 * canonical polygonal schema. For details on this part, refer to:
 *
 *    Obtaining a Canonical Polygonal Schema from a
 *    Greedy Homotopy Basis with Minimal Mesh Refinement
 *    Marco Livesu
 *    (submitted)
 *
 * and
 *
 *    Globally Optimal Surface Mapping for Surfaces with Arbitrary Topology
 *    Xin Li, Yunfan Bao, Xiaohu Guo, Miao Jin, Xianfeng Gu and Hong Qin
 *    IEEE Transactions on Visualization and Computer Graphics, 2008
 */

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Mesh refinement strategies
enum
{
    EDGE_SPLIT_STRATEGY,   // Extact (no deviation from surface), but the mesh may grow a lot
    VERT_SPLIT_STRATEGY,   // Approximated (deviates from surface), but the mesh grows less
    HYBRID_SPLIT_STRATEGY  // Uses the approximated when possible (i.e. tiny surface deviation), and the exact otherwise
};

static const std::string ref_txt[3] =
{
    "EDGE_SPLIT_STRATEGY"  ,
    "VERT_SPLIT_STRATEGY"  ,
    "HYBRID_SPLIT_STRATEGY",
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
   uint  splits_tot    = 0; // total number of splits
   uint  splits_vert   = 0; // number of vert split operations
   uint  splits_edge   = 0; // number of edge split operations
   uint  splits_poly   = 0; // number of poly split operations
   uint  vert_val_max  = 0; // maximum per vertex valence
   float vert_val_avg  = 0; // average per vertex valence
   uint  num_verts_bef = 0; // verts in the input  mesh
   uint  num_verts_now = 0; // verts in the input  mesh
   uint  num_polys_bef = 0; // triangles in the output mesh
   uint  num_polys_now = 0; // triangles in the output mesh
}
RefinementStats;

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const RefinementStats & stats);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

typedef struct
{
    // INPUT: SETTINGS
    bool  globally_shortest  = false; // cost for globally shortest is O(n^2 log n). When this is set to true, root will contain the root of the globally shortest basis
    uint  root               = 0;     // cost for a base centered at root is O(n log n)

    // INPUT: REFINEMENT OPTIONS AND STATISTICS
    bool  detach_loops       = false;                 // refine mesh topology to detach loops traversing the same edges
    int   split_strategy     = HYBRID_SPLIT_STRATEGY; // sets the splitting strategy
    float coplanarity_thresh = 1;                     // sets the colpanarity threshold for the HYBRID_SPLIT_STRATEGY. Faces with dihedral angles lower than 5 degrees will be deemed coplanar and used as local support for vertex splitting
    RefinementStats refinement_stats;

    // OUTPUT: BASIS AND LENGTH
    std::vector<std::vector<uint>> loops;
    float length = 0.0; // length of the basis

    // OUTPUT: AUXILIARY DATA (may be useful for visual inspection/debugging)
    // note: tree and cotree reference the input mesh. If loops are detached they are useless
    std::vector<bool> tree;   // one element per edge. True if it is part of the tree, false otherwise
    std::vector<bool> cotree; // one element per edge. True if it is part of the cotree, false otherwise
}
HomotopyBasisData;

CINO_INLINE
std::ostream & operator<<(std::ostream & in, const HomotopyBasisData & data);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void homotopy_basis(AbstractPolygonMesh<M,V,E,P> & m,
                    HomotopyBasisData            & data);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
double homotopy_basis(AbstractPolygonMesh<M,V,E,P>   & m,
                      const uint                       root,
                      std::vector<std::vector<uint>> & basis,
                      std::vector<bool>              & tree,
                      std::vector<bool>              & cotree);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// globally detaches loops in the homotopy basis
template<class M, class V, class E, class P>
CINO_INLINE
void detach_loops(Trimesh<M,V,E,P>  & m,
                  HomotopyBasisData & data);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// locally detaches loops in the homotopy basis around vertex vid
template<class M, class V, class E, class P>
CINO_INLINE
uint detach_loops(Trimesh<M,V,E,P>  & m,
                  HomotopyBasisData & data,
                  const uint          vid);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void detach_loops_preproc(Trimesh<M,V,E,P>  & m,
                          HomotopyBasisData & data);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void detach_loops_postproc(Trimesh<M,V,E,P>  & m,
                           HomotopyBasisData & data);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// locally detaches loops in the homotopy basis around vertex vid using a triangle split
template<class M, class V, class E, class P>
CINO_INLINE
uint detach_loops_by_poly_split(Trimesh<M,V,E,P>  & m,
                                HomotopyBasisData & data,
                                const uint          e_in,
                                const uint          e_out);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// locally detaches loops in the homotopy basis around vertex vid using a sequence of edge splits
template<class M, class V, class E, class P>
CINO_INLINE
uint detach_loops_by_edge_split(Trimesh<M,V,E,P>        & m,
                                HomotopyBasisData       & data,
                                const std::vector<uint> & edge_fan);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// locally detaches loops in the homotopy basis around vertex vid using a vertex split
template<class M, class V, class E, class P>
CINO_INLINE
uint detach_loops_by_vert_split(Trimesh<M,V,E,P>  & m,
                                HomotopyBasisData & data,
                                const uint          e_in,
                                const uint          e_out,
                                const vec3d         new_pos = vec3d(inf_double, inf_double, inf_double));

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// planarity test for the vert split operator
template<class M, class V, class E, class P>
CINO_INLINE
bool polys_are_planar(const Trimesh<M,V,E,P>  & m,
                      const std::vector<uint> & edge_fan,
                      const float               coplanarity_tresh);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// point positioning for the vert split operator
template<class M, class V, class E, class P>
CINO_INLINE
bool find_position_within_fan(const Trimesh<M,V,E,P>  & m,
                              const std::vector<uint> & edge_fan,
                              const uint                v_mid,
                                    vec3d             & pos);
}

#ifndef  CINO_STATIC_LIB
#include "homotopy_basis.cpp"
#endif

#endif // CINO_HOMOTOPY_BASIS_H
