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
*                                                                               *
*                                                                               *
*     Claudio Mancinelli (claudio.f.mancinelli@gmail.com)                       *
*                                                                               *
*     University of Genoa                                                       *
*     Via Dodecaneso, 35                                                        *
*     16146 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef CINO_DISTORTION_ENERGIES_H
#define CINO_DISTORTION_ENERGIES_H

#include <cinolib/geometry/vec_mat.h>
#include <cinolib/geometry/vec_mat.h>

namespace cinolib
{

enum class DistEnergy
{
    // NOTATION: s_max, s_min are the maximum and minimum singular values of
    // a geometric transformation. For 2D maps (i.e. triangles) there exist
    // exactly two values. For 3D maps (i.e. tetrahedra) there exist three
    // values. When needed, the second singular value will be denoted as s_mid.
    //
    // For each metric we report various information, such as range, optimal
    // value, scale invariant or dependent, flip preventing (i.e. grows to inf
    // when e_min goes to zero), and a reference to the paper that introduced it.

    CONFORMAL,
    // Definition     : (s_max - s_min)^2
    // Range          : [0,inf)
    // Optimal Value  : 0
    // Scale Invariant: no
    // Flip Preventing: no
    // Reference      : Least squares conformal maps for automatic texture atlas generation (SIGGRAPH 2002)

    ISOMETRIC_MAX,
    // Definition     : max(s_max^2, 1/s_min^-2)
    // Range          : unbounded
    // Optimal Value  : 1
    // Scale Invariant: no
    // Flip Preventing: yes
    // References     : Bounded Distortion Parametrization in the Space of Metrics (SIGGRAPH 2016)

    ISOMETRIC_SUM,
    // Definition     : s_max^2 + 1/s_min^-2
    // Range          : [0,inf)
    // Optimal Value  : 1
    // Scale Invariant: no
    // Flip Preventing: yes
    // References     : Bounded Distortion Parametrization in the Space of Metrics (SIGGRAPH 2016)

    DIRICHLET,
    // 2D Definition  : s_max^2 + s_min^2
    // 3D Definition  : s_max^2 + s_mid^2 + s_min^2
    // Range          : [0,inf)
    // Optimal Value  : 2 in 2D, 3 in 3D
    // Scale Invariant: no
    // Flip Preventing: no
    // References     : Computing discrete minimal surfaces and their conjugates (1993)

    SYMMETRIC_DIRICHLET,
    // 2D Definition  : s_max^2 + s_min^2 + s_max^-2 + s_min^-2
    // 3D Definition  : s_max^2 + s_mid^2 + s_min^2 + s_max^-2 + s_mid^-2 + s_min^-2
    // Range          : [0,inf)
    // Optimal Value  : 4 in 2D, 6 in 3D
    // Scale Invariant: no
    // Flip Preventing: yes
    // References     : Bijective Parameterization with Free Boundaries (SIGGRAPH 2015)

    ARAP,
    // Definition     : (s_max-1)^2 + (s_min-1)^2
    // Range          : [0,inf)
    // Optimal Value  : 0
    // Scale Invariant: no
    // Flip Preventing: no
    // Notes          : also called Green-Lagrange
    // References     : A Local/Global Approach to Mesh Parameterization (SGP 2008)

    MIPS,
    // Definition     : s_max/s_min + s_min/s_max
    // Range          : unbounded
    // Optimal Value  : 2
    // Scale Invariant: yes
    // Flip Preventing: yes
    // Reference      : MIPS - An efficient global parametrization method (2000)

    STRETCH_L2,
    // Definition     : sqrt[(e_max*e_max + e_min*e_min)/2]
    // Range          : [0,inf)
    // Optimal Value  : 1
    // Scale Invariant: no
    // Flip Preventing: no
    // Reference      : Texture Mapping Progressive Meshes (2001)

    STRETCH_Linf,
    // Definition     : e_max
    // Range          : unbounded
    // Optimal Value  : 1
    // Scale Invariant: no
    // Flip Preventing: no
    // Reference      : Texture Mapping Progressive Meshes (2001)

    STRETCH_SORKINE,
    // Definition     : max(e_max, 1/e_min)
    // Range          : unbounded
    // Optimal Value  : 1
    // Scale Invariant: no
    // Flip Preventing: yes
    // References     : Bounded-Distortion Piecewise Mesh Parameterization (2002)
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double distortion(const double       s_max,
                  const double       s_min,
                  const DistEnergy & energy);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// mapping distortion between 2D frames (u0,v0) and (u1,v1)
CINO_INLINE
double distortion(const vec2d      & u0,
                  const vec2d      & v0,
                  const vec2d      & u1,
                  const vec2d      & v1,
                  const DistEnergy & energy);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// mapping distortion between 3D triangles (a0,a1,a2) and (b0,b1,b2)
CINO_INLINE
double distortion(const vec3d      & a0,
                  const vec3d      & a1,
                  const vec3d      & a2,
                  const vec3d      & b0,
                  const vec3d      & b1,
                  const vec3d      & b2,
                  const DistEnergy & energy);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

}

#ifndef  CINO_STATIC_LIB
#include "distortion_energies.cpp"
#endif

#endif // CINO_DISTORTION_ENERGIES_H
