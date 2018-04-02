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
#ifndef CINO_COARSE_LAYOUT_H
#define CINO_COARSE_LAYOUT_H

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/quadmesh.h>
#include <cinolib/meshes/hexmesh.h>

/* Compute coarse layouts for structured surface (quad)
 * and volume (hexa) meshes. The layout will be encoded
 * in the labels of each element.
 *
 * One label per domain will be generated, and all the
 * mesh elements will receive a label corresponding to
 * the domain they blong to.
 *
 * Interfaces between adjacent domains will also be marked.
 * For surfaces, boundary edges and vertices will be marked.
 * Similarly, for volumes boundary edges, vertices and faces
 * will be marked.
*/

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void compute_coarse_quad_layout(Quadmesh<M,V,E,P> & m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void compute_coarse_hex_layout(Hexmesh<M,V,E,P> & m);

}

#ifndef  CINO_STATIC_LIB
#include "coarse_layout.cpp"
#endif

#endif // CINO_COARSE_LAYOUT_H
