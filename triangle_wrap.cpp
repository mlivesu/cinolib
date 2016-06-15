/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#include "triangle_wrap.h"
#include "vec3.h"
#include "timer.h"

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

namespace cinolib
{

CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in,   // serialized xy coordinates
                   const std::vector<uint>   & segs_in,     // serialized segment endpoint ids
                   const std::vector<double> & holes_in,    // serialized xy holes innerpoints
                   const double                z_coord,    // z coord for coords_out
                   const std::string         & flags,       // options
                         std::vector<double> & coords_out,  // serialized xyz's (z = z_coord)
                         std::vector<uint>   & tris_out)    // serialized triangle vertex ids
{
    //timer_start("Triangulate 2D polygon");

    triangulateio in, out;

    assert(!coords_in.empty());

    in.numberofpoints = coords_in.size()/2;
    in.pointlist      = (double*)calloc(coords_in.size(),sizeof(double));
    for(int vid=0; vid<in.numberofpoints; ++vid)
    {
        in.pointlist[vid*2  ] = coords_in[vid*2  ];
        in.pointlist[vid*2+1] = coords_in[vid*2+1];
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
    for(size_t i=0; i<segs_in.size(); ++i)
    {
        in.segmentlist[i] = static_cast<int>(segs_in[i]);
    }
    in.segmentmarkerlist = NULL;

    in.numberofholes = holes_in.size()/2;
    in.holelist      = (double*)calloc(holes_in.size(),sizeof(double));
    for(int vid=0; vid<in.numberofholes; ++vid)
    {
        in.holelist[vid*2  ] = holes_in[vid*2  ];
        in.holelist[vid*2+1] = holes_in[vid*2+1];
    }
    in.numberofregions = 0;

    out.pointlist      = NULL;
    out.trianglelist   = NULL;
    out.segmentlist    = NULL;

    std::string s = flags + "pzB";

    triangulate(const_cast<char*>(s.c_str()), &in, &out, NULL);

    coords_out.reserve(out.numberofpoints * 3);
    for(int vid=0; vid<out.numberofpoints; ++vid)
    {
        coords_out.push_back(out.pointlist[vid*2  ]);
        coords_out.push_back(out.pointlist[vid*2+1]);
        coords_out.push_back(z_coord);
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

    //timer_stop("Triangulate 2D polygon");
}

}

#undef ANSI_DECLARATORS
#undef REAL
#undef VOID
