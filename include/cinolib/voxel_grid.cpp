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
#include <cinolib/voxel_grid.h>
#include <cinolib/standard_elements_tables.h>
#include <cinolib/serialize_index.h>

namespace cinolib
{

CINO_INLINE
uint voxel_corner_index(const VoxelGrid & g,
                        const uint        ijk[3],
                        const uint        corner)
{
    return serialize_3D_index(ijk[0] + REFERENCE_HEX_VERTS[corner][0],
                              ijk[1] + REFERENCE_HEX_VERTS[corner][1],
                              ijk[2] + REFERENCE_HEX_VERTS[corner][2],
                              g.dim[1]+1, g.dim[2]+1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint voxel_corner_index(const VoxelGrid & g,
                        const uint        index,
                        const uint        corner)
{
    vec3u ijk = deserialize_3D_index(index, g.dim[1], g.dim[2]);
    return voxel_corner_index(g, ijk.ptr(), corner);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d voxel_corner_xyz(const VoxelGrid & g,
                       const uint        ijk[3],
                       const uint        corner)
{
    return vec3d(g.bbox.min[0] + g.len*ijk[0] + g.len*REFERENCE_HEX_VERTS[corner][0],
                 g.bbox.min[1] + g.len*ijk[1] + g.len*REFERENCE_HEX_VERTS[corner][1],
                 g.bbox.min[2] + g.len*ijk[2] + g.len*REFERENCE_HEX_VERTS[corner][2]);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d voxel_corner_xyz(const VoxelGrid & g,
                       const uint        index,
                       const uint        corner)
{
    vec3u ijk = deserialize_3D_index(index, g.dim[1], g.dim[2]);
    return voxel_corner_xyz(g, ijk.ptr(), corner);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

AABB voxel_bbox(const VoxelGrid & g,
                const uint        ijk[3])
{
    return AABB(voxel_corner_xyz(g,ijk,0),
                voxel_corner_xyz(g,ijk,6));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

AABB voxel_bbox(const VoxelGrid & g,
                const uint        index)
{
    vec3u ijk = deserialize_3D_index(index, g.dim[1], g.dim[2]);
    return voxel_bbox(g, ijk.ptr());
}

}

