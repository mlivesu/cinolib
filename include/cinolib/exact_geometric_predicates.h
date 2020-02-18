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
#ifndef CINO_EXACT_GEOMETRIC_PREDICATES
#define CINO_EXACT_GEOMETRIC_PREDICATES

#include <cinolib/geometry/vec2.h>
#include <cinolib/geometry/vec3.h>

namespace cinolib
{

/* Exact predicates to test point in segment/triangle/tetrahedron, and
 * also to test intersections between segments/triangles/tets in 2D and
 * 3D. All test based on the popular Shewchuk's exact orient predicates.
 *
 * When present, the "strict" variable allows to restrict the search only
 * to the interior of the simplex, ignoring the sub-simplices that define
 * its boundary.
 *
 * Additionally to the boolean return type, each predicate returns a "where"
 * variable which indicates exactly where, in the input simplex, the intersection
 * occurred. Note that a point may belong to multiple subsimplices. For
 * example, a point coincident to a triangle vertex belongs to a 0-dimensional
 * simplex (the vertex), at least two 2-dimensional simplices (its incident edges),
 * and one 2-dimensional simplex (the triangle). The where indicator will always
 * point to the smallest dimensional simplex that contains it.
 *
 * WARNING: for degenerate elements such as zero length segments, zero area
 * triangles and zero volume tets, the smallest dimesional simplex that contains
 * a given point may not be unique!
 */

// location of intersection points for exact predicates.
// Vert, edge, and face orderings are compliant with the
// tables in cinolib/standard_elements_tables.h
//
enum
{
    STRICTLY_OUTSIDE = 0,  // used for segs, tris, tets
    STRICTLY_INSIDE  = 1,  // used for segs, tris, tets
    ON_VERT_0        = 2,  // used for segs, tris, tets
    ON_VERT_1        = 3,  // used for segs, tris, tets
    ON_VERT_2        = 4,  // used for tris, tets
    ON_VERT_3        = 5,  // used for tets
    ON_EDGE_0        = 6,  // used for tris, tets
    ON_EDGE_1        = 7,  // used for tris, tets
    ON_EDGE_2        = 8,  // used for tris, tets
    ON_EDGE_3        = 9,  // used for tets
    ON_EDGE_4        = 10, // used for tets
    ON_EDGE_5        = 11, // used for tets
    ON_FACE_0        = 12, // used for tets
    ON_FACE_1        = 13, // used for tets
    ON_FACE_2        = 14, // used for tets
    ON_FACE_3        = 15, // used for tets
    COINCIDENT       = 16, // used for segs, tris, tets
    OVERLAP          = 16, // used for segs (when they are colinear and intersect)
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool points_are_colinear_exact(const vec2d & p0,
                               const vec2d & p1,
                               const vec2d & p2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool points_are_colinear_exact(const vec3d & p0,
                               const vec3d & p1,
                               const vec3d & p2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool points_are_coplanar_exact(const vec3d & p0,
                               const vec3d & p1,
                               const vec3d & p2,
                               const vec3d & p3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_segment_exact(const vec2d & p,
                            const vec2d   s[],
                            const bool    strict,
                                  int   & where);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_segment_exact(const vec3d & p,
                            const vec3d   s[],
                            const bool    strict,
                                  int   & where);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_triangle_exact(const vec2d & p,
                             const vec2d   t[],
                             const bool    strict,
                                   int   & where);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_triangle_exact(const vec3d & p,
                             const vec3d   t[],
                                   int   & where);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool point_in_tet_exact(const vec3d & p,
                        const vec3d   t[],
                              int   & where);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool segment_segment_intersect_exact(const vec2d   s0[],
                                     const vec2d   s1[],
                                           int   & where_on_s0,
                                           int   & where_on_s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool segment_segment_intersect_exact(const vec3d   s0[],
                                     const vec3d   s1[],
                                     const bool    strict,
                                           int   & where_on_s0,
                                           int   & where_on_s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool segment_triangle_intersect_exact(const vec2d   s[],
                                      const vec2d   t[],
                                            int   & where_on_s,
                                            int   & where_on_t);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool segment_triangle_intersect_exact(const vec3d   s[],
                                      const vec3d   t[],
                                            int   & where_on_s,
                                            int   & where_on_t);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_triangle_intersect_exact(const vec2d   t0[],
                                       const vec2d   t1[],
                                             int   & where_on_t0,
                                             int   & where_on_t1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_triangle_intersect_exact(const vec3d   t0[],
                                       const vec3d   t1[],
                                             int   & where_on_t0,
                                             int   & where_on_t1);
}

#ifndef  CINO_STATIC_LIB
#include "exact_geometric_predicates.cpp"
#endif

#endif // CINO_INTERSECTION_EXACT_PREDICATES
