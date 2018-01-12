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
#ifndef CINO_EXPORT_SURFACE_H
#define CINO_EXPORT_SURFACE_H

#include <map>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/abstract_polyhedralmesh.h>
#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_surface(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                          AbstractPolygonMesh<M,V,E,F>      & srf);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_surface(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                          AbstractPolygonMesh<M,V,E,F>      & srf,
                          std::map<uint,uint>               & m2srf_vmap,
                          std::map<uint,uint>               & srf2m_vmap);

}

#ifndef  CINO_STATIC_LIB
#include "export_surface.cpp"
#endif

#endif // CINO_LABELING_H
