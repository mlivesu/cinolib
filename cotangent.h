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
#ifndef COTANGENT_H
#define COTANGENT_H

#include "cinolib.h"
#include "trimesh/trimesh.h"
#include "tetmesh/tetmesh.h"


namespace cinolib
{

template<class Mesh>
CINO_INLINE
void cotangent_weights(const Mesh &, const int, std::vector<int> &, std::vector<double> &)
{
    std::cerr << "WARNING! - Cotangent weights are not available for this mesh type yet!" << std::endl;
    assert(false);
}


/* Template Specializations.
 *
 * As a reference for the tetmesh version of cotangent weights, see:
 *
 * Gradient field based inhomogeneous volumetric mesh deformation for maxillofacial surgery simulation
 * Sheng-hui Liao, Ruo-feng Tong, Jin-xiang Dong, Fu-dong Zhu
 * Computer & Graphics, 2009
*/

template<>
CINO_INLINE
void cotangent_weights<Trimesh>(const Trimesh       & m,
                                const int             vid,
                                std::vector<int>    & nbrs,
                                std::vector<double> & wgts);

template<>
CINO_INLINE
void cotangent_weights<Tetmesh>(const Tetmesh       & m,
                                const int             vid,
                                std::vector<int>    & nbrs,
                                std::vector<double> & wgts);

}

#ifndef  CINO_STATIC_LIB
#include "cotangent.cpp"
#endif


#endif // COTANGENT_H
