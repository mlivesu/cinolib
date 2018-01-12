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
#ifndef CINO_READ_HEXEX_H
#define CINO_READ_HEXEX_H

#include <sys/types.h>
#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

/* Read a tetrahedral mesh and an integer grid map associated to it in the
 * libHexEx format (https://www.graphics.rwth-aachen.de/software/libHexEx).
 * For further deails please refer to the original paper:
 *
 *     HexEx: Robust Hexahedral Mesh Extraction
 *     M.Lyon, D.Bommes and L.Kobbelt
 *     ACM TOG (SIGGRAPH 2016)
*/

CINO_INLINE
void read_HEXEX(const char         * filename,
                std::vector<vec3d> & verts,
                std::vector<uint>  & tets,        // serialized tets (4 vids per tet)
                std::vector<vec3d> & tets_param); // tets param (4 points per tet)

}

#ifndef  CINO_STATIC_LIB
#include "read_HEXEX.cpp"
#endif

#endif // CINO_READ_HEXEX
