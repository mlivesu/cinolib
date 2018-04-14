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
#ifndef CINO_REMESH_BOTSCH_KOBBELT_2004_H
#define CINO_REMESH_BOTSCH_KOBBELT_2004_H

#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/gui/qt/glcanvas.h>

namespace cinolib
{

/* This method implements one iteration of the remeshing algorithm
 * described in:
 *
 * A Remeshing Approach to Multiresolution Modeling
 * M.Botsch, L.Kobbelt
 * Symposium on Geomtry Processing, 2004
*/

template<class M, class V, class E, class P>
CINO_INLINE
void remesh_Botsch_Kobbelt_2004(DrawableTrimesh<M,V,E,P> & m,
                                const double               target_edge_length = -1,
                                const bool                 preserve_marked_features = true);
}

#ifndef  CINO_STATIC_LIB
#include "remesh_BotschKobbelt2004.cpp"
#endif

#endif // CINO_REMESH_BOTSCH_KOBBELT_2004_H
