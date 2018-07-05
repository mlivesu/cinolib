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
#include <cinolib/tetgen_wrap.h>
#include <cinolib/vector_serialization.h>

#ifdef CINOLIB_USES_TETGEN
#include <tetgen.h>
#endif

namespace cinolib
{

CINO_INLINE
void tetgen_wrap(const std::vector<double>            & coords_in,
                 const std::vector<std::vector<uint>> & polys_in,
                 const std::vector<uint>              & edges_in,
                 const std::string                    & flags,       // options
                       std::vector<double>            & coords_out,
                       std::vector<uint>              & tets_out)
{
#ifdef CINOLIB_USES_TETGEN

    assert(!coords_in.empty());
    assert(!polys_in.empty());
    coords_out.clear();
    tets_out.clear();

    tetgenio           in, out;
    tetgenio::facet   *f;
    tetgenio::polygon *p;

    // vertices
    //
    in.firstnumber    = 0;
    in.numberofpoints = coords_in.size() / 3;
    in.pointlist      = new REAL[coords_in.size()];

    for(uint i=0; i<coords_in.size(); ++i)
    {
        in.pointlist[i] = static_cast<REAL>(coords_in[i]);
    }

    // faces
    //
    in.numberoffacets = polys_in.size();
    in.facetlist      = new tetgenio::facet[in.numberoffacets];

    for(int fid=0; fid<in.numberoffacets; ++fid)
    {
        f = &in.facetlist[fid];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
        f->numberofholes = 0;
        f->holelist = NULL;
        p = &f->polygonlist[0];
        p->numberofvertices = polys_in.at(fid).size();
        p->vertexlist = new int[p->numberofvertices];
        for(int i=0; i<p->numberofvertices; ++i)
        {
            p->vertexlist[i] = polys_in.at(fid).at(i);
        }
    }

    // edges
    //
    in.numberofedges = edges_in.size() / 2;
    in.edgelist       = new int[edges_in.size()];
    for(uint i=0; i<edges_in.size(); ++i)
    {
        in.edgelist[i] = edges_in[i];
    }

    // tetgen options
    //
    std::string s = "p" + flags;

    tetrahedralize(const_cast<char*>(s.c_str()), &in, &out);

    // generate tetmesh
    //
    for(int vid=0; vid<out.numberofpoints; ++vid)
    {
        int vid_ptr = 3 * vid;
        coords_out.push_back(static_cast<double>(out.pointlist[vid_ptr    ]));
        coords_out.push_back(static_cast<double>(out.pointlist[vid_ptr + 1]));
        coords_out.push_back(static_cast<double>(out.pointlist[vid_ptr + 2]));
    }

    for(int tid=0; tid<out.numberoftetrahedra; ++tid)
    {
        int tid_ptr = 4 * tid;
        tets_out.push_back(out.tetrahedronlist[tid_ptr    ]);
        tets_out.push_back(out.tetrahedronlist[tid_ptr + 1]);
        tets_out.push_back(out.tetrahedronlist[tid_ptr + 2]);
        tets_out.push_back(out.tetrahedronlist[tid_ptr + 3]);
    }

#else
    std::cerr << "ERROR : Tetgen missing. Install Tetgen and recompile defining symbol CINOLIB_USES_TETGEN" << std::endl;
    exit(-1);
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void tetgen_wrap(const std::vector<double> & coords_in,
                 const std::vector<uint>   & tris_in,
                 const std::vector<uint>   & edges_in,
                 const std::string         & flags,       // options
                       std::vector<double> & coords_out,
                       std::vector<uint>   & tets_out)
{
    tetgen_wrap(coords_in, polys_from_serialized_vids(tris_in,3), edges_in, flags, coords_out, tets_out);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void tetgen_wrap(const std::vector<vec3d>             & verts_in,
                 const std::vector<std::vector<uint>> & polys_in,
                 const std::vector<uint>              & edges_in,
                 const std::string                    & flags,       // options
                       std::vector<vec3d>             & verts_out,
                       std::vector<uint>              & tets_out)
{
    std::vector<double> coords_out;
    tetgen_wrap(serialized_xyz_from_vec3d(verts_in), polys_in, edges_in, flags, coords_out, tets_out);
    verts_out = vec3d_from_serialized_xyz(coords_out);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void tetgen_wrap(const std::vector<vec3d>  & verts_in,
                 const std::vector<uint>   & tris_in,
                 const std::vector<uint>   & edges_in,
                 const std::string         & flags,       // options
                       std::vector<vec3d>  & verts_out,
                       std::vector<uint>   & tets_out)
{
    std::vector<double> coords_out;
    tetgen_wrap(serialized_xyz_from_vec3d(verts_in), polys_from_serialized_vids(tris_in,3), edges_in, flags, coords_out, tets_out);
    verts_out = vec3d_from_serialized_xyz(coords_out);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void tetgen_wrap(const std::vector<vec3d>  & verts_in,
                 const std::vector<uint>   & tris_in,
                 const std::vector<uint>   & edges_in,
                 const std::string         & flags,       // options
                       Tetmesh<M,V,E,F,P>  & m)
{
    std::vector<vec3d> verts;
    std::vector<uint>  tets;
    tetgen_wrap(verts_in, tris_in, edges_in, flags, verts, tets);
    m = Tetmesh<M,V,E,F,P>(verts,tets);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void tetgen_wrap(const std::vector<vec3d>             & verts_in,
                 const std::vector<std::vector<uint>> & polys_in,
                 const std::vector<uint>              & edges_in,
                 const std::string                    & flags,       // options
                       Tetmesh<M,V,E,F,P>             & m)
{
    std::vector<vec3d> verts;
    std::vector<uint>  tets;
    tetgen_wrap(verts_in, polys_in, edges_in, flags, verts, tets);
    m = Tetmesh<M,V,E,F,P>(verts,tets);
}

}
