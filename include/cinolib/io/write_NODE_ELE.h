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
#ifndef CINO_WRITE_NODE_ELE_H
#define CINO_WRITE_NODE_ELE_H

#include <sys/types.h>
#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

CINO_INLINE
void write_NODE_ELE(const char                           * basename,
                    const std::vector<vec3d>             & verts,
                    const std::vector<std::vector<uint>> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_NODE_ELE_2D(const char                           * basename,
                       const std::vector<vec3d>             & verts, // z will be discarded
                       const std::vector<std::vector<uint>> & poly);

}

#ifndef  CINO_STATIC_LIB
#include "write_NODE_ELE.cpp"
#endif

#endif // CINO_WRITE_NODE_ELE_H
