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
#include <cinolib/sphere_coverage.h>
#include <cinolib/pi.h>
#include <stdlib.h> // rand()
#include <math.h>   // sin(), cos(), sqrt(), pow()
#include <time.h>   // time()
#include <stddef.h> // NULL
#include <assert.h> // assert()

namespace cinolib
{

// Compute an approximate (though good) even coverage of the unit sphere. Reference:
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
