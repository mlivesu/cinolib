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
#ifndef CINO_VECTOR_SERIALIZATION
#define CINO_VECTOR_SERIALIZATION

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/vec2.h>

namespace cinolib
{

CINO_INLINE std::vector<vec3d> vec3d_from_serialized_xyz(const std::vector<double> & coords);
CINO_INLINE std::vector<vec3d> vec3d_from_serialized_xy (const std::vector<double> & coords, const double z);
CINO_INLINE std::vector<vec3d> vec3d_from_vec2d         (const std::vector<vec2d>  & verts,  const double z);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE std::vector<double> serialized_xyz_from_vec3d        (const std::vector<vec3d>  & verts);
CINO_INLINE std::vector<double> serialized_xyz_from_vec2d        (const std::vector<vec2d>  & verts,  const double z);
CINO_INLINE std::vector<double> serialized_xyz_from_serialized_xy(const std::vector<double> & coords, const double z);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE std::vector<vec2d> vec2d_from_serialized_xy (const std::vector<double> & coords);
CINO_INLINE std::vector<vec2d> vec2d_from_serialized_xyz(const std::vector<double> & coords);
CINO_INLINE std::vector<vec2d> vec2d_from_vec3d         (const std::vector<vec3d>  & verts);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE std::vector<double> serialized_xy_from_vec2d         (const std::vector<vec2d>  & verts);
CINO_INLINE std::vector<double> serialized_xy_from_vec3d         (const std::vector<vec3d>  & verts);
CINO_INLINE std::vector<double> serialized_xy_from_serialized_xyz(const std::vector<double> & coords);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE std::vector<std::vector<uint>> polys_from_serialized_vids(const std::vector<uint> & vids, const uint vids_per_poly);
CINO_INLINE std::vector<uint>              serialized_vids_from_polys(const std::vector<std::vector<uint>> & polys);

}

#ifndef  CINO_STATIC_LIB
#include "vector_serialization.cpp"
#endif

#endif // CINO_VECTOR_SERIALIZATION
