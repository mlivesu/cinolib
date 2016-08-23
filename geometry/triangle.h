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
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>

#include <set>

namespace cinolib
{

static const int TRI_EDGES[3][2] =
{
    { 0, 1 }, // 0
    { 1, 2 }, // 1
    { 2, 0 }, // 2
};


CINO_INLINE
vec3d triangle_normal(const vec3d A,
                      const vec3d B,
                      const vec3d C);



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
                                      int   & exit_edge);


// https://en.wikipedia.org/wiki/Law_of_sines
//
CINO_INLINE
double triangle_law_of_sines(const double angle_0,
                             const double angle_1,
                             const double length_0); // returns length_1



CINO_INLINE
bool triangle_barycentric_coords(const vec3d  & A,
                                 const vec3d  & B,
                                 const vec3d  & C,
                                 const vec3d  & P,
                                 std::vector<double> & wgts,
                                 const double   tol = 1e-5);

CINO_INLINE
bool triangle_bary_is_vertex(const std::vector<double> & bary,
                             int                       & vid, // 0,1,2
                             const double                tol = 1e-5);

CINO_INLINE
bool triangle_bary_is_edge(const std::vector<double> & bary,
                             int                     & eid, // 0,1,2 (see TRI_EDGES)
                             const double              tol = 1e-5);
}

#ifndef  CINO_STATIC_LIB
#include "triangle.cpp"
#endif

#endif // TRIANGLE_H
