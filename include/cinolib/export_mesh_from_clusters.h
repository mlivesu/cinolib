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
#ifndef CINO_EXPORT_MESH_FROM_CLUSTERS_H
#define CINO_EXPORT_MESH_FROM_CLUSTERS_H

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/meshes.h>

/*
* These methods take in input a labeled surface/volumetric mesh M and export
* a mesh having one polygon/polyhedron for each cluster of elements having same label.
*/

namespace cinolib
{

// NOTE: this assumes clusters form simply connected polygons (i.e. no holes)
//
template<class M, class V, class E, class P>
CINO_INLINE
void export_mesh_from_clusters(const AbstractPolygonMesh<M,V,E,P> & m_in,
                                     Polygonmesh<M,V,E,P>         & m_out);

}

#ifndef  CINO_STATIC_LIB
#include "export_mesh_from_clusters.cpp"
#endif

#endif // CINO_EXPORT_MESH_FROM_CLUSTERS_H
