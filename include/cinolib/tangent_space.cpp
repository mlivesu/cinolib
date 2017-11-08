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
#include <cinolib/tangent_space.h>

namespace cinolib
{

CINO_INLINE
bool tangent_frame(const vec3d & p0, // 3D coordinates of triangle <p0,p1,p2>
                   const vec3d & p1,
                   const vec3d & p2,
                         vec3d & u,  // axes of a 2D orthonormal frame in the supporting
                         vec3d & v)  // plane of triangle <p0,p1,p2>
{
    bool degenerate = false;
    u  = p1-p0;
    if (u.normalize() < 1e-8) degenerate = true;
    v  = p2-p0;
    v -= u*u.dot(v);
    if (v.normalize() < 1e-8) degenerate = true;
    if (degenerate)
    {
        u = v = vec3d(0,0,0);
        std::cerr << "WARNING: degenerate triangle. Undefined tangent space!" << std::endl;
        return false;
    }
    return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool tangent_space_2d_coords(const vec3d & p0,  // 3D coordinates of triangle <p0,p1,p2>
                             const vec3d & p1,
                             const vec3d & p2,
                                   vec2d & P0,  // 2D coordinates of triangle <P0,P1,P2>
                                   vec2d & P1,  // in tangent space
                                   vec2d & P2)
{
    vec3d u,v;
    if (tangent_frame(p0, p1, p2, u, v))
    {
        P0 = vec2d(0,0);
        P1 = vec2d((p1-p0).dot(u), (p1-p0).dot(v));
        P2 = vec2d((p2-p0).dot(u), (p2-p0).dot(v));
        return true;
    }
    P0 = P1 = P2 = vec2d(0,0);
    return false;
}

}
