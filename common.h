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
#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <cmath>
#include <vector>
#include <eigen3/Eigen/Dense>

#include "cino_inline.h"
#include "vec3.h"

namespace cinolib
{

enum
{
    A = 0,
    B = 1,
    C = 2,
    D = 3
};

enum
{
    x = 0,
    y = 1,
    z = 2
};


typedef std::pair<int,int> ipair;

CINO_INLINE ipair unique_pair(int v0, int v1)
{
    ipair e;
    e.first  = std::min(v0,v1);
    e.second = std::max(v0,v1);
    return e;
}

CINO_INLINE std::ostream & operator<<(std::ostream & in, const ipair & p)
{
    in << "(" << p.first << "," << p.second << ") ";
    return in;
}

// http://stackoverflow.com/questions/3738384/stable-cotangent
//
CINO_INLINE double cot(double x) { return tan(M_PI_2 - x); }

template<typename C>
CINO_INLINE void CHECK_BOUNDS(const C & container, int index)
{
    assert(index < (int)container.size());
}

template<typename C,typename E>
CINO_INLINE bool DOES_NOT_CONTAIN(const C & container, const E & element)
{
    return (container.find(element) == container.end());
}

template<typename C,typename E>
CINO_INLINE bool CONTAINS(const C & container, const E & element)
{
    return (container.find(element) != container.end());
}

CINO_INLINE void from_std_3x3_to_Eigen_3x3(const double stdM[3][3], Eigen::Matrix3d & eigenM)
{
    eigenM.coeffRef(0,0) = stdM[0][0];  eigenM.coeffRef(0,1) = stdM[0][1];  eigenM.coeffRef(0,2) = stdM[0][2];
    eigenM.coeffRef(1,0) = stdM[1][0];  eigenM.coeffRef(1,1) = stdM[1][1];  eigenM.coeffRef(1,2) = stdM[1][2];
    eigenM.coeffRef(2,0) = stdM[2][0];  eigenM.coeffRef(2,1) = stdM[2][1];  eigenM.coeffRef(2,2) = stdM[2][2];
}

CINO_INLINE void from_eigen_3x3_to_std_3x3(const Eigen::Matrix3d & eigenM, double stdM[3][3])
{
    stdM[0][0] = eigenM.coeffRef(0,0);  stdM[0][1] = eigenM.coeffRef(0,1);  stdM[0][2] = eigenM.coeffRef(0,2);
    stdM[1][0] = eigenM.coeffRef(1,0);  stdM[1][1] = eigenM.coeffRef(1,1);  stdM[1][2] = eigenM.coeffRef(1,2);
    stdM[2][0] = eigenM.coeffRef(2,0);  stdM[2][1] = eigenM.coeffRef(2,1);  stdM[2][2] = eigenM.coeffRef(2,2);
}

CINO_INLINE int serialize_2D_index(const int r, const int c, const int n_cols)
{
    return r * n_cols + c;
}

CINO_INLINE ipair deserialize_2D_index(const int i, const int n_cols)
{
    return std::make_pair(i/n_cols, i%n_cols);
}

}


#endif // COMMON_H
