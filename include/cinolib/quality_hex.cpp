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
#include <cinolib/quality_hex.h>
#include <cinolib/min_max_inf.h>

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
void norms(const vec3d vec[], const int size, double norms[])
{
    for(int i=0; i<size; ++i) norms[i] = vec[i].length();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void hex_edges(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
               const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7,
               vec3d L[], const bool normalized)
{
    L[0] = p1 - p0;    L[4] = p4 - p0;    L[8]  = p5 - p4;
    L[1] = p2 - p1;    L[5] = p5 - p1;    L[9]  = p6 - p5;
    L[2] = p3 - p2;    L[6] = p6 - p2;    L[10] = p7 - p6;
    L[3] = p3 - p0;    L[7] = p7 - p3;    L[11] = p7 - p4;

    if(normalized) for(int i=0; i<12; ++i) if(!L[i].is_null()) L[i].normalize();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void hex_principal_axes(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                        const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7,
                        vec3d X[], const bool normalized)
{
    X[0] = (p1 - p0) + (p2 - p3) + (p5 - p4) + (p6 - p7);
    X[1] = (p3 - p0) + (p2 - p1) + (p7 - p4) + (p6 - p5);
    X[2] = (p4 - p0) + (p5 - p1) + (p6 - p2) + (p7 - p3);

    if(normalized) for(int i=0; i<3; ++i) if(!X[i].is_null()) X[i].normalize();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void hex_cross_derivatives(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7,
                           vec3d XX[], const bool normalized)
{
    XX[0] = (p2 - p3) - (p1 - p0) + (p6 - p7) - (p5 - p4); // X_01 and X_10
    XX[1] = (p5 - p1) - (p4 - p0) + (p6 - p2) - (p7 - p3); // X_02 and X_20
    XX[2] = (p7 - p4) - (p3 - p0) + (p6 - p5) - (p2 - p1); // X_12 and X_21

    if(normalized) for(int i=0; i<3; ++i) if(!XX[i].is_null()) XX[i].normalize();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void hex_diagonals(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                   const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7,
                   vec3d D[], const bool normalized)
{
    D[0] = p6 - p0;
    D[1] = p7 - p1;
    D[2] = p4 - p2;
    D[3] = p5 - p3;

    if(normalized) for(int i=0; i<4; ++i) if(!D[i].is_null()) D[i].normalize();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void hex_subtets(const vec3d L[], const vec3d X[], const int id, vec3d tet[])
{
    switch(id)
    {
        case 0: tet[0]= L[0];  tet[1]= L[3];  tet[2]= L[4]; break;
        case 1: tet[0]= L[1];  tet[1]=-L[0];  tet[2]= L[5]; break;
        case 2: tet[0]= L[2];  tet[1]=-L[1];  tet[2]= L[6]; break;
        case 3: tet[0]=-L[3];  tet[1]=-L[2];  tet[2]= L[7]; break;
        case 4: tet[0]= L[11]; tet[1]= L[8];  tet[2]=-L[4]; break;
        case 5: tet[0]=-L[8];  tet[1]= L[9];  tet[2]=-L[5]; break;
        case 6: tet[0]=-L[9];  tet[1]= L[10]; tet[2]=-L[6]; break;
        case 7: tet[0]=-L[10]; tet[1]=-L[11]; tet[2]=-L[7]; break;
        case 8: tet[0]= X[0];  tet[1]= X[1];  tet[2]= X[2]; break;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double determinant(const vec3d & col0, const vec3d & col1, const vec3d & col2)
{
    return col0.dot(col1.cross(col2));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double frobenius(const vec3d & col0, const vec3d & col1, const vec3d & col2)
{
    double det = determinant(col0, col1, col2);
    if(det <= min_double) return max_double;

    double term1 = col0.dot(col0) + col1.dot(col1) + col2.dot(col2);
    double term2 = (col0.cross(col1)).dot(col0.cross(col1)) +
                   (col1.cross(col2)).dot(col1.cross(col2)) +
                   (col2.cross(col0)).dot(col2.cross(col0));
    double frob  = sqrt(term1*term2)/det;
    return frob/3.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_diagonal(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                    const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d  D[4];
    double D_norms[4];
    hex_diagonals(p0, p1, p2, p3, p4, p5, p6, p7, D, false);
    norms(D, 4, D_norms);
    return *std::min_element(D_norms, D_norms+4) / *std::max_element(D_norms, D_norms+4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_edge_ratio(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                      const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d  L[12];
    double L_norms[12];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, false);
    norms(L, 12, L_norms);
    return *std::max_element(L_norms, L_norms+12) / *std::min_element(L_norms, L_norms+12);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                    const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d L[12];
    vec3d X[3];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, false);
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);

    double sj[9];
    for(int i=0; i<9; ++i)
    {
        vec3d tet[3];
        hex_subtets(L, X, i, tet);
        sj[i] = determinant(tet[0], tet[1], tet[2]);
    }
    sj[8]/=64.0;
    return *std::min_element(sj,sj+9);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_max_edge_ratio(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                          const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d  X[3];
    double X_norms[3];
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);
    norms(X, 3, X_norms);

    if (X_norms[0] < min_double||
        X_norms[1] < min_double||
        X_norms[2] < min_double)
    {
        return max_double;
    }

    double max_ratios[3] =
    {
        std::max(X_norms[0]/X_norms[1] , X_norms[1]/X_norms[0]),
        std::max(X_norms[0]/X_norms[2] , X_norms[2]/X_norms[0]),
        std::max(X_norms[1]/X_norms[2] , X_norms[2]/X_norms[1]),
    };

    return *std::max_element(max_ratios, max_ratios+3);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_max_aspect_Frobenius(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                                const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d L[12];
    vec3d X[3];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, false);
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);

    double frob[8];
    for(int i=0; i<8; ++i)
    {
        vec3d tet[3];
        hex_subtets(L, X, i, tet);
        frob[i] = frobenius(tet[0], tet[1], tet[2]);
        if(frob[i]==max_double) return frob[i];
    }
    return *std::max_element(frob, frob+8);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_mean_aspect_Frobenius(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d L[12];
    vec3d X[3];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, false);
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);

    double frob = 0;
    for(int i=0; i<8; ++i)
    {
        vec3d tet[3];
        hex_subtets(L, X, i, tet);
        frob += frobenius(tet[0], tet[1], tet[2]);
    }
    return frob/8.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_oddy(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    static double four_over_three = 4.0/3.0;

    vec3d L[12];
    vec3d X[3];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, false);
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);

    float oddy[9];
    for(int i=0; i<9; ++i)
    {
        vec3d tet[3];
        hex_subtets(L, X, i, tet);
        double det = determinant(tet[0], tet[1], tet[2]);

        if(det > min_double)
        {
            double a11 = tet[0].dot(tet[0]);
            double a12 = tet[0].dot(tet[1]);
            double a13 = tet[0].dot(tet[2]);
            double a22 = tet[1].dot(tet[1]);
            double a23 = tet[1].dot(tet[2]);
            double a33 = tet[2].dot(tet[2]);

            double AtA_sqrd = a11*a11 + 2.0*a12*a12 + 2.0*a13*a13 + a22*a22 + 2.0*a23*a23 +a33*a33;
            double A_sqrd   = a11 + a22 + a33;

            oddy[i] = (AtA_sqrd - A_sqrd*A_sqrd/3.0) / pow(det,four_over_three);
        }
        else return max_double;
    }
    return *std::max_element(oddy,oddy+9);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_relative_size_squared(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7, const double & avgV)
{
    vec3d X[3];
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);
    double D = determinant(X[0], X[1], X[2]) / (64.0*avgV);

    if(avgV<=min_double || D<=min_double) return 0;
    return std::pow(std::min(D, 1.f/D), 2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_scaled_jacobian(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d  L[12];
    vec3d  X[3];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, true);
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, true);

    double sj[9];
    for(int i=0; i<9; ++i)
    {
        vec3d tet[3];
        hex_subtets(L, X, i, tet);
        sj[i] = determinant(tet[0], tet[1], tet[2]);
    }
    double msj = *std::min_element(sj, sj+9);
    if(msj > 1.0001) return -1.0;
    return msj;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_shape(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    static double two_over_three = 2.0/3.0;

    vec3d  L[12];
    vec3d  X[3];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, false);
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);

    double shape[9];
    for(int i=0; i<9; ++i)
    {
        vec3d tet[3];
        hex_subtets(L, X, i, tet);
        double det = determinant(tet[0], tet[1], tet[2]);
        if(det<=min_double) return 0;
        double num = pow(det, two_over_three);
        double den = tet[0].dot(tet[0]) + tet[1].dot(tet[1]) + tet[2].dot(tet[2]);
        if(den<=min_double) return 0;
        shape[i] = 3.0 * num/den;
    }
    return *std::min_element(shape, shape+9);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_shape_and_size(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                          const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7, const double & avg_V)
{
    return hex_shape(p0, p1, p2, p3, p4, p5, p6, p7) *
           hex_relative_size_squared(p0, p1, p2, p3, p4, p5, p6, p7, avg_V);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_shear(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d  L[12];
    vec3d  X[3];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, true);
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, true);

    double shear[9];
    for(int i=0; i<9; ++i)
    {
        vec3d tet[3];
        hex_subtets(L, X, i, tet);
        shear[i] = determinant(tet[0], tet[1], tet[2]);
        if(shear[i]<=min_double) return 0;
    }
    return *std::min_element(shear, shear+9);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_shear_and_size(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                          const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7, const double & avg_V)
{
    return hex_shear(p0, p1, p2, p3, p4, p5, p6, p7) *
           hex_relative_size_squared(p0, p1, p2, p3, p4, p5, p6, p7, avg_V);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_skew(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d X[3];
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, true);

    if(X[0].length() <= min_double) return 0;
    if(X[1].length() <= min_double) return 0;
    if(X[2].length() <= min_double) return 0;

    double skew[3] =
    {
        std::fabs(X[0].dot(X[1])),
        std::fabs(X[0].dot(X[2])),
        std::fabs(X[1].dot(X[2]))
    };

    return *std::max_element(skew,skew+3);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_stretch(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                   const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    static double sqrt3 = 1.732050807568877f;

    vec3d  L[12];
    double L_norms[12];
    hex_edges(p0, p1, p2, p3, p4, p5, p6, p7, L, false);
    norms(L, 12, L_norms);

    vec3d  D[4];
    double D_norms[4];
    hex_diagonals(p0, p1, p2, p3, p4, p5, p6, p7, D, false);
    norms(D, 4, D_norms);

    return sqrt3 * *std::min_element(L_norms, L_norms+12) /
                   *std::max_element(D_norms, D_norms+4);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_taper(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                 const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d  X[3];
    vec3d  XX[3];
    double X_norms[3];
    double XX_norms[3];
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);
    hex_cross_derivatives(p0, p1, p2, p3, p4, p5, p6, p7, XX, false);
    norms(X, 3, X_norms);
    norms(XX, 3, XX_norms);

    if(X_norms[0] <= min_double) return max_double;
    if(X_norms[1] <= min_double) return max_double;
    if(X_norms[2] <= min_double) return max_double;

    double taper[3] =
    {
        XX_norms[0] / std::min(X_norms[0], X_norms[1]),
        XX_norms[1] / std::min(X_norms[0], X_norms[2]),
        XX_norms[2] / std::min(X_norms[1], X_norms[2]),
    };

    return *std::max_element(taper, taper+3);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                  const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    vec3d X[3];
    hex_principal_axes(p0, p1, p2, p3, p4, p5, p6, p7, X, false);
    return determinant(X[0],X[1],X[2])/64.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double hex_unsigned_volume(const vec3d & p0, const vec3d & p1, const vec3d & p2, const vec3d & p3,
                           const vec3d & p4, const vec3d & p5, const vec3d & p6, const vec3d & p7)
{
    return fabs(hex_volume(p0, p1, p2, p3, p4, p5, p6, p7));
}

}
