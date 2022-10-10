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
#ifndef CINO_PREDICATES
#define CINO_PREDICATES

#include <cinolib/geometry/vec_mat.h>
#include <bitset>

namespace cinolib
{
/* This file provides orient, incircle and in sphere predicates,
 * as well as additional predicates that build on top of them
 * to test point in segments/triangles/tetrahedra, and also to
 * test intersections between these entities in 2D and 3D. In the
 * default configuration, all these predicates are INEXACT, and the
 * basic orient, incircle and in sphere are basically equivalent to
 * the "fast" version of the Shewchuk predicates.
 *
 * *********************************************************************
 * IMPORTANT: to switch to exact predicates, you must define the symbol
 * CINOLIB_USES_SHEWCHUK_PREDICATES at compilation time.
 * *********************************************************************
 *
 * Return values for the point_in_{segment | triangle | tet} predicates:
 * an integer flag which indicates exactly where, in the input simplex, the
 * point is located is returned. Note that a point tipically belongs to
 * multiple sub-simplices. For example, a point coincident to a triangle
 * vertex belongs to a 0-dimensional simplex (the vertex), at least two
 * 2-dimensional simplices (its incident edges), and one 2-dimensional
 * simplex (the triangle). The integer flag points to the lowest dimensional
 * (sub) simplex that fully contains the point.
 *
 * WARNING: for degenerate elements such as zero length segments, zero area
 * triangles and zero volume tets, the lowest dimensional simplex containing
 * a point may not be unique. In these cases, only one of them will be returned.
 *
 * Return values for intersection tests: an integer flag which indicates
 * whether the input simplices are fully disjoint or intersect is returned.
 * Intersection can be of three types: (i) the simplices intersect only
 * at a shared sub-simplex, defining a valid simplicial complex (e.g. two
 * edges sharing a commong endpoint); (ii) the simplices intersect in a way
 * that does not define a valid simplicial complex; (iii) the simplices
 * intersect in some pathological way (e.g. two colinear triangles that
 * partially overlap, or two coplanar triangles that overlap).
 *
 * WARNING: intersections tests all assume that simplices are non degenerate.
 * If the code is compiled without defining the NDEBUG symbol, dedicated assertions
 * will make the program stop in case of zero length edges, zero area triangles,
 * or zero volume tets.
 */

// location of intersection points for point_in_{segment | triangle | tet}
// predicates. Elements' orders are compliant with the tables in:
//
//   #include <cinolib/standard_elements_tables.h>
typedef enum
{
    STRICTLY_OUTSIDE = 0,  // strictly outside the input simplex
    STRICTLY_INSIDE  = 1,  // strictly inside  the input simplex
    ON_VERT0         = 2,  // used for segs, tris and tets
    ON_VERT1         = 3,  // used for segs, tris and tets
    ON_VERT2         = 4,  // used for tris and tets
    ON_VERT3         = 5,  // used for tets
    ON_EDGE0         = 6,  // used for tris and tets
    ON_EDGE1         = 7,  // used for tris and tets
    ON_EDGE2         = 8,  // used for tris and tets
    ON_EDGE3         = 9,  // used for tets
    ON_EDGE4         = 10, // used for tets
    ON_EDGE5         = 11, // used for tets
    ON_FACE0         = 12, // used for tets
    ON_FACE1         = 13, // used for tets
    ON_FACE2         = 14, // used for tets
    ON_FACE3         = 15, // used for tets
}
PointInSimplex;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// intersection types
typedef enum
{
    DO_NOT_INTERSECT   = 0, // simplices do not intersect
    SIMPLICIAL_COMPLEX = 1, // simplices form a valid simplicial complex (i.e. they are coincident or share a sub-simplex)
    INTERSECT          = 2, // simplices intersect in a non conforming way
    OVERLAP            = 3, // for corner cases: simplices intersect and partially overlap
}                           // (e.g. colinear segments or coplanar triangles)
SimplexIntersection;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifdef CINOLIB_USES_SHEWCHUK_PREDICATES

/* Wrap of the popular geometric predicates described by Shewchuk in:
 *
 * Routines for Arbitrary Precision Floating-point Arithmetic and
 * Fast Robust Geometric Predicates
 *
 * WARNING: if you use these predicates, you should include in your
 * project <CINOLIB_HOME>/external/predicates/shewchuk.c and compile it,
 * otherwise the linker will not find an implementation for the methods
 * below
 */
extern "C"
{

double orient2d(const double * pa,
                const double * pb,
                const double * pc);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double orient3d(const double * pa,
                const double * pb,
                const double * pc,
                const double * pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double incircle(const double * pa,
                const double * pb,
                const double * pc,
                const double * pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double insphere(const double * pa,
                const double * pb,
                const double * pc,
                const double * pd,
                const double * pe);
}

#else

// These are equivalent to the "fast" version of Shewchuk's predicates. Hence are INEXACT
// geometric predicates solely based on the accuracy of the floating point system

CINO_INLINE
double orient2d(const double * pa,
                const double * pb,
                const double * pc);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double orient3d(const double * pa,
                const double * pb,
                const double * pc,
                const double * pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double incircle(const double * pa,
                const double * pb,
                const double * pc,
                const double * pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double insphere(const double * pa,
                const double * pb,
                const double * pc,
                const double * pd,
                const double * pe);
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// wrap of orient2d for cinolib points. Either exact or not depending on CINOLIB_USES_SHEWCHUK_PREDICATES
CINO_INLINE
double orient2d(const vec2d & pa,
                const vec2d & pb,
                const vec2d & pc);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// wrap of orient3d for cinolib points. Either exact or not depending on CINOLIB_USES_SHEWCHUK_PREDICATES
CINO_INLINE
double orient3d(const vec3d & pa,
                const vec3d & pb,
                const vec3d & pc,
                const vec3d & pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// wrap of incircle for cinolib points. Either exact or not depending on CINOLIB_USES_SHEWCHUK_PREDICATES
CINO_INLINE
double incircle(const vec2d & pa,
                const vec2d & pb,
                const vec2d & pc,
                const vec2d & pd);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// wrap of insphere for cinolib points. Either exact or not depending on CINOLIB_USES_SHEWCHUK_PREDICATES
CINO_INLINE
double insphere(const vec3d & pa,
                const vec3d & pb,
                const vec3d & pc,
                const vec3d & pd,
                const vec3d & pe);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the area of the triangle p0-p1-p2 is zero
CINO_INLINE
bool points_are_colinear_2d(const vec2d & p0,
                            const vec2d & p1,
                            const vec2d & p2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the area of the triangle p0-p1-p2 is zero
CINO_INLINE
bool points_are_colinear_2d(const double * p0,
                            const double * p1,
                            const double * p2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the area of all the orthogonal 2d projections of the triangle p0-p1-p2 is zero
CINO_INLINE
bool points_are_colinear_3d(const vec3d & p0,
                           const vec3d & p1,
                           const vec3d & p2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the area of all the orthogonal 2d projections of the triangle p0-p1-p2 is zero
CINO_INLINE
bool points_are_colinear_3d(const double * p0,
                            const double * p1,
                            const double * p2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the volume of the tetrahedron p0-p1-p2-p3 is zero
CINO_INLINE
bool points_are_coplanar_3d(const vec3d & p0,
                            const vec3d & p1,
                            const vec3d & p2,
                            const vec3d & p3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the volume of the tetrahedron p0-p1-p2-p3 is zero
CINO_INLINE
bool points_are_coplanar_3d(const double * p0,
                            const double * p1,
                            const double * p2,
                            const double * p3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of s
// STRICTLY_INSIDE  if p lies inside segment s (endpoints excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_segment_2d(const vec2d & p,
                                   const vec2d & s0,
                                   const vec2d & s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of s
// STRICTLY_INSIDE  if p lies inside segment s (endpoints excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_segment_2d(const double * p,
                                   const double * s0,
                                   const double * s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of s
// STRICTLY_INSIDE  if p lies inside segment s (endpoints excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_segment_3d(const vec3d & p,
                                   const vec3d & s0,
                                   const vec3d & s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of s
// STRICTLY_INSIDE  if p lies inside segment s (endpoints excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_segment_3d(const double * p,
                                   const double * s0,
                                   const double * s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// STRICTLY_INSIDE  if p lies inside triangle t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_triangle_2d(const vec2d & p,
                                    const vec2d & t0,
                                    const vec2d & t1,
                                    const vec2d & t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// STRICTLY_INSIDE  if p lies inside triangle t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_triangle_2d(const double * p,
                                    const double * t0,
                                    const double * t1,
                                    const double * t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// STRICTLY_INSIDE  if p lies inside triangle t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_triangle_3d(const vec3d & p,
                                    const vec3d & t0,
                                    const vec3d & t1,
                                    const vec3d & t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// STRICTLY_INSIDE  if p lies inside triangle t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_triangle_3d(const double * p,
                                    const double * t0,
                                    const double * t1,
                                    const double * t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// ON_FACEk         if p lies inside the k-th face of t (borders excluded)
// STRICTLY_INSIDE  if p lies inside tetrahedron t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_tet(const vec3d & p,
                            const vec3d & t0,
                            const vec3d & t1,
                            const vec3d & t2,
                            const vec3d & t3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// ON_VERTi         if p coincides with the i-th vertex of t
// ON_EDGEj         if p lies inside the j-th edge of t (endpoints excluded)
// ON_FACEk         if p lies inside the k-th face of t (borders excluded)
// STRICTLY_INSIDE  if p lies inside tetrahedron t (borders excluded)
// STRICTLY_OUTSIDE otherwise
CINO_INLINE
PointInSimplex point_in_tet(const double * p,
                            const double * t0,
                            const double * t1,
                            const double * t2,
                            const double * t3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if segments are fully disjoint
// SIMPLICIAL_COMPLEX   if segments coincide or intersect at a shared vertex
// INTERSECT            if segments intersect at an inner point (for s0, s1, or both)
// OVERLAP              if segments are colinear and partially overlapped
CINO_INLINE
SimplexIntersection segment_segment_intersect_2d(const vec2d & s00,
                                                 const vec2d & s01,
                                                 const vec2d & s10,
                                                 const vec2d & s11);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if segments are fully disjoint
// SIMPLICIAL_COMPLEX   if segments coincide or intersect at a shared vertex
// INTERSECT            if segments intersect at an inner point (for s0, s1, or both)
// OVERLAP              if segments are colinear and partially overlapped
CINO_INLINE
SimplexIntersection segment_segment_intersect_2d(const double * s00,
                                                 const double * s01,
                                                 const double * s10,
                                                 const double * s11);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if segments are fully disjoint
// SIMPLICIAL_COMPLEX   if segments coincide or intersect at a shared vertex
// INTERSECT            if segments intersect at an inner point (for s0, s1, or both)
// OVERLAP              if segments are colinear and partially overlapped
CINO_INLINE
SimplexIntersection segment_segment_intersect_3d(const vec3d & s00,
                                                 const vec3d & s01,
                                                 const vec3d & s10,
                                                 const vec3d & s11);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if segments are fully disjoint
// SIMPLICIAL_COMPLEX   if segments coincide or intersect at a shared vertex
// INTERSECT            if segments intersect at an inner point (for s0, s1, or both)
// OVERLAP              if segments are colinear and partially overlapped
CINO_INLINE
SimplexIntersection segment_segment_intersect_3d(const double * s00,
                                                 const double * s01,
                                                 const double * s10,
                                                 const double * s11);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if s and t are fully disjoint
// SIMPLICIAL_COMPLEX   if s is an edge of t, or s is degenerate and coincides with a vertex of t
// INTERSECT            if s and t intersect and do not forma a valid simplex
CINO_INLINE
SimplexIntersection segment_triangle_intersect_2d(const vec2d & s0,
                                                  const vec2d & s1,
                                                  const vec2d & t0,
                                                  const vec2d & t1,
                                                  const vec2d & t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if s and t are fully disjoint
// SIMPLICIAL_COMPLEX   if s is an edge of t, or s is degenerate and coincides with a vertex of t
// INTERSECT            if s and t intersect and do not forma a valid simplex
CINO_INLINE
SimplexIntersection segment_triangle_intersect_2d(const double * s0,
                                                  const double * s1,
                                                  const double * t0,
                                                  const double * t1,
                                                  const double * t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if s and t are fully disjoint
// SIMPLICIAL_COMPLEX   if s is an edge of t, or s is degenerate and coincides with a vertex of t
// INTERSECT            if s and t intersect and do not forma a valid simplex
CINO_INLINE
SimplexIntersection segment_triangle_intersect_3d(const vec3d & s0,
                                                  const vec3d & s1,
                                                  const vec3d & t0,
                                                  const vec3d & t1,
                                                  const vec3d & t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if s and t are fully disjoint
// SIMPLICIAL_COMPLEX   if s is an edge of t, or s is degenerate and coincides with a vertex of t
// INTERSECT            if s and t intersect and do not forma a valid simplex
CINO_INLINE
SimplexIntersection segment_triangle_intersect_3d(const double * s0,
                                                  const double * s1,
                                                  const double * t0,
                                                  const double * t1,
                                                  const double * t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if s and t are fully disjoint
// SIMPLICIAL_COMPLEX   if s is an edge of t, or s is degenerate and coincides with a vertex of t
// INTERSECT            if s and t intersect and do not forma a valid simplex
CINO_INLINE
SimplexIntersection segment_tet_intersect_3d(const vec3d & s0,
                                             const vec3d & s1,
                                             const vec3d & t0,
                                             const vec3d & t1,
                                             const vec3d & t2,
                                             const vec3d & t3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if s and t are fully disjoint
// SIMPLICIAL_COMPLEX   if s is an edge of t, or s is degenerate and coincides with a vertex of t
// INTERSECT            if s and t intersect and do not forma a valid simplex
CINO_INLINE
SimplexIntersection segment_tet_intersect_3d(const double * s0,
                                             const double * s1,
                                             const double * t0,
                                             const double * t1,
                                             const double * t2,
                                             const double * t3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if triangles are fully disjoint
// SIMPLICIAL_COMPLEX   if triangles coincide or intersect at a shared sub-simplex
// INTERSECT            if triangles intersect without making a valid simplcial complex
CINO_INLINE
SimplexIntersection triangle_triangle_intersect_2d(const vec2d & t00,
                                                   const vec2d & t01,
                                                   const vec2d & t02,
                                                   const vec2d & t10,
                                                   const vec2d & t11,
                                                   const vec2d & t12);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if triangles are fully disjoint
// SIMPLICIAL_COMPLEX   if triangles coincide or intersect at a shared sub-simplex
// INTERSECT            if triangles intersect without making a valid simplcial complex
CINO_INLINE
SimplexIntersection triangle_triangle_intersect_2d(const double * t00,
                                                   const double * t01,
                                                   const double * t02,
                                                   const double * t10,
                                                   const double * t11,
                                                   const double * t12);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if triangles are fully disjoint
// SIMPLICIAL_COMPLEX   if triangles coincide or intersect at a shared sub-simplex
// INTERSECT            if triangles intersect without making a valid simplcial complex
CINO_INLINE
SimplexIntersection triangle_triangle_intersect_3d(const vec3d & t00,
                                                   const vec3d & t01,
                                                   const vec3d & t02,
                                                   const vec3d & t10,
                                                   const vec3d & t11,
                                                   const vec3d & t12);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns:
// DO_NOT_INTERSECT     if triangles are fully disjoint
// SIMPLICIAL_COMPLEX   if triangles coincide or intersect at a shared sub-simplex
// INTERSECT            if triangles intersect without making a valid simplcial complex
CINO_INLINE
SimplexIntersection triangle_triangle_intersect_3d(const double * t00,
                                                   const double * t01,
                                                   const double * t02,
                                                   const double * t10,
                                                   const double * t11,
                                                   const double * t12);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if s0==s1
CINO_INLINE
bool segment_is_degenerate_2d(const vec2d & s0,
                              const vec2d & s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if s0==s1
CINO_INLINE
bool segment_is_degenerate_2d(const double * s0,
                              const double * s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if s0==s1
CINO_INLINE
bool segment_is_degenerate_3d(const vec3d & s0,
                              const vec3d & s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if s0==s1
CINO_INLINE
bool segment_is_degenerate_3d(const double * s0,
                              const double * s1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t0, t1 and t2 are colinear
CINO_INLINE
bool triangle_is_degenerate_2d(const vec2d & t0,
                               const vec2d & t1,
                               const vec2d & t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t0, t1 and t2 are colinear
CINO_INLINE
bool triangle_is_degenerate_2d(const double * t0,
                               const double * t1,
                               const double * t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t0, t1 and t2 are colinear
CINO_INLINE
bool triangle_is_degenerate_3d(const vec3d & t0,
                               const vec3d & t1,
                               const vec3d & t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t0, t1 and t2 are colinear
CINO_INLINE
bool triangle_is_degenerate_3d(const double * t0,
                               const double * t1,
                               const double * t2);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t0, t1, t2 and t3 are coplanar
CINO_INLINE
bool tet_is_degenerate(const vec3d & t0,
                       const vec3d & t1,
                       const vec3d & t2,
                       const vec3d & t3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if t0, t1, t2 and t3 are coplanar
CINO_INLINE
bool tet_is_degenerate(const double * t0,
                       const double * t1,
                       const double * t2,
                       const double * t3);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if v0 and v1 are equal
CINO_INLINE
bool vec_equals_2d(const double * v0,
                   const double * v1);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// returns true if v0 and v1 are equal
CINO_INLINE
bool vec_equals_3d(const double * v0,
                   const double * v1);
}

#ifndef  CINO_STATIC_LIB
#include "predicates.cpp"
#endif

#endif // CINO_PREDICATES
