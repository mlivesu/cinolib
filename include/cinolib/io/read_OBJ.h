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
#ifndef CINO_READ_OBJ_H
#define CINO_READ_OBJ_H

#include <sys/types.h>
#include <map>
#include <string>
#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/color.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & pos,         // vertex xyz positions
              std::vector<vec3d>             & tex,         // vertex uv(w) texture coordinates
              std::vector<vec3d>             & nor,         // vertex normals
              std::vector<std::vector<uint>> & poly_pos,    // polygons with references to pos
              std::vector<std::vector<uint>> & poly_tex,    // polygons with references to tex
              std::vector<std::vector<uint>> & poly_nor,    // polygons with references to nor
              std::vector<Color>             & poly_col);    // per polygon colors

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & verts,
              std::vector<std::vector<uint>> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OBJ(const char                     * filename,
              std::vector<vec3d>             & xyz,
              std::vector<vec3d>             & uvw,
              std::vector<std::vector<uint>> & poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_MTU(const char                  * filename,
              std::map<std::string,Color> & color_map);

}

#ifndef  CINO_STATIC_LIB
#include "read_OBJ.cpp"
#endif

#endif // CINO_READ_OBJ
