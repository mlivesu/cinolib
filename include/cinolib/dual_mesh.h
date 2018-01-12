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
#ifndef CINO_DUAL_MESH
#define CINO_DUAL_MESH

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/polygonmesh.h>
#include <cinolib/meshes/polyhedralmesh.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void dual_mesh(const AbstractPolyhedralMesh<M,V,E,F,P> & primal,
                     Polyhedralmesh<M,V,E,F,P>         & dual,
               const bool                                with_clipped_cells); // consider/discard boundary vertices

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void dual_mesh(const AbstractPolyhedralMesh<M,V,E,F,P> & primal,
                     std::vector<vec3d>                & dual_verts,
                     std::vector<std::vector<uint>>    & dual_faces,
                     std::vector<std::vector<uint>>    & dual_polys,
                     std::vector<std::vector<bool>>    & dual_polys_winding,
               const bool                                with_clipped_cells); // consider/discard boundary vertices

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dual_mesh(const AbstractPolygonMesh<M,V,E,P>   & primal,
                     std::vector<vec3d>             & dual_verts,
                     std::vector<std::vector<uint>> & dual_faces,
               const bool                             with_clipped_cells); // consider/discard boundary vertices

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void dual_mesh(const AbstractPolygonMesh<M,V,E,P> & primal,
                     Polygonmesh<M,V,E,P>         & dual,
               const bool                           with_clipped_cells); // consider/discard boundary vertices

}

#ifndef  CINO_STATIC_LIB
#include "dual_mesh.cpp"
#endif

#endif // CINO_DUAL_MESH
