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

#include <vector>
#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

/* This switches from OBJ vertex representation (xyz coordinates and
 * texture coordinates represented as different entities) to OpenGL
 * vertex representation (unified vertex entity, having both xyz and
 * texture coordinates).
 *
 * It is important when reading textured meshes from OBJ files, where the
 * mesh may not be homeomorphic to a disk from a geometric point of view
 * but often contain seams (vertices with multiple texture coordinates) that
 * allow to flatten it to a plane from a texturing point of view.
 *
 * What it does is to create one unified vertex entity for each unique pair
 * of xyz and uv(w) coordinates, updating the references in the polygon list
 * accordingly.
 *
 * https://stackoverflow.com/questions/29867926/why-does-the-number-of-vt-and-v-elements-in-a-blender-obj-file-differ
*/

CINO_INLINE
void cut_mesh_along_seams(const std::vector<vec3d>             & xyz,
                          const std::vector<vec3d>             & uvw,
                          const std::vector<std::vector<uint>> & xyz_poly,
                          const std::vector<std::vector<uint>> & uvw_poly,
                                std::vector<vec3d>             & unified_xyz,
                                std::vector<vec3d>             & unified_uvw,
                                std::vector<std::vector<uint>> & unified_poly);

}

#ifndef  CINO_STATIC_LIB
#include "cut_along_seams.cpp"
#endif

#endif // CINO_CUT_ALONG_SEAMS_H
