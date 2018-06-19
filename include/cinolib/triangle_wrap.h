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
#ifndef CINO_TRIANGLE_WRAP_H
#define CINO_TRIANGLE_WRAP_H

#include <vector>
#include <sys/types.h>
#include <string>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/meshes/meshes.h>

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
void triangle_wrap(const std::vector<double> & coords_in,  // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,    // serialized segments
                   const std::vector<double> & holes_in,   // serialized xy holes
                   const double                z_coord,    // lift triangulation to z_coord
                   const std::string         & flags,      // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         std::vector<double> & coords_out, // serialized output xy+z_coord coordinates
                         std::vector<uint>   & tris_out);  // serialized tris

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<vec2d>             & verts_in,
                   const std::vector<uint>              & segs_in,
                   const std::vector<vec2d>             & holes_in,
                   const std::string                    & flags,
                         std::vector<vec2d>             & verts_out,
                         std::vector<std::vector<uint>> & tris_out);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void triangle_wrap(const std::vector<vec3d>             & verts_in,  // automatically ignores the third component (z)
                   const std::vector<uint>              & segs_in,
                   const std::vector<vec3d>             & holes_in,  // automatically ignores the third component (z)
                   const std::string                    & flags,
                         std::vector<vec3d>             & verts_out, // automatically sets the third component (z) to zero
                         std::vector<std::vector<uint>> & tris_out);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void triangle_wrap(const std::vector<double> & coords_in, // serialized input xy coordinates
                   const std::vector<uint>   & segs_in,   // serialized segments
                   const std::vector<double> & holes_in,  // serialized xy holes
                   const double                z_coord,   // lift triangulation to z_coord
                   const std::string         & flags,     // https://www.cs.cmu.edu/~quake/triangle.switch.html
                         Trimesh<M,V,E,P>    & m);
}

#ifndef  CINO_STATIC_LIB
#include "triangle_wrap.cpp"
#endif

#endif // CINO_TRIANGLE_WRAP_H
