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
#ifndef SMOOTHING_H
#define SMOOTHING_H

#include <cinolib/cinolib.h>
#include <cinolib/laplacian.h>

namespace cinolib
{

/*
 * Implementation of Taubin Smoothing.
 *
 * Reference:
 * Curve and Surface Smoothing Without Shrinkage
 * Gabriel Taubin
 * Proceeedings of IEEE 5th International Conference on Computer Vision, 1995.
 *
*/

template<class Mesh>
CINO_INLINE
void smooth_taubin(Mesh                & m,
                   const int             mode          =  COTANGENT,
                   const std::set<int>   do_not_smooth =  std::set<int>(),
                   const int             n_iters       =  10,
                   const double          lambda        =  0.9,
                   const double          mu            = -0.9);

}

#ifndef  CINO_STATIC_LIB
#include "smoothing.cpp"
#endif

#endif // SMOOTHING_H
