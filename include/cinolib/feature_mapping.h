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
#ifndef CINO_FEATURE_MAPPING_H
#define CINO_FEATURE_MAPPING_H

#include <cinolib/meshes/meshes.h>

namespace cinolib
{

/* Given a mesh and its feature network (i.e., a set of chains made of consecutive mesh vertices),
 * this function maps the network onto a target mesh, finding a corresponding set of vertices in
 * the new connectivity.
 *
 * The algorithm maps feature endpoints first, then samples all the curves and finds the closest points
 * in the target geometry. These points are used to generate a distance field, which is eventually used
 * to trace the curves in the new connectivity using Dijkstra. A similar method was firstly proposed in
 *
 *    Feature Preserving Octree-Based Hexahedral Meshing
 *    Xifeng Gao, Hanxiao Shen, Daniele Panozzo
 *    Computer Graphics Forum (SGP), 2019
 *
 * WARNING: the algorithm does not guarantee that ALL the input curves will be mapped. Edge conflicts
 * during the shortest path tracing may arise. The method returns true if all input features have been
 * sucecssfully mapped, false otherwise.
*/

template<class M1, class V1, class E1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
bool feature_mapping(const AbstractPolygonMesh<M1,V1,E1,P1> & m_source,
                     const std::vector<std::vector<uint>>   & f_source,
                           AbstractPolygonMesh<M2,V2,E2,P2> & m_target,
                           std::vector<std::vector<uint>>   & f_target);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M1, class V1, class E1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
bool feature_mapping(const AbstractPolygonMesh<M1,V1,E1,P1> & m_source,
                           AbstractPolygonMesh<M2,V2,E2,P2> & m_target);
}

#ifndef  CINO_STATIC_LIB
#include "feature_mapping.cpp"
#endif

#endif // CINO_FEATURE_MAPPING_H
