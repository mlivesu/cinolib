/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_VOXELIZE_H
#define CINO_VOXELIZE_H

#include <cinolib/meshes/abstract_polygonmesh.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

enum
{
    VOXEL_OUTSIDE,
    VOXEL_INSIDE,
    VOXEL_BOUNDARY,
    VOXEL_UNKNOWN
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct VoxelGrid
{
    int  * voxels; // array of voxels
    vec3i  dim;    // number of voxels along XYZ axis
    AABB   bbox;   // bounding box
    double len;    // per voxel edge length
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Voxelizes an object described by a surface mesh. Voxels will be deemed
// as being entirely inside, outside or traversed by the boundary of the
// input surface mesh, which can contain triangles, quads or general polygons.
//
// Memory allocation is performed internally. It is up the the user to
// release the memory when no longer needed (calling delete[] g.voxels)
//
template<class M, class V, class E, class P>
CINO_INLINE
void voxelize(const AbstractPolygonMesh<M,V,E,P> & m,
              const uint                           max_voxels_per_side,
                    VoxelGrid                    & g);


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Voxelizes an object described by an analytic function f. Voxels will be
// deemed as being entirely on the positive halfspace, negative halfspace
// or traversed by the zero level set of the function f.
//
// Memory allocation is performed internally. It is up the the user to
// release the memory when no longer needed (calling delete[] g.voxels)
//
CINO_INLINE
void voxelize(const std::function<double(const vec3d &p)> & f,
              const AABB                                  & volume,
              const uint                                    max_voxels_per_side,
                    VoxelGrid                             & g);
}

#ifndef  CINO_STATIC_LIB
#include "voxelize.cpp"
#endif

#endif // CINO_VOXELIZE_H
