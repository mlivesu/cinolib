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
#include <cinolib/quality.h>
#include <cinolib/inf.h>
#include <algorithm>

/*
 * Scaled Jacobian and Volume computations are all based on:
 *
 * The Verdict Geometric Quality Library
 * SANDIA Report SAND2007-1751
 *
*/

namespace cinolib
{

static const double sqrt_2 = 1.414213562373095;

CINO_INLINE
double tet_scaled_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3)
{
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

CINO_INLINE
double tet_unsigned_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3)
{
    vec3d L0 = p1 - p0;
    vec3d L2 = p0 - p2;
    vec3d L3 = p3 - p0;

    return fabs((L2.cross(L0)).dot(L3) / 6.0);
}



CINO_INLINE
double hex_scaled_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    // edges
    vec3d L0  = p1 - p0;    vec3d L4  = p4 - p0;    vec3d L8  = p5 - p4;
    vec3d L1  = p2 - p1;    vec3d L5  = p5 - p1;    vec3d L9  = p6 - p5;
    vec3d L2  = p3 - p2;    vec3d L6  = p6 - p2;    vec3d L10 = p7 - p6;
    vec3d L3  = p3 - p0;    vec3d L7  = p7 - p3;    vec3d L11 = p7 - p4;

    // principal axes
    vec3d X1  = (p1 - p0) + (p2 - p3) + (p5 - p4) + (p6 - p7);
    vec3d X2  = (p3 - p0) + (p2 - p1) + (p7 - p4) + (p6 - p5);
    vec3d X3  = (p4 - p0) + (p5 - p1) + (p6 - p2) + (p7 - p3);

    L0.normalize();     L4.normalize();     L8.normalize();
    L1.normalize();     L5.normalize();     L9.normalize();
    L2.normalize();     L6.normalize();     L10.normalize();
    L3.normalize();     L7.normalize();     L11.normalize();
    X1.normalize();     X2.normalize();     X3.normalize();

    // normalized jacobian matrices determinants
    double alpha[9] =
    {
         L0.dot (  L3.cross ( L4) ),
         L1.dot ( -L0.cross ( L5) ),
         L2.dot ( -L1.cross ( L6) ),
        -L3.dot ( -L2.cross ( L7) ),
         L11.dot(  L8.cross (-L4) ),
        -L8.dot (  L9.cross (-L5) ),
        -L9.dot (  L10.cross(-L6) ),
        -L10.dot( -L11.cross(-L7) ),
         X1.dot (  X2.cross ( X3) )
    };

    double msj = *std::min_element(alpha, alpha+9);

    if (msj > 1.1) msj = -1.0;

    return msj;
}


CINO_INLINE
double hex_unsigned_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    // principal axes
    vec3d X1  = (p1 - p0) + (p2 - p3) + (p5 - p4) + (p6 - p7);
    vec3d X2  = (p3 - p0) + (p2 - p1) + (p7 - p4) + (p6 - p5);
    vec3d X3  = (p4 - p0) + (p5 - p1) + (p6 - p2) + (p7 - p3);

    return X1.dot (  X2.cross ( X3) ) / 64.0;
}

}
