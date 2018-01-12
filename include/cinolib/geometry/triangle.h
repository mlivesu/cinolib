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
#ifndef CINO_TRIANGLE_H
#define CINO_TRIANGLE_H

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

CINO_INLINE
vec3d triangle_normal(const vec3d A, const vec3d B, const vec3d C);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
double triangle_area(const vec A, const vec B, const vec C);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Given a triangle t(A,B,C) and a ray r(P,dir) compute both
// the edge and position where r exits from t
//
// NOTE: r is assumed to live "within" t, like in a gradient field for example...
//
CINO_INLINE
void triangle_traverse_with_ray(const vec3d   tri[3],
                                const vec3d   P,
                                const vec3d   dir,
                                      vec3d & exit_pos,
                                      uint  & exit_edge);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://en.wikipedia.org/wiki/Law_of_sines
//
CINO_INLINE
double triangle_law_of_sines(const double angle_0,
                             const double angle_1,
                             const double length_0); // returns length_1

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
bool triangle_barycentric_coords(const vec & A,
                                 const vec & B,
                                 const vec & C,
                                 const vec & P,
                                 std::vector<double> & wgts,
                                 const double   tol = 1e-5);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
bool triangle_point_is_inside(const vec    & A,
                              const vec    & B,
                              const vec    & C,
                              const vec    & P,
                              const double   tol = 0.0);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_bary_is_vertex(const std::vector<double> & bary,
                             uint                      & vid, // 0,1,2
                             const double                tol = 1e-5);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_bary_is_edge(const std::vector<double> & bary,
                           uint                      & eid, // 0,1,2 (see TRI_EDGES)
                           const double              tol = 1e-5);
}

#ifndef  CINO_STATIC_LIB
#include "triangle.cpp"
#endif

#endif // CINO_TRIANGLE_H
