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
#ifndef CINO_TRIANGLE_H
#define CINO_TRIANGLE_H

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/bbox.h>

namespace cinolib
{

class Triangle
{
    public:

        Triangle(const std::vector<vec3d> v) : v0(v.at(0)), v1(v.at(1)), v2(v.at(2)) {}
        Triangle(const vec3d & v0, const vec3d & v1, const vec3d & v2) : v0(v0), v1(v1), v2(v2) {}
       ~Triangle() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Bbox aabb() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        double dist            (const vec3d & p) const;
        double dist_sqrd       (const vec3d & p) const;
        vec3d  point_closest_to(const vec3d & p) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d v0, v1, v2;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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
                                 const double   tol = 1e-10);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template <class vec>
CINO_INLINE
bool triangle_point_is_inside(const vec    & A,
                              const vec    & B,
                              const vec    & C,
                              const vec    & P,
                              const double   tol = 1e-10);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_bary_is_vertex(const std::vector<double> & bary,
                             uint                      & vid, // 0,1,2
                             const double                tol = 1e-10);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool triangle_bary_is_edge(const std::vector<double> & bary,
                           uint                      & eid, // 0,1,2 (see TRI_EDGES)
                           const double              tol = 1e-10);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d triangle_closest_point(const vec3d & P,
                             const vec3d & A,
                             const vec3d & B,
                             const vec3d & C);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double point_to_triangle_dist(const vec3d & P,
                              const vec3d & A,
                              const vec3d & B,
                              const vec3d & C);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double point_to_triangle_dist_sqrd(const vec3d & P,
                                   const vec3d & A,
                                   const vec3d & B,
                                   const vec3d & C);
}

#ifndef  CINO_STATIC_LIB
#include "triangle.cpp"
#endif

#endif // CINO_TRIANGLE_H
