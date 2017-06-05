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
#ifndef CINO_COTANGENT_H
#define CINO_COTANGENT_H

#include <cinolib/cinolib.h>
#include <cinolib/meshes/trimesh/trimesh.h>
#include <cinolib/meshes/tetmesh/tetmesh.h>

namespace cinolib
{

template<class Mesh>
CINO_INLINE
void cotangent_weights(const Mesh &, const uint, std::vector<uint> &, std::vector<double> &)
{
    std::cerr << "WARNING! - Cotangent weights are not available for this mesh type!" << endl;
    assert(false);
}

}

#ifndef  CINO_STATIC_LIB
#include "cotangent.cpp"
#endif


#endif // CINO_COTANGENT_H
