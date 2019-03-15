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
#ifndef CINO_LSCM_H
#define CINO_LSCM_H

#include <cinolib/meshes/trimesh.h>
#include <cinolib/scalar_field.h>

namespace cinolib
{

/* Compute a Least Squares Conformal Map (LSCM). Here LSCM is implemented
 * as described in Section 2.2 of:
 *
 * Spectral Conformal Parameterization
 * Patrick Mullen, Yiying Tong, Pierre Alliez, Mathieu Desbrun
 * Symposium on Geometry Processing, 2008
 *
 * which is shown to be equivalent to the original formulation, presented in:
 *
 * Least Squares Conformal Maps for Automatic Texture Atlas Generation
 * Bruno Levy, Sylvain Petitjean, Nicolas Ray, Jérome Maillot
 * ACM Transactions on Graphics, 2002
 *
 * The function returns a 2 x #V scalar field containing the
 * serialized UV mesh coordinates (i.e. {u0,u1,..,un,v0,v1,..,vn}).
 *
 * NOTE: if the (at least two) required Dirichlet boundary conditions are
 * not provided in input by the user, they will be automatically selected
 * as the furthest pair of points along the boundary of the shape.
*/

template<class M, class V, class E, class P>
CINO_INLINE
ScalarField LSCM(const Trimesh<M,V,E,P>     & m,
                 const std::map<uint,vec2d> & bc = std::map<uint,vec2d>());
}

#ifndef  CINO_STATIC_LIB
#include "lscm.cpp"
#endif

#endif // CINO_LSCM_H
