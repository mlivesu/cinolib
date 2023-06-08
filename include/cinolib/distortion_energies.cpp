/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2023: Marco Livesu                                              *
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
*                                                                               *
*                                                                               *
*     Claudio Mancinelli (claudio.f.mancinelli@gmail.com)                       *
*                                                                               *
*     University of Genoa                                                       *
*     Via Dodecaneso, 35                                                        *
*     16146 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/distortion_energies.h>
#include <cinolib/jacobian_matrix.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double distortion(const double       s_max,
                  const double       s_min,
                  const DistEnergy & energy)
{
    switch(energy)
    {
        case DistEnergy::CONFORMAL           : return (s_max*s_max + s_min*s_min) / (2*s_max*s_min);
        case DistEnergy::ISOMETRIC_MAX       : return std::max(s_max*s_max, 1.0/(s_min*s_min));
        case DistEnergy::ISOMETRIC_SUM       : return s_max*s_max + 1/(s_min*s_min);
        case DistEnergy::DIRICHLET           : return s_max*s_max + s_min*s_min;
        case DistEnergy::SYMMETRIC_DIRICHLET : return s_max*s_max + s_min*s_min + 1/(s_max*s_max) + 1/(s_min*s_min);
        case DistEnergy::ARAP                : return pow(s_max-1, 2) + pow(s_min-1, 2);
        case DistEnergy::MIPS                : return s_max/s_min + s_min/s_max;
        case DistEnergy::STRETCH_L2          : return sqrt((s_max*s_max + s_min*s_min)/2);
        case DistEnergy::STRETCH_Linf        : return s_max;
        case DistEnergy::STRETCH_SORKINE     : return std::max(s_max, 1.0/s_min);
        default: throw("unkwnonw energy");
    }
    throw("I shouldn't be here");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double distortion(const double       s_max,
                  const double       s_mid,
                  const double       s_min,
                  const DistEnergy & energy)
{
    switch(energy)
    {
        case DistEnergy::CONFORMAL           : return (s_max*s_max + s_mid*s_mid + s_min*s_min) / (3*pow(s_max*s_mid*s_min, 2./3.));
        case DistEnergy::DIRICHLET           : return s_max*s_max + s_mid*s_mid + s_min*s_min;
        case DistEnergy::SYMMETRIC_DIRICHLET : return s_max*s_max + s_mid*s_mid + s_min*s_min + 1/(s_max*s_max) + 1/(s_mid*s_mid) + 1/(s_min*s_min);
        case DistEnergy::ARAP                : return pow(s_max-1, 2) + pow(s_mid-1, 2) + pow(s_min-1, 2);
        case DistEnergy::MIPS3D              : return 1./8. * (s_max/s_mid + s_mid/s_max) * (s_min/s_max + s_max/s_min) * (s_mid/s_min + s_min/s_mid);
        default: throw("unkwnonw energy");
    }
    throw("I shouldn't be here");
    return 0.0;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// mapping distortion between 2D frames (u0,v0) and (u1,v1)
CINO_INLINE
double distortion(const vec2d      & u0,
                  const vec2d      & v0,
                  const vec2d      & u1,
                  const vec2d      & v1,
                  const DistEnergy & energy)
{
    mat2d J;
    jacobian_matrix(u0,v0,u1,v1,J);

    vec2d S;
    mat2d U,V;
    J.SVD(U,S,V);

    return distortion(S[0],S[1],energy);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// mapping distortion between 3D triangles (a0,a1,a2) and (b0,b1,b2)
CINO_INLINE
double distortion(const vec3d      & a0,
                  const vec3d      & a1,
                  const vec3d      & a2,
                  const vec3d      & b0,
                  const vec3d      & b1,
                  const vec3d      & b2,
                  const DistEnergy & energy)
{
    mat2d J;
    jacobian_matrix(a0,a1,a2,b0,b1,b2,J);

    vec2d S;
    mat2d U,V;
    J.SVD(U,S,V);

    return distortion(S[1], S[0], energy);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// mapping distortion between 3D tetrahedra (a0,a1,a2,a3) and (b0,b1,b2,b3)
CINO_INLINE
double distortion(const vec3d      & a0,
                  const vec3d      & a1,
                  const vec3d      & a2,
                  const vec3d      & a3,
                  const vec3d      & b0,
                  const vec3d      & b1,
                  const vec3d      & b2,
                  const vec3d      & b3,
                  const DistEnergy & energy)
{
    mat3d J;
    jacobian_matrix(a0,a1,a2,a3,b0,b1,b2,b3,J);

    vec3d S;
    mat3d U,V;
    J.SVD(U,S,V);

    return distortion(S[0],S[1],S[2], energy);
}

}
