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
#include <cinolib/parallel_for.h>

namespace cinolib
{

// Voxelizes an object described by a surface mesh. Voxels will be deemed
// as being entirely inside, outside or traversed by the boundary of the
// input surface mesh, which can contain triangles, quads or general polygons.
//
template<class M, class V, class E, class P>
CINO_INLINE
void voxelize(const AbstractPolygonMesh<M,V,E,P> & m,
              const uint                           max_voxels_per_side,
                    VoxelGrid                    & g)
{
    // determine grid size across all dimensions
    g.bbox   = m.bbox();
    g.len    = g.bbox.delta().max_entry() / max_voxels_per_side;
    g.dim[0] = uint(ceil(g.bbox.delta_x()/g.len));
    g.dim[1] = uint(ceil(g.bbox.delta_y()/g.len));
    g.dim[2] = uint(ceil(g.bbox.delta_z()/g.len));

    // allocate the grid memory and flag voxels that have
    // non empty intersection with the input mesh elements
    uint size = g.dim[0]*g.dim[1]*g.dim[2];
    g.voxels = new int[size];
    std::fill_n(g.voxels, size, VOXEL_UNKNOWN); // initialize grid
    int flood_seed = -1;
    std::mutex mutex;
    PARALLEL_FOR(0, m.num_polys(), 1000, [&](uint pid)
    {
        AABB  box = m.poly_aabb(pid);
        vec3d beg = (box.min - g.bbox.min)/g.len;
        vec3d end = (box.max - g.bbox.min)/g.len;

        for(uint i=uint(floor(beg[0])); i<uint(ceil(end[0])); ++i)
        for(uint j=uint(floor(beg[1])); j<uint(ceil(end[1])); ++j)
        for(uint k=uint(floor(beg[2])); k<uint(ceil(end[2])); ++k)
        {
            uint index = serialize_3D_index(i,j,k,g.dim[1],g.dim[2]);
            if(g.voxels[index]==VOXEL_UNKNOWN)
            {
                vec3u ijk(i,j,k);
                AABB voxel = voxel_bbox(g,ijk.ptr());
                for(uint i=0; i<m.poly_tessellation(pid).size()/3; ++i)
                {
                    vec3d t[3] = { m.vert(m.poly_tessellation(pid).at(3*i+0)),
                                   m.vert(m.poly_tessellation(pid).at(3*i+1)),
                                   m.vert(m.poly_tessellation(pid).at(3*i+2)) };

                    if(voxel.intersects_triangle(t))
                    {
                        std::lock_guard<std::mutex> guard(mutex);
                        g.voxels[index] = VOXEL_BOUNDARY;
                        if(i==0 || j==0 || k==0 || i==g.dim[0]-1 || j==g.dim[1]-1 || k==g.dim[2]-1)
                        {
                            flood_seed = index;
                        }
                        break; // do not test other triangles for this boundary voxel...
                    }
                }
            }
        }
    });

    // flood the outside
    if(flood_seed>0)
    {
        g.voxels[flood_seed]=VOXEL_OUTSIDE;
        std::queue<uint> q;
        q.push(flood_seed);
        while(!q.empty())
        {
            uint index = q.front();
            q.pop();
            assert(g.voxels[index]==VOXEL_OUTSIDE);

            vec3u ijk = deserialize_3D_index(index,g.dim[1],g.dim[2]);
            std::vector<uint> n6 = voxel_n6(g,ijk.ptr()); // 6 neighborhood
            for(auto nbr : n6)
            {
                if(g.voxels[nbr]==VOXEL_UNKNOWN)
                {
                    g.voxels[nbr]=VOXEL_OUTSIDE;
                    q.push(nbr);
                }
            }
        }
    }

    // mark the rest as inside
    for(uint index=0; index<size; ++index)
    {
        if(g.voxels[index]==VOXEL_UNKNOWN)
        {
            g.voxels[index]=VOXEL_INSIDE;
        }
    }
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

