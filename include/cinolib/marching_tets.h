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
#ifndef CINO_MARCHING_TETS_H
#define CINO_MARCHING_TETS_H

#include <vector>
#include <map>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/ipair.h>
#include <cinolib/meshes/tetmesh.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void marching_tets(const Tetmesh<M,V,E,F,P> & m,
                   const double               isovalue,
                   std::vector<vec3d>       & verts,
                   std::vector<uint>        & tris,
                   std::vector<vec3d>       & norms);
}

#ifndef  CINO_STATIC_LIB
#include "marching_tets.cpp"
#endif

#endif // CINO_MARCHING_TETS_H
