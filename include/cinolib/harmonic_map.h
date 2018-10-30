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
#ifndef CINO_HARMONIC_MAP_H
#define CINO_HARMONIC_MAP_H

#include <sys/types.h>
#include <vector>
#include <map>
#include <cinolib/cino_inline.h>
#include <cinolib/scalar_field.h>
#include <cinolib/linear_solvers.h>
#include <cinolib/symbols.h>

namespace cinolib
{

/*
 * Solve the (n)-harmonic problem  L phi = 0,
 * subject to certain Dirichlet boundary conditions
 *
 * The parameter n control the smoothness of phi. That is:
 *
 * n = 1  | harmonic    | C^0 at boundary conditions, C^1 everywhere else
 * n = 2  | biharmonic  | C^1 at boundary conditions, C^2 everywhere else
 * n = 3  | triharmonic | C^2 at boundary conditions, C^3 everywhere else
 * ...
*/

template<class M, class V, class E, class P>
CINO_INLINE
ScalarField harmonic_map(const AbstractMesh<M,V,E,P> & m,
                         const std::map<uint,double> & bc,
                         const uint                    n = 1,
                         const int                     laplacian_mode = COTANGENT,
                         const int                     solver = SIMPLICIAL_LLT);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<vec3d> harmonic_map_3d(const AbstractMesh<M,V,E,P> & m,
                                   const std::map<uint,vec3d>  & bc,
                                   const uint                    n = 1,
                                   const int                     laplacian_mode = COTANGENT,
                                   const int                     solver = SIMPLICIAL_LLT);
}

#ifndef  CINO_STATIC_LIB
#include "harmonic_map.cpp"
#endif

#endif // CINO_HARMONIC_MAP_H
