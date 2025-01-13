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
#include <cinolib/voxel_grid_to_hexmesh.h>

namespace cinolib
{

// Converts a voxel grid into a hexahedral mesh. Users can select what voxel types
// can be retained in the output mesh. Legal choices are combinations of the following
// types:
//         - VOXEL_INSIDE   => fully inside the voxelized object
//         - VOXEL_OUTSIDE  => fully outside the voxelized object
//         - VOXEL_BOUNDARY => traversed by the boundary of the voxelized object
//
template<class M, class V, class E, class F, class P>
CINO_INLINE
void voxel_grid_to_hexmesh(const VoxelGrid                         & g,
                                 AbstractPolyhedralMesh<M,V,E,F,P> & m,
                           const int voxel_types)
{
    uint n_voxel =  g.dim[0]   * g.dim[1]   * g.dim[2];
    uint n_verts = (g.dim[0]+1)*(g.dim[1]+1)*(g.dim[2]+1);
    std::vector<int> vert_map(n_verts,-1); // to keep track of already existing vertices
    for(uint id=0; id<n_voxel; ++id)
    {
        if(voxel_types==VOXEL_ANY || g.voxels[id] & voxel_types)
        {

            vec3u ijk = deserialize_3D_index(id,g.dim[1],g.dim[2]);

            std::vector<uint> verts(8);
            std::vector<uint> faces(6);
            std::vector<bool> winding(6,false);

            // make verts
            for(uint off=0; off<8; ++off)
            {
                uint index = voxel_corner_index(g, ijk.ptr(), off);
                if(vert_map[index]<0)
                {
                    vec3d p = voxel_corner_xyz(g,ijk.ptr(),off);
                    vert_map[index] = m.vert_add(p);
                }
                verts[off] = vert_map[index];
            }

            // make faces
            for(uint off=0; off<6; ++off)
            {
                std::vector<uint> face =
                {
                    verts[HEXA_FACES[off][0]],
                    verts[HEXA_FACES[off][1]],
                    verts[HEXA_FACES[off][2]],
                    verts[HEXA_FACES[off][3]]
                };
                int fid = m.face_id(face);
                if(fid<0)
                {
                    fid = m.face_add(face);
                    winding[off] = true;
                }
                faces[off] = fid;
            }
            // add voxel
            uint pid = m.poly_add(faces,winding);
            m.poly_data(pid).label = g.voxels[id];
        }
    }
}

}

