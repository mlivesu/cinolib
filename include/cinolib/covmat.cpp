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
