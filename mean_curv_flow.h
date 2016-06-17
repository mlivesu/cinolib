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
#ifndef MEAN_CURV_FLOW_H
#define MEAN_CURV_FLOW_H

#include "cinolib.h"
#include "trimesh/trimesh.h"

namespace cinolib
{

/* For the differences between cassical mean curvature flow (MCF) and
 * conformalized mean curvature flow (cMCF), please refer to:
 *
 * Can Mean-Curvature Flow be Modified to be Non-singular?
 * Michael Kazhdan, Jake Solomon and Mirela Ben-Chen
 * Computer Graphics Forum, 31(5), 2012.
*/

CINO_INLINE
void MCF(Trimesh      & m,
         const int      n_iters,
         const double   time = 1e-3,
         const bool     conformalized = true);

}

#ifndef  CINO_STATIC_LIB
#include "mean_curv_flow.cpp"
#endif

#endif // MEAN_CURV_FLOW_H
