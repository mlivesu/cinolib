/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2016                                                         *
* All rights reserved.                                                      *
*                                                                           *
* This file is part of CinoLib                                              *
*                                                                           *
* CinoLib is free software; you can redistribute it and/or modify           *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
****************************************************************************/
#ifndef CINO_SPHERE_COVERAGE_H
#define CINO_SPHERE_COVERAGE_H

#include <cinolib/cinolib.h>
#include <cinolib/geometry/vec3.h>

#include <vector>

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
void sphere_coverage(const int n_samples, std::vector<vec3d> & points);

}

#ifndef  CINO_STATIC_LIB
#include "sphere_coverage.cpp"
#endif

#endif // CINO_SPHERE_COVERAGE_H
