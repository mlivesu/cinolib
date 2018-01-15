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
#include <cinolib/sphere_coverage.h>
#include <stdlib.h> // rand()
#include <math.h>   // sin(), cos(), sqrt(), pow()
#include <time.h>   // time()
#include <stddef.h> // NULL
#include <assert.h> // assert()

namespace cinolib
{

// Compute an approximate (though good) approximation of the even
// coverage of a sphere. Reference:
//
// http://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
//
CINO_INLINE
void sphere_coverage(const uint n_samples, std::vector<vec3d> & points)
{
    points.clear();

    srand(time(NULL));
    double rnd      = rand() * n_samples;
    double offset   = 2.0/double(n_samples);
    double increment = M_PI * (3.0 - sqrt(5.0));

    for(uint i=0; i<n_samples; ++i)
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
