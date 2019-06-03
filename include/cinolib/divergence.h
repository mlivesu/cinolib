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
*                                                                               *
*                                                                               *
*     Claudio Mancinelli (claudio.f.mancinelli@gmail.com)                       *
*                                                                               *
*     University of Genoa                                                       *
*     Via Dodecaneso, 35                                                        *
*     16146 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_DIVERGENCE_H
#define CINO_DIVERGENCE_H

#include <Eigen/Sparse>
#include <cinolib/scalar_field.h>
#include <cinolib/meshes/abstract_polyhedralmesh.h>
#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

/* Compute the divergence of a scalar field embedded on a mesh by transposing the
 * gradient matrix of the same mesh, and multiplying it by the gradient of the field,
 * as explained in
 *
 *   A Heat Flow Based Relaxation Scheme for n Dimensional Discrete Hyper Surfaces
 *   Marco Livesu
 *   Computers & Graphics (2018)
 *
 *   and
 *
 *   A Comparison of Methods for Gradient Field Estimation on Simplicial Meshes
 *   Claudio Mancinelli, Marco Livesu, Enrico Puppo
 *   Computers & Graphics (2019)
*/


template<class M, class V, class E, class P>
CINO_INLINE
ScalarField divergence(const AbstractPolygonMesh<M,V,E,P> & m, ScalarField & f);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
ScalarField divergence(const AbstractPolyhedralMesh<M,V,E,F,P> & m, ScalarField & f);

}

#ifndef  CINO_STATIC_LIB
#include "divergence.cpp"
#endif

#endif // CINO_DIVERGENCE_H
