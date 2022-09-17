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
#ifndef CINO_VOXEL_GRID_H
#define CINO_VOXEL_GRID_H

#include <cinolib/geometry/vec_mat.h>
#include <cinolib/geometry/aabb.h>

namespace cinolib
{

struct VoxelGrid
{
    int  * voxels = nullptr; // array of voxels
    uint   dim[3];           // number of voxels along XYZ axis
    AABB   bbox;             // bounding box
    double len;              // per voxel edge length

    ~VoxelGrid(){ delete[] voxels; }
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Per voxel flags
enum
{
    VOXEL_OUTSIDE  = 0x00000001,
    VOXEL_INSIDE   = 0x00000010,
    VOXEL_BOUNDARY = 0x00000100,
    VOXEL_UNKNOWN  = 0x00001000
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint voxel_corner_index(const VoxelGrid & g,
                        const uint        ijk[3],
                        const uint        corner);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint voxel_corner_index(const VoxelGrid & g,
                        const uint        index,
                        const uint        corner);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d voxel_corner_xyz(const VoxelGrid & g,
                       const uint        ijk[3],
                       const uint        corner);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d voxel_corner_xyz(const VoxelGrid & g,
                       const uint        index,
                       const uint        corner);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

AABB voxel_bbox(const VoxelGrid & g,
                const uint        ijk[3]);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

AABB voxel_bbox(const VoxelGrid & g,
                const uint        index);

}

#ifndef  CINO_STATIC_LIB
#include "voxel_grid.cpp"
#endif

#endif // CINO_VOXEL_GRID_H
