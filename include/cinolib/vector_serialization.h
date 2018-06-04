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
#ifndef CINO_VECTOR_SERIALIZATION
#define CINO_VECTOR_SERIALIZATION

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

CINO_INLINE
std::vector<vec3d> vec3d_from_serialized_xyz(const std::vector<double> & coords);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> vec3d_from_serialized_xy(const std::vector<double> & coords, const double z);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> serialized_xyz_from_vec3d(const std::vector<vec3d> & verts);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<std::vector<uint>> polys_from_serialized_vids(const std::vector<uint> & vids, const uint vids_per_poly);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> serialized_vids_from_polys(const std::vector<std::vector<uint>> & polys);

}

#ifndef  CINO_STATIC_LIB
#include "vector_serialization.cpp"
#endif

#endif // CINO_VECTOR_SERIALIZATION
