/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#ifndef TETRAHEDRALIZATION_H
#define TETRAHEDRALIZATION_H

#include <cinolib/cinolib.h>

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
void tetrahedralize_prism(int prism[], std::vector<uint> & tets);


}

#ifndef  CINO_STATIC_LIB
#include "tetrahedralization.cpp"
#endif

#endif // TETRAHEDRALIZATION_H
