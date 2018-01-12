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
#ifndef CINO_MEAN_CURV_FLOW_H
#define CINO_MEAN_CURV_FLOW_H

#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

/* For the differences between cassical mean curvature flow (MCF) and
 * conformalized mean curvature flow (cMCF), please refer to:
 *
 * Can Mean-Curvature Flow be Modified to be Non-singular?
 * Michael Kazhdan, Jake Solomon and Mirela Ben-Chen
 * Computer Graphics Forum, 31(5), 2012.
*/

template<class M, class V, class E, class P>
CINO_INLINE
void MCF(AbstractPolygonMesh<M,V,E,P> & m,
         const uint                     n_iters,
         const double                   time_scalar = 0.01, // I suggest very small steps for the conformalized version
         const bool                     conformalized = true);

}

#ifndef  CINO_STATIC_LIB
#include "mean_curv_flow.cpp"
#endif

#endif // CINO_MEAN_CURV_FLOW_H
