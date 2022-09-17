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
#include <cinolib/voxelize.h>
#include <cinolib/serialize_index.h>
#include <cinolib/octree.h>
#include <cinolib/clamp.h>

namespace cinolib
{

// Voxelizes an object described by a surface mesh. Voxels will be deemed
// as being entirely inside, outside or traversed by the boundary of the
// input surface mesh, which can contain triangles, quads or general polygons.
//
template<class M, class V, class E, class P>
CINO_INLINE
void voxelize(const AbstractPolygonMesh<M,V,E,P> & m, const uint max_voxels_per_side, VoxelGrid & g)
{
    // determine grid size across all dimensions
    g.bbox = m.bbox();
    g.len = g.bbox.delta().max_entry() / max_voxels_per_side;
    g.dim[0] = int(ceil(g.bbox.delta_x()/g.len));
    g.dim[1] = int(ceil(g.bbox.delta_y()/g.len));
    g.dim[2] = int(ceil(g.bbox.delta_z()/g.len));

    // allocate the grid memory and flag voxels that have
    // non empty intersection with the input mesh elements
    uint size = g.dim[0]*g.dim[1]*g.dim[2];
    g.voxels = new int[size];
    vec3d O = g.bbox.min;
    vec3d u(g.len,g.len,g.len);
    Octree o;
    o.build_from_mesh_polys(m);
    PARALLEL_FOR(0, size, 100000, [&](uint ijk)
    {
        vec3u tmp = deserialize_3D_index(ijk, g.dim[1], g.dim[2]);
        uint i = tmp[0];
        uint j = tmp[1];
        uint k = tmp[2];
        vec3d d(g.len*i,g.len*j,g.len*k);
        vec3d min = O+d;
        vec3d max = min+u;
        AABB voxel(min,max);
        bool boundary = false;
        std::unordered_set<uint> elems;
        o.intersects_box(voxel,elems);
        for(uint id : elems)
        {
            // MOVE THIS PART INSIDE THE OCTREE LOGIC...
            const Triangle *t = dynamic_cast<Triangle*>(o.items.at(id));
            if(voxel.intersects_triangle(t->v)) boundary = true;
        }
        g.voxels[ijk] = (boundary) ? VOXEL_BOUNDARY : VOXEL_UNKNOWN;
    });

//    // flood the outside
//    if(flood_seed>0)
//    {
//        g.voxels[flood_seed]=VOXEL_OUTSIDE;
//        std::queue<int> q;
//        q.push(flood_seed);
//        while(!q.empty())
//        {
//            int index = q.front();
//            q.pop();
//            assert(g.voxels[index]==VOXEL_OUTSIDE);

//            static std::vector<vec3u> neighborhood =
//            {
//                vec3u(-1,0,0),
//                vec3u(+1,0,0),
//                vec3u(0,-1,0),
//                vec3u(0,+1,0),
//                vec3u(0,0,-1),
//                vec3u(0,0,+1),
//            };
//            vec3u ijk = deserialize_3D_index(index,g.dim[1],g.dim[2]);
//            for(auto delta : neighborhood)
//            {
//                auto nbr = ijk + delta;
//                uint i = clamp(nbr[0],0,g.dim[0]-1);
//                uint j = clamp(nbr[1],0,g.dim[1]-1);
//                uint k = clamp(nbr[2],0,g.dim[2]-1);
//                uint ijk = serialize_3D_index(i,j,k,g.dim[1],g.dim[2]);
//                if(g.voxels[ijk]==VOXEL_UNMARKED)
//                {
//                    g.voxels[ijk]=VOXEL_OUTSIDE;
//                    q.push(ijk);
//                }
//            }
//        }
//    }

//    // flood the inside
//    for(int i=0; i<g.dim[0]; ++i)
//    for(int j=0; j<g.dim[1]; ++j)
//    for(int k=0; k<g.dim[2]; ++k)
//    {
//        uint ijk = serialize_3D_index(i,j,k,g.dim[0],g.dim[1]);
//        if(g.voxels[ijk]==VOXEL_UNMARKED)
//        {
//            g.voxels[ijk]=VOXEL_INSIDE;
//        }
//    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Voxelizes an object described by an analytic function f. Voxels will be
// deemed as being entirely on the positive halfspace, negative halfspace
// or traversed by the zero level set of the function f.
//
CINO_INLINE
void voxelize(const std::function<double(const vec3d & p)> & f,
              const AABB                                   & volume,
              const uint                                     max_voxels_per_side,
                    VoxelGrid                              & g)
{
    // determine grid size across all dimensions
    g.bbox = volume;
    g.len = g.bbox.delta().max_entry() / max_voxels_per_side;
    g.dim[0] = int(ceil(g.bbox.delta_x()/g.len));
    g.dim[1] = int(ceil(g.bbox.delta_y()/g.len));
    g.dim[2] = int(ceil(g.bbox.delta_z()/g.len));

    // allocate the grid memory and flag voxels depending
    // on how function f evaluates at the voxel corners
    uint size = g.dim[0]*g.dim[1]*g.dim[2];
    g.voxels = new int[size];
    vec3d O = g.bbox.min;
    vec3d u(g.len,g.len,g.len);
    PARALLEL_FOR(0, size, 100000, [&](uint index)
    {
        vec3u ijk = deserialize_3D_index(index,g.dim[1],g.dim[2]);
        bool negative = false;
        bool positive = false;
        bool zero     = false;
        for(uint off=0; off<8; ++off)
        {
            vec3d p = voxel_corner_xyz(g,ijk.ptr(),off);
            double fp = f(p);
            positive |= (fp>0);
            negative |= (fp<0);
            zero     |= (fp==0);
        }
        if( positive && !negative && !zero) g.voxels[index] = VOXEL_OUTSIDE; else
        if(!positive &&  negative && !zero) g.voxels[index] = VOXEL_INSIDE;  else
        g.voxels[index] = VOXEL_BOUNDARY;
    });
}

}

