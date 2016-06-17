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
#ifndef MARCHING_TETS_H
#define MARCHING_TETS_H

#include "../cinolib.h"
#include "tetmesh.h"

namespace cinolib
{


CINO_INLINE
void marching_tets(const Tetmesh       & m,
                   const double          isovalue,
                   std::vector<double> & coords,
                   std::vector<u_int>  & tris,
                   std::vector<double> & norm);

}


#ifndef  CINO_STATIC_LIB
#include "marching_tets.cpp"
#endif


#endif // MARCHING_TETS_H
