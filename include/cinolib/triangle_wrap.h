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
#ifndef CINO_TRIANGLE_WRAP_H
#define CINO_TRIANGLE_WRAP_H

#include <vector>
#include <sys/types.h>
#include <string>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/trimesh.h>

namespace cinolib
{

CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in,  // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,    // serialized segments
                   const std::vector<double> & holes_in,   // serialized xy holes
                   const std::string         & flags,      // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         std::vector<double> & coords_out, // serialized output xy coordinates
                         std::vector<uint>   & tris_out);  // serialized tris

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<vec2d> & verts_in,
                   const std::vector<uint>  & segs_in,
                   const std::vector<vec2d> & holes_in,
                   const std::string        & flags,
                         std::vector<vec2d> & verts_out,
                         std::vector<uint>  & tris_out);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in,  // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,    // serialized segments
                   const std::vector<double> & holes_in,   // serialized xy holes
                   const double                z_coord,    // lift triangulation to z_coord
                   const std::string         & flags,      // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         std::vector<double> & coords_out, // serialized output xy+z_coord coordinates
                         std::vector<uint>   & tris_out);  // serialized tris

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<vec2d>  & verts_in,  // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,    // serialized segments
                   const std::vector<vec2d>  & holes_in,   // serialized xy holes
                   const double                z_coord,    // lift triangulation to z_coord
                   const std::string         & flags,      // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         std::vector<vec2d>  & verts_out, // serialized output xy+z_coord coordinates
                         std::vector<uint>   & tris_out);  // serialized tris

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in, // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,   // serialized segments
                   const std::vector<double> & holes_in,  // serialized xy holes
                   const double                z_coord,   // lift triangulation to z_coord
                   const std::string         & flags,     // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         Trimesh<M,V,E,P>    & m);


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void triangle_wrap(const std::vector<vec2d> & verts_in,
                   const std::vector<uint>  & segs_in,   // serialized segments
                   const std::vector<vec2d> & holes_in,
                   const double               z_coord,   // lift triangulation to z_coord
                   const std::string        & flags,     // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         Trimesh<M,V,E,P>   & m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void triangle_wrap(const std::vector<vec3d> & verts_in,
                   const std::vector<uint>  & segs_in,   // serialized segments
                   const std::vector<vec3d> & holes_in,
                   const double               z_coord,   // lift triangulation to z_coord
                   const std::string        & flags,     // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         Trimesh<M,V,E,P>   & m);
}

#ifndef  CINO_STATIC_LIB
#include "triangle_wrap.cpp"
#endif

#endif // CINO_TRIANGLE_WRAP_H
