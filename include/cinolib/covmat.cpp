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
#include <cinolib/covmat.h>
#include <cinolib/symbols.h>

namespace cinolib
{

CINO_INLINE
void covariance_matrix(const std::vector<vec3d> & points, double cov[3][3], const bool centered)
{
    double norm_fact = static_cast<double>(points.size());

    // mean
    vec3d avg(0,0,0);
    if(!centered)
    {
        for(auto p : points) avg += p;
        avg /= norm_fact;
    }

    cov[X][X]=0; cov[X][Y]=0; cov[X][Z]=0;
                 cov[Y][Y]=0; cov[Y][Z]=0;
                              cov[Z][Z]=0;
    for(auto p : points)
    {
        vec3d pc = p - avg;
        cov[X][X] += pc.x() * pc.x();
        cov[X][Y] += pc.x() * pc.y();
        cov[X][Z] += pc.x() * pc.z();
        cov[Y][Y] += pc.y() * pc.y();
        cov[Y][Z] += pc.y() * pc.z();
        cov[Z][Z] += pc.z() * pc.z();
    }
    cov[X][X]/=norm_fact; cov[X][Y]/=norm_fact; cov[X][Z]/=norm_fact;
                          cov[Y][Y]/=norm_fact; cov[Y][Z]/=norm_fact;
                                                cov[Z][Z]/=norm_fact;
    cov[Y][X]=cov[X][Y];
    cov[Z][X]=cov[X][Z]; cov[Z][Y]=cov[Y][Z];
}

}
