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
#include <cinolib/line_triangle_intersection.h>
#include <cinolib/Moller_Trumbore_intersection.h>

namespace cinolib
{

CINO_INLINE
bool line_triangle_intersection(const vec3d  & line_point,
                                const vec3d  & line_dir,
                                const vec3d  & v0,
                                const vec3d  & v1,
                                const vec3d  & v2,
                                      vec3d  & res)
{
    double t;
    vec3d  bary;
    bool   coplanar, hits_back;
    if (Moller_Trumbore_intersection(line_point, line_dir, v0, v1, v2, hits_back, coplanar, t, bary))
    {
        res = line_point + line_dir * t;
        return true;
    }
    return false;
}

}
