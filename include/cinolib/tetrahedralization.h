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
#ifndef CINO_TETRAHEDRALIZATION_H
#define CINO_TETRAHEDRALIZATION_H

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>

/* The split schemes implemented here are largely based on:
 *
 * How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra
 * J.Dompierre, P.Labbé, M.G. Vallet, R.Camarero
 * International Meshing Roundtable, pp.195-204, 1999
*/

namespace cinolib
{

/* This assumes the prism has a triangular cross-section.
 * The vertices are ordered as follows:
 *
 *      v2             v5
 *    /    \         /    \
 *  v0 --- v1      v3 --- v4
 *
 *   bot base       top base
*/
CINO_INLINE
void tetrahedralize_prism(uint prism[], std::vector<uint> & tets);


}

#ifndef  CINO_STATIC_LIB
#include "tetrahedralization.cpp"
#endif

#endif // CINO_TETRAHEDRALIZATION_H
