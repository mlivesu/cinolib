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
#ifndef CINO_TANGENTIAL_SMOOTHING_H
#define CINO_TANGENTIAL_SMOOTHING_H

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/trimesh.h>

namespace cinolib
{

/* These functions implement per vertex tangential smoothing,
 * as described in the paper:
 *
 * A Remeshing Approach to Multiresolution Modeling
 * M.Botsch, L.Kobbelt
 * Symposium on Geomtry Processing, 2004
*/

template<class M, class V, class E, class P>
CINO_INLINE
void tangential_smoothing(Trimesh<M,V,E,P> & m, const uint vid);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void tangential_smoothing(Trimesh<M,V,E,P> & m);

}

#ifndef  CINO_STATIC_LIB
#include "tangential_smoothing.cpp"
#endif

#endif // CINO_TANGENTIAL_SMOOTHING_H
