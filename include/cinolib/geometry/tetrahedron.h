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
#ifndef CINO_TETRAHEDRON_H
#define CINO_TETRAHEDRON_H

#include <cinolib/bbox.h>

namespace cinolib
{

class Tetrahedron
{
    public:

        Tetrahedron(const std::vector<vec3d> v) : v0(v.at(0)), v1(v.at(1)), v2(v.at(2)), v3(v.at(3)) {}

        Tetrahedron(const vec3d & v0,
                    const vec3d & v1,
                    const vec3d & v2,
                    const vec3d & v3) : v0(v0), v1(v1), v2(v2), v3(v3) {}

       ~Tetrahedron() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        Bbox aabb() const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        bool   contains        (const vec3d & p) const;
        double dist            (const vec3d & p) const;
        double dist_sqrd       (const vec3d & p) const;
        vec3d  point_closest_to(const vec3d & p) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        vec3d v0, v1, v2, v3;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool tet_barycentric_coords(const vec3d & A,
                            const vec3d & B,
                            const vec3d & C,
                            const vec3d & D,
                            const vec3d & P,
                            std::vector<double> & wgts,
                            const double  tol = 1e-5);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Given a point P and a tetrahedron ABCD, finds the point in ABCD that
// is closest to P. This code was taken directly from Ericson's seminal
// book "Real Time Collision Detection", Section 5.1.6
//
vec3d tetrahedron_closest_point(const vec3d & P,
                                const vec3d & A,
                                const vec3d & B,
                                const vec3d & C,
                                const vec3d & D);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double point_to_tetrahedron_dist(const vec3d & P,
                                 const vec3d & A,
                                 const vec3d & B,
                                 const vec3d & C,
                                 const vec3d & D);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double point_to_tetrahedron_dist_sqrd(const vec3d & P,
                                      const vec3d & A,
                                      const vec3d & B,
                                      const vec3d & C,
                                      const vec3d & D);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//void tet_closest_vertex(const vec3d  & A,
//                        const vec3d  & B,
//                        const vec3d  & C,
//                        const vec3d  & D,
//                        const vec3d  & query,
//                              uint   & id,
//                              double & dist);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//CINO_INLINE
//void tet_closest_edge(const vec3d  & A,
//                      const vec3d  & B,
//                      const vec3d  & C,
//                      const vec3d  & D,
//                      const vec3d  & query,
//                            uint   & id,
//                            double & dist);
}

#ifndef  CINO_STATIC_LIB
#include "tetrahedron.cpp"
#endif

#endif // CINO_TETRAHEDRON_H
