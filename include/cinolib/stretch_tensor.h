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
#ifndef CINO_STRETCH_TENSOR_H
#define CINO_STRETCH_TENSOR_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>
#include <cinolib/geometry/vec2.h>

namespace cinolib
{

// Starting from a reference triangle <p0,p1,p2> and a
// deformed version of it <q0,q1,q2>, this method first
// computes the associated deformation gradient F, and
// subsequently the stretch tensor U defined as the square
// root of the Right Cauchy-Green Deformation Tensor C. it
// returns the eigenvalues and eigenvectors of the stretch
// tensor U.
//
// These are all concepts and notations coming from continuum
// mechanics. You may want to refer to the following papers/books
// for some theoretical background:
//
//     Nonlinear Continuum Mechanics for Finite Element Analysis
//     Javier Bonet and Richard D.Wood
//     Cambridge University Press, 1997
//
//     https://en.wikiversity.org/wiki/Continuum_mechanics/Polar_decomposition
//
//     http://www.brown.edu/Departments/Engineering/Courses/En221/Notes/Kinematics/Kinematics.htm
//
// For some examples of computer graphics algorithm exploiting
// these concepts, you may also refer to:
//
// Real-Time Dynamic Wrinkling of Coarse Animated Cloth
// R.Gillette, C.Peters, N.Vining, E.Edwards, A.Sheffer
// Symposium on Computer Animation, 2015
//
CINO_INLINE
void stretch_tensor(const vec3d  & p0,    // reference triangle <p0,p1,p2>
                    const vec3d  & p1,    //
                    const vec3d  & p2,    //
                    const vec3d  & q0,    // deformed triangle <q0,q1,q2>
                    const vec3d  & q1,    //
                    const vec3d  & q2,    //
                          vec2d  & v_min, // eigenvector of the minimum eigenvalue
                          vec2d  & v_max, // eigenvector of the maximum eigenvalue
                          double & min,   // minimum eigenvalue
                          double & max);  // maximum eigenvalue
}

#ifndef  CINO_STATIC_LIB
#include "stretch_tensor.cpp"
#endif

#endif // CINO_STRETCH_TENSOR_H
