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
#include <cinolib/quality_tet.h>

/*
 * All metrics implemented here all based on:
 *
 * The Verdict Geometric Quality Library
 * SANDIA Report SAND2007-1751
 *
*/

namespace cinolib
{

CINO_INLINE
double tet_scaled_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3)
{
    static double sqrt_2 = 1.414213562373095;

    vec3d L0 = p1 - p0;
    vec3d L1 = p2 - p1;
    vec3d L2 = p0 - p2;
    vec3d L3 = p3 - p0;
    vec3d L4 = p3 - p1;
    vec3d L5 = p3 - p2;

    double L0_length = L0.length();
    double L1_length = L1.length();
    double L2_length = L2.length();
    double L3_length = L3.length();
    double L4_length = L4.length();
    double L5_length = L5.length();

    double J = (L2.cross(L0)).dot(L3);

    double lambda[] =
    {
        L0_length * L2_length * L3_length,
        L0_length * L1_length * L4_length,
        L1_length * L2_length * L5_length,
        L3_length * L4_length * L5_length,
        J
    };

    double max = *std::max_element(lambda, lambda + 5);

    if (max < -inf_double) return -1.0;
    return  (J * sqrt_2 / max);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double tet_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3)
{
    vec3d L0 = p1 - p0;
    vec3d L2 = p0 - p2;
    vec3d L3 = p3 - p0;

    return (L2.cross(L0)).dot(L3) / 6.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double tet_unsigned_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3)
{
    return fabs(tet_volume(p0, p1, p2, p3));
}

}
