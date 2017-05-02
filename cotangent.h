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
#ifndef CINO_COTANGENT_H
#define CINO_COTANGENT_H

#include <cinolib/cinolib.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/meshes/tetmesh/tetmesh.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
void cotangent_weights(const Mesh &, const int, std::vector<int> &, std::vector<double> &)
{
    std::cerr << "WARNING! - Cotangent weights are not available for this mesh type!" << endl;
    assert(false);
}

}

#ifndef  CINO_STATIC_LIB
#include "cotangent.cpp"
#endif


#endif // CINO_COTANGENT_H
