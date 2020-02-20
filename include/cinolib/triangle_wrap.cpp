/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
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
#include <cinolib/triangle_wrap.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/vector_serialization.h>

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

#ifdef CINOLIB_USES_TRIANGLE

CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in,
                   const std::vector<uint>   & segs_in,
                   const std::vector<double> & holes_in,
                   const std::string         & flags,
                         std::vector<double> & coords_out,
                         std::vector<uint>   & tris_out)
{
    coords_out.clear();
    tris_out.clear();

    triangulateio in, out;

    assert(!coords_in.empty());

    in.numberofpoints = coords_in.size()/2;
    in.pointlist      = (double*)calloc(coords_in.size(),sizeof(double));
    for(int vid=0; vid<in.numberofpoints; ++vid)
    {
        in.pointlist[vid*2  ] = coords_in.at(vid*2  );
        in.pointlist[vid*2+1] = coords_in.at(vid*2+1);
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

    coords_out.reserve(out.numberofpoints*2);
    for(int vid=0; vid<out.numberofpoints; ++vid)
    {
        coords_out.push_back(out.pointlist[vid*2  ]);
        coords_out.push_back(out.pointlist[vid*2+1]);
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
}
#else
CINO_INLINE
void triangle_wrap(const std::vector<double> & /*coords_in*/,
                   const std::vector<uint>   & /*segs_in*/,
                   const std::vector<double> & /*holes_in*/,
                   const std::string         & /*flags*/,
                         std::vector<double> & /*coords_out*/,
                         std::vector<uint>   & /*tris_out*/)
{
    std::cerr << "ERROR : Triangle missing. Install Triangle and recompile defining symbol CINOLIB_USES_TRIANGLE" << std::endl;
    exit(-1);
}
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<vec2d> & verts_in,
                   const std::vector<uint>  & segs_in,
                   const std::vector<vec2d> & holes_in,
                   const std::string        & flags,
                         std::vector<vec2d> & verts_out,
                         std::vector<uint>  & tris_out)
{
    std::vector<double> tmp;
    triangle_wrap(serialized_xy_from_vec2d(verts_in), segs_in, serialized_xy_from_vec2d(holes_in), flags, tmp, tris_out);
    verts_out = vec2d_from_serialized_xy(tmp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in,  // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,    // serialized segments
                   const std::vector<double> & holes_in,   // serialized xy holes
                   const double                z_coord,    // lift triangulation to z_coord
                   const std::string         & flags,      // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         std::vector<double> & coords_out, // serialized output xy+z_coord coordinates
                         std::vector<uint>   & tris_out)   // serialized tris
{
    std::vector<double> tmp;
    triangle_wrap(coords_in, segs_in, holes_in, flags, tmp, tris_out);
    coords_out = serialized_xyz_from_serialized_xy(tmp, z_coord);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<vec2d>  & verts_in,  // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,    // serialized segments
                   const std::vector<vec2d>  & holes_in,   // serialized xy holes
                   const double                z_coord,    // lift triangulation to z_coord
                   const std::string         & flags,      // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         std::vector<vec3d>  & verts_out, // serialized output xy+z_coord coordinates
                         std::vector<uint>   & tris_out)   // serialized tris

{
    std::vector<double> tmp;
    triangle_wrap(serialized_xy_from_vec2d(verts_in), segs_in,
                  serialized_xy_from_vec2d(holes_in), flags, tmp, tris_out);
    verts_out = vec3d_from_serialized_xy(tmp, z_coord);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in, // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,   // serialized segments
                   const std::vector<double> & holes_in,  // serialized xy holes
                   const double                z_coord,   // lift triangulation to z_coord
                   const std::string         & flags,     // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         Trimesh<M,V,E,P>    & m)

{
    std::vector<double> v;
    std::vector<uint>   t;
    triangle_wrap(coords_in, segs_in, holes_in, flags, v, t);
    m = Trimesh<M,V,E,P>(vec3d_from_serialized_xy(v,z_coord), t);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void triangle_wrap(const std::vector<vec2d> & verts_in,
                   const std::vector<uint>  & segs_in,   // serialized segments
                   const std::vector<vec2d> & holes_in,
                   const double               z_coord,   // lift triangulation to z_coord
                   const std::string        & flags,     // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         Trimesh<M,V,E,P>   & m)
{
    std::vector<double> v;
    std::vector<uint>   t;
    triangle_wrap(serialized_xy_from_vec2d(verts_in), segs_in,
                  serialized_xy_from_vec2d(holes_in), flags, v, t);
    m = Trimesh<M,V,E,P>(vec3d_from_serialized_xy(v,z_coord), t);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void triangle_wrap(const std::vector<vec3d> & verts_in,
                   const std::vector<uint>  & segs_in,   // serialized segments
                   const std::vector<vec3d> & holes_in,
                   const double               z_coord,   // lift triangulation to z_coord
                   const std::string        & flags,     // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         Trimesh<M,V,E,P>   & m)
{
    std::vector<double> v;
    std::vector<uint>   t;
    triangle_wrap(serialized_xy_from_vec3d(verts_in), segs_in,
                  serialized_xy_from_vec3d(holes_in), flags, v, t);
    m = Trimesh<M,V,E,P>(vec3d_from_serialized_xy(v,z_coord), t);
}

}

#ifdef CINOLIB_USES_TRIANGLE
    #undef ANSI_DECLARATORS
    #undef REAL
    #undef VOID
#endif
