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
#ifndef CINO_EXPORT_CLUSTER_H
#define CINO_EXPORT_CLUSTER_H

#include <map>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/meshes.h>

/* 
* These methods take in input a surface/volumetric mesh M and a label L, and output
* a submesh M' containing all and only the polygons/polyhedra having L as label.
*/

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P> & m,
                    const std::set<int>                & labels,
                          AbstractPolygonMesh<M,V,E,P> & subm);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P>  & m,
                    const std::set<int>                & labels,
                          AbstractPolygonMesh<M,V,E,P>  & subm,
                          std::map<uint,uint>           & m2subm_vmap,
                          std::map<uint,uint>           & subm2m_vmap);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P> & m,
                    const int                            label,
                          AbstractPolygonMesh<M,V,E,P> & subm);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P>  & m,
                    const int                             label,
                          AbstractPolygonMesh<M,V,E,P>  & subm,
                          std::map<uint,uint>           & m2subm_vmap,
                          std::map<uint,uint>           & subm2m_vmap);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                    const std::set<int>                     & labels,
                          AbstractPolyhedralMesh<M,V,E,F,P> & subm);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P>  & m,
                    const std::set<int>                      & labels,
                          AbstractPolyhedralMesh<M,V,E,F,P>  & subm,
                          std::map<uint,uint>                & m2subm_vmap,
                          std::map<uint,uint>                & subm2m_vmap);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                    const int                                 label,
                          AbstractPolyhedralMesh<M,V,E,F,P> & subm);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P>  & m,
                    const int                                  label,
                          AbstractPolyhedralMesh<M,V,E,F,P>  & subm,
                          std::map<uint,uint>                & m2subm_vmap,
                          std::map<uint,uint>                & subm2m_vmap);
}

#ifndef  CINO_STATIC_LIB
#include "export_cluster.cpp"
#endif

#endif // CINO_EXPORT_CLUSTER_H
