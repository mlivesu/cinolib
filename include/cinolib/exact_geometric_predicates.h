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
 * 3D. All test are based on the popular Shewchuk's exact orient predicates.
 *
 * When present, the "strict" variable allows to restrict the search only
 * to the interior of the simplex, ignoring the sub-simplices that define
 * its boundary.
 *
 * Additionally to the boolean return type, each predicate returns a "where"
 * variable that indicates exactly where, in the input simplex, the point
 * is located (or the intersection occurred). For intersection queries, each
 * simplex involved in the test has its own "where" variable, which locates
 * the intersection inside it.
 *
 * Note that a point may belong to multiple sub-simplices. For example, a
 * point coincident to a triangle vertex belongs to a 0-dimensional simplex
 * (the vertex), at least two 2-dimensional simplices (its incident edges),
 * and one 2-dimensional simplex (the triangle).
 *
 * For conforming intersections (i.e. when the two input simplices form a
 * valid simplicial complex), the where variable points to the highest
 * dimensional (sub) simplex that fully contains the other simplex.
 *
 * For non conforming intersections, the where variable returns the lowest
 * dimensional simplex that must be split in order to achieve conformity
 * and realize a valid simplicial complex.
 *
 * WARNING: for degenerate elements such as zero length segments, zero area
 * triangles and zero volume tets, and for simplices that do not intersect
 * at a unique point, the lowest dimensional simplex to be split may not be
 * unique. In these cases, only one of them will be returned, and multiple
 * checks must be done to split the original simplices at all the necessary
 * points to realize a valid simplicial complex.
 */

/*
 * location of intersection points for exact predicates. Vert,
 * edge, and face orderings are compliant with the tables in:
 * #include <cinolib/standard_elements_tables.h>
 *
 * EQUALS_  prefix denotes (sub) simplices that intersect the
 *          other simplex in a conforming way (i.e. they make
 *          a valid simplicial complex)
 *
 * INSIDE_ suffix denotes (sub) simplices that are intersected
 *         by the other simplex at an inner point/area, therefore
 *         they must be split to achieve conformity and make a
 *         valid simplicial complex
 */
enum
{
    STRICTLY_OUTSIDE = 0,
    STRICTLY_INSIDE  = 1,
    ON_VERT0         = 2,
    ON_VERT1         = 3,
    ON_VERT2         = 4,
    ON_VERT3         = 5,
    ON_EDGE0         = 6,
    ON_EDGE1         = 7,
    ON_EDGE2         = 8,
    ON_EDGE3         = 9,
    ON_EDGE4         = 10,
    ON_EDGE5         = 11,
    ON_FACE0         = 12,
    ON_FACE1         = 12,
    ON_FACE2         = 12,
    ON_FACE3         = 12,
}
    OUTSIDE,       // no intersection
    EQUALS_VERT_0, // 1st vertex of an edge, triangle, tet
    EQUALS_VERT_1, // 2nd vertex of an edge, triangle, tet
    EQUALS_VERT_2, // 3rd vertex of a triangle, tet
    EQUALS_VERT_3, // 4th vertex of a tet
    INSIDE_SEG_0,  // input edge, or 1st edge of a triangle, tet
    EQUALS_SEG_0,  //
    INSIDE_SEG_1,  // 2nd edge of a triangle, tet
    EQUALS_SEG_1,  //
    INSIDE_SEG_2,  // 3rd edge of a triangle, tet
    EQUALS_SEG_2,  //
    INSIDE_SEG_3,  // 4th edge of a tet
    EQUALS_SEG_3,  //
    INSIDE_SEG_4,  // 5th edge of a tet
    EQUALS_SEG_4,  //
    INSIDE_SEG_5,  // 6th edge of a tet
    EQUALS_SEG_5,  //
    INSIDE_TRI_0,  // input triangle, or 1st face of a tet
    EQUALS_TRI_0,  //
    INSIDE_TRI_1,  // 2nd face of a tet,
    EQUALS_TRI_1,  //
    INSIDE_TRI_2,  // 3rd face of a tet,
    EQUALS_TRI_2,  //
    INSIDE_TRI_3,  // 4th face of a tet,
    EQUALS_TRI_3,  //
    INSIDE_TET_0,  // input tet
    EQUALS_TET_0,  //
};

/* forse meglio rinunciare al where per le intersezioni. è ben definito solo per i punti.... */

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
                                      const bool    strict,
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
