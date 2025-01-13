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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void voxel_grid_init(     VoxelGrid  & g,
                    const uint         dim[3],
                    const AABB       & bbox)
{
    assert(g.voxels==nullptr);

    g.dim[0] = dim[0];
    g.dim[1] = dim[1];
    g.dim[2] = dim[2];
    g.bbox   = bbox;

    uint max_voxels_per_side = std::max(dim[0],std::max(dim[1],dim[2]));
    g.len = g.bbox.delta().max_entry() / max_voxels_per_side;

    uint size = dim[0]*dim[1]*dim[2];
    g.voxels  = new int[size](); // https://stackoverflow.com/questions/12553154/initial-value-of-dynamically-allocated-memory-in-c
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint voxel_corner_index(const uint dim[3],
                        const uint ijk[3],
                        const uint corner)
{
    return serialize_3D_index(ijk[0] + uint(REFERENCE_HEX_VERTS[corner][0]),
                              ijk[1] + uint(REFERENCE_HEX_VERTS[corner][1]),
                              ijk[2] + uint(REFERENCE_HEX_VERTS[corner][2]),
                              dim[1]+1, dim[2]+1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint voxel_corner_index(const uint dim[3],
                        const uint index,
                        const uint corner)
{
    vec3u ijk = deserialize_3D_index(index, dim[1], dim[2]);
    return voxel_corner_index(dim, ijk.ptr(), corner);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint voxel_corner_index(const VoxelGrid & g,
                        const uint        ijk[3],
                        const uint        corner)
{
    return voxel_corner_index(g.dim, ijk, corner);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
uint voxel_corner_index(const VoxelGrid & g,
                        const uint        index,
                        const uint        corner)
{
    return voxel_corner_index(g.dim, index, corner);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d voxel_corner_xyz(const AABB   & bbox,
                       const double & len,
                       const uint     ijk[3],
                       const uint     corner)
{
    return vec3d(bbox.min[0] + len*ijk[0] + len*REFERENCE_HEX_VERTS[corner][0],
                 bbox.min[1] + len*ijk[1] + len*REFERENCE_HEX_VERTS[corner][1],
                 bbox.min[2] + len*ijk[2] + len*REFERENCE_HEX_VERTS[corner][2]);
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

CINO_INLINE
AABB voxel_bbox(const AABB   & bbox,
                const double & len,
                const uint     ijk[3])
{
    return AABB(voxel_corner_xyz(bbox,len,ijk,0),
                voxel_corner_xyz(bbox,len,ijk,6));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
AABB voxel_bbox(const VoxelGrid & g,
                const uint        ijk[3])
{
    return AABB(voxel_corner_xyz(g,ijk,0),
                voxel_corner_xyz(g,ijk,6));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
AABB voxel_bbox(const VoxelGrid & g,
                const uint        index)
{
    vec3u ijk = deserialize_3D_index(index, g.dim[1], g.dim[2]);
    return voxel_bbox(g, ijk.ptr());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> voxel_n6(const uint dim[3],
                           const uint ijk[3])
{
    std::vector<uint> n6;
    n6.reserve(6);

    if(ijk[0]>0) n6.push_back(serialize_3D_index(ijk[0]-1,ijk[1]  ,ijk[2]  ,dim[1],dim[2]));
    if(ijk[1]>0) n6.push_back(serialize_3D_index(ijk[0]  ,ijk[1]-1,ijk[2]  ,dim[1],dim[2]));
    if(ijk[2]>0) n6.push_back(serialize_3D_index(ijk[0]  ,ijk[1]  ,ijk[2]-1,dim[1],dim[2]));

    if(ijk[0]+1<dim[0]) n6.push_back(serialize_3D_index(ijk[0]+1,ijk[1]  ,ijk[2]  ,dim[1],dim[2]));
    if(ijk[1]+1<dim[1]) n6.push_back(serialize_3D_index(ijk[0]  ,ijk[1]+1,ijk[2]  ,dim[1],dim[2]));
    if(ijk[2]+1<dim[2]) n6.push_back(serialize_3D_index(ijk[0]  ,ijk[1]  ,ijk[2]+1,dim[1],dim[2]));

    return n6;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> voxel_n6(const uint dim[3],
                           const uint index)
{
    vec3u ijk = deserialize_3D_index(index, dim[1], dim[2]);
    return voxel_n6(dim,ijk.ptr());
}

}

