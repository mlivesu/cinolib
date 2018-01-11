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
#include <cinolib/triangle_wrap.h>
#include <cinolib/geometry/vec3.h>

#ifdef CINOLIB_USES_TRIANGLE
    /*
     * triangle.h is written in an old fashioned C style. As such, it requires
     * some annoying adjustment to comply with C++ standards
    */
    #ifndef ANSI_DECLARATORS
    #define ANSI_DECLARATORS
    #endif
    #ifndef REAL
    #define REAL double
    #endif
    #ifndef VOID
    #define VOID void
    #endif
    extern "C" // set C-like linking style (i.e. no name mungling)
    {
    #include <triangle.h>
    }
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<double> & verts_in,
                   const std::vector<uint>   & segs_in,
                   const std::vector<double> & holes_in,
                   const std::string         & flags,
                         std::vector<double> & verts_out,
                         std::vector<uint>   & tris_out)
{
    verts_out.clear();
    tris_out.clear();

#ifdef CINOLIB_USES_TRIANGLE

    triangulateio in, out;

    assert(!verts_in.empty());

    in.numberofpoints = verts_in.size()/2;
    in.pointlist      = (double*)calloc(verts_in.size(),sizeof(double));
    for(int vid=0; vid<in.numberofpoints; ++vid)
    {
        in.pointlist[vid*2  ] = verts_in.at(vid*2  );
        in.pointlist[vid*2+1] = verts_in.at(vid*2+1);
    }
    in.numberofpointattributes = 0;
    in.pointmarkerlist         = (int*)calloc(in.numberofpoints,sizeof(int));
    for(int vid=0; vid<in.numberofpoints; ++vid)
    {
       in.pointmarkerlist[vid] = 1;
    }

    in.numberoftriangles          = 0;
    in.numberofcorners            = 0;
    in.numberoftriangleattributes = 0;
    in.trianglelist               = NULL;
    in.triangleattributelist      = NULL;

    in.numberofsegments = segs_in.size()/2;
    in.segmentlist      = (int*)calloc(segs_in.size(),sizeof(int));
    for(uint i=0; i<segs_in.size(); ++i)
    {
        in.segmentlist[i] = static_cast<int>(segs_in[i]);
    }
    in.segmentmarkerlist = NULL;

    in.numberofholes = holes_in.size()/2;
    in.holelist      = (double*)calloc(holes_in.size(),sizeof(double));
    for(int hid=0; hid<in.numberofholes; ++hid)
    {
        in.holelist[hid*2  ] = holes_in.at(hid*2  );
        in.holelist[hid*2+1] = holes_in.at(hid*2+1);
    }
    in.numberofregions = 0;

    out.pointlist      = NULL;
    out.trianglelist   = NULL;
    out.segmentlist    = NULL;

    std::string s = flags + "pzB";

    triangulate(const_cast<char*>(s.c_str()), &in, &out, NULL);

    verts_out.reserve(out.numberofpoints*2);
    for(int vid=0; vid<out.numberofpoints; ++vid)
    {
        verts_out.push_back(out.pointlist[vid*2  ]);
        verts_out.push_back(out.pointlist[vid*2+1]);
    }

    tris_out.reserve(out.numberoftriangles * 3);
    for(int tid=0; tid<out.numberoftriangles; ++tid)
    {
        tris_out.push_back(out.trianglelist[tid*3  ]);
        tris_out.push_back(out.trianglelist[tid*3+1]);
        tris_out.push_back(out.trianglelist[tid*3+2]);
    }

    free(in.pointlist);
    free(in.pointmarkerlist);
    free(in.segmentlist);
    free(in.holelist);
    free(out.pointlist);
    free(out.trianglelist);
    free(out.segmentlist);

#else
    std::cerr << "ERROR : Triangle missing. Install Triangle and recompile defining symbol CINOLIB_USES_TRIANGLE" << std::endl;
    exit(-1);
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<vec2d>             & verts_in,
                   const std::vector<uint>              & segs_in,
                   const std::vector<vec2d>             & holes_in,
                   const std::string                    & flags,
                         std::vector<vec2d>             & verts_out,
                         std::vector<std::vector<uint>> & tris_out)
{
    std::vector<double> tmp_verts_in, tmp_holes_in, tmp_verts_out;
    std::vector<uint>   tmp_tris_out;

    tmp_verts_in.reserve(verts_in.size()*2);
    tmp_holes_in.reserve(holes_in.size()*2);
    tmp_verts_out.reserve(tmp_verts_in.size());

    for(const vec2d & p : verts_in)
    {
        tmp_verts_in.push_back(p.x());
        tmp_verts_in.push_back(p.y());
    }
    for(const vec2d & h : holes_in)
    {
        tmp_holes_in.push_back(h.x());
        tmp_holes_in.push_back(h.y());
    }

    triangle_wrap(tmp_verts_in, segs_in, tmp_holes_in, flags, tmp_verts_out, tmp_tris_out);

    verts_out.clear();
    uint nv = tmp_verts_out.size()/2;
    for(uint vid=0; vid<nv; ++vid)
    {
        verts_out.push_back(vec2d(tmp_verts_out.at(vid*2  ),
                                  tmp_verts_out.at(vid*2+1)));
    }

    uint nt = tmp_tris_out.size()/3;
    tris_out.clear();
    tris_out.reserve(nt);
    for(uint tid=0; tid<nt; ++tid)
    {
        std::vector<uint> t =
        {
            tmp_tris_out.at(3*tid+0),
            tmp_tris_out.at(3*tid+1),
            tmp_tris_out.at(3*tid+2),
        };
        tris_out.push_back(t);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<vec3d>             & verts_in,  // automatically ignores the third component (z)
                   const std::vector<uint>              & segs_in,
                   const std::vector<vec3d>             & holes_in,  // automatically ignores the third component (z)
                   const std::string                    & flags,
                         std::vector<vec3d>             & verts_out, // automatically sets the third component (z) to zero
                         std::vector<std::vector<uint>> & tris_out)
{
    std::vector<double> tmp_verts_in, tmp_holes_in, tmp_verts_out;
    std::vector<uint>   tmp_tris_out;

    tmp_verts_in.reserve(verts_in.size()*2);
    tmp_holes_in.reserve(holes_in.size()*2);
    tmp_verts_out.reserve(tmp_verts_in.size());

    for(const vec3d & p : verts_in)
    {
        tmp_verts_in.push_back(p.x());
        tmp_verts_in.push_back(p.y());
    }
    for(const vec3d & h : holes_in)
    {
        tmp_holes_in.push_back(h.x());
        tmp_holes_in.push_back(h.y());
    }

    triangle_wrap(tmp_verts_in, segs_in, tmp_holes_in, flags, tmp_verts_out, tmp_tris_out);

    verts_out.clear();
    uint nv = tmp_verts_out.size()/2;
    for(uint vid=0; vid<nv; ++vid)
    {
        verts_out.push_back(vec3d(tmp_verts_out.at(vid*2  ),
                                  tmp_verts_out.at(vid*2+1),
                                  0.0));
    }

    uint nt = tmp_tris_out.size()/3;
    tris_out.clear();
    tris_out.reserve(nt);
    for(uint tid=0; tid<nt; ++tid)
    {
        std::vector<uint> t =
        {
            tmp_tris_out.at(3*tid+0),
            tmp_tris_out.at(3*tid+1),
            tmp_tris_out.at(3*tid+2),
        };
        tris_out.push_back(t);
    }
}


}

#ifdef CINOLIB_USES_TRIANGLE
    #undef ANSI_DECLARATORS
    #undef REAL
    #undef VOID
#endif
