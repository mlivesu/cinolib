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
#ifndef CINO_CUT_ALONG_SEAMS_H
#define CINO_CUT_ALONG_SEAMS_H

#include <sys/types.h>
#include <vector>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

/* These methods serve to switch from separated representations for
 * each vertex property (position, texture coordinates, normals) to
 * an unified (OpenGL-like) vertex representation, where position,
 * texture coordinates and normals are all attributes of the same entity
 *
 * https://stackoverflow.com/questions/29867926/why-does-the-number-of-vt-and-v-elements-in-a-blender-obj-file-differ
*/

CINO_INLINE
void cut_mesh_along_seams(const std::vector<vec3d>             & v_attr_0,          // xyz or uvw or nor
                          const std::vector<vec3d>             & v_attr_1,          // xyz or uvw or nor
                          const std::vector<std::vector<uint>> & v2v_v_attr_0,      // connectivity attr #0
                          const std::vector<std::vector<uint>> & v2v_v_attr_1,      // connectivity attr #1
                                std::vector<vec3d>             & unified_v_attr_0,
                                std::vector<vec3d>             & unified_v_attr_1,
                                std::vector<std::vector<uint>> & unified_v2v);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void cut_mesh_along_seams(const std::vector<vec3d>             & v_attr_0,
                          const std::vector<vec3d>             & v_attr_1,
                          const std::vector<vec3d>             & v_attr_2,
                          const std::vector<std::vector<uint>> & v2v_attr_0,
                          const std::vector<std::vector<uint>> & v2v_attr_1,
                          const std::vector<std::vector<uint>> & v2v_attr_2,
                                std::vector<vec3d>             & unified_v_attr_0,
                                std::vector<vec3d>             & unified_v_attr_1,
                                std::vector<vec3d>             & unified_v_attr_2,
                                std::vector<std::vector<uint>> & unified_v2v);
}

#ifndef  CINO_STATIC_LIB
#include "cut_along_seams.cpp"
#endif

#endif // CINO_CUT_ALONG_SEAMS_H
