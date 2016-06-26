
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
#include "sphere_coverage.h"

namespace cinolib
{

// Compute an approximate (though good) approximation of the even
// coverage of a sphere. Reference:
//
// http://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
//
CINO_INLINE
void sphere_coverage(const int n_samples, std::vector<vec3d> & points)
{
    assert(points.empty());

    srand(time(NULL));
    double rnd      = rand() * n_samples;
    double offset   = 2.0/double(n_samples);
    double increment = M_PI * (3.0 - sqrt(5.0));

    for(int i=0; i<n_samples; ++i)
    {
        double y   = ((i * offset) - 1) + (offset / 2);
        double r   = sqrt(1 - pow(y,2));
        double phi = ((i + int(rnd)) % n_samples) * increment;
        double x   = cos(phi) * r;
        double z   = sin(phi) * r;

        points.push_back(vec3d(x,y,z));
    }
}

}
