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
#ifndef CINO_GRAPH_CUT_WRAP_H
#define CINO_GRAPH_CUT_WRAP_H

#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/abstract_mesh.h>

#ifdef CINOLIB_USES_GRAPH_CUT
#include <../external/graph_cut/GCoptimization.h>
#endif

namespace cinolib
{

/* This wraps the GraphCut algorithm, run on the dual graph of a general mesh
 * (i.e. one node for each polygon/polyhedron, one arc for each pair of adjacent
 * polygons/polyhedra).
 *
 * - data_term is a vector containing a serialized lists of costs (per node, per label):
 *
 *      <node#0,label#0>,...,<node#0,label#n>,<node#1,label#0>,...,<node#1,label#0>,...
 *
 * - smooth_term is a pointer to a function f:
 *
 *      double f(int site_1, int site_2, int label_1, int label_2, void *smooth_data)
 *
*/

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> graph_cut_wrap(const  AbstractMesh<M,V,E,P>             & m,
                                const int                                 n_labels,
                                const std::vector<double>               & data_term,
                                      GCoptimization::SmoothCostFnExtra   smooth_term,
                                      void                              * smooth_data = NULL);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/* Constrained version (some sites can receive only a subset of the available labels).
 *
 * - feasible_region is a vector containing one entry for each label. Each entry is a vector
 *   of node ids, containing all the nodes that can receive that label.
 *
 * - data_term is similar to feasible_region, but instead of the node ids, contains the costs
 *   of each node to receive that label
 *
*/

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> graph_cut_wrap(const AbstractMesh<M,V,E,P>             & m,
                                const int                                 n_labels,
                                const std::vector<std::vector<uint>>    & feasible_region,
                                const std::vector<std::vector<double>>  & data_term,
                                      GCoptimization::SmoothCostFnExtra   smooth_term,
                                      void                              * smooth_data = NULL);

}

#ifndef  CINO_STATIC_LIB
#include "graph_cut_wrap.cpp"
#endif

#endif // CINO_GRAPH_CUT_WRAP_H
