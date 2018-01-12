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
#ifndef CINO_SPHERE_COVERAGE_H
#define CINO_SPHERE_COVERAGE_H

#include <vector>
#include <sys/types.h>
#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec3.h>


namespace cinolib
{
// Compute an approximate (though good) approximation of the even
// coverage of a sphere. It is based on the spherical Fibonacci lattice
//
// References:
//
//    http://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
//
//    Measurement of Areas on a Sphere Using Fibonacci and Latitude–Longitude Lattices
//    Mathematical Geosciences 42(1) - 2010
//    Springer
//
CINO_INLINE
void sphere_coverage(const uint n_samples, std::vector<vec3d> & points);

}

#ifndef  CINO_STATIC_LIB
#include "sphere_coverage.cpp"
#endif

#endif // CINO_SPHERE_COVERAGE_H
