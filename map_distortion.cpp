/****************************************************************************
* Italian National Research Council                                         *
* Institute for Applied Mathematics and Information Technologies, Genoa     *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Author: Marco Livesu (marco.livesu@gmail.com)                             *
*                                                                           *
* Copyright(C) 2017                                                         *
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
#include <cinolib/map_distortion.h>
#include <eigen3/Eigen/Dense>

namespace cinolib
{

CINO_INLINE
double aspect_ratio_distortion(const double m[3][3])
{
    using namespace Eigen;

    Matrix3d M;
    from_std_3x3_to_Eigen_3x3(m,M);

    JacobiSVD<Matrix3d> svd(M);
    DiagonalMatrix<double,3> s = DiagonalMatrix<double,3>(svd.singularValues());

    assert(s.diagonal()[0] >= s.diagonal()[1]);
    assert(s.diagonal()[1] >= s.diagonal()[2]);

    return s.diagonal()[0] / std::fabs(s.diagonal()[2]);
}

*
