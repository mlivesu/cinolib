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
#include <cinolib/io/read_TAGLIASACCHI2012.h>


namespace cinolib
{

CINO_INLINE
void read_Tagliasacchi2012(const char          * filename,
                           std::vector<double> & coords,
                           std::vector<int>    & arcs,
                           std::vector<double> & radius)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    coords.clear();
    arcs.clear();
    radius.clear();

    FILE *f = fopen(filename,"r");

    if (!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_Livesu2012() : error while reading file " << filename << std::endl;
        exit(-1);
    }

    int dummy, nv, ne;
    fscanf(f,"# D:%d NV:%d NE:%d\n", &dummy, &nv, &ne);

    for (int i=0; i<nv; ++i)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        double x, y, z;
        fscanf(f, "v %lf %lf %lf\n", &x, &y, &z);
        coords.push_back(x);
        coords.push_back(y);
        coords.push_back(z);
        radius.push_back(0); // not supported
    }

    for (int i=0; i<ne; ++i)
    {
        int v0, v1;
        fscanf(f, "e %d %d\n", &v0, &v1);
        v0 -= 1;
        v1 -= 1;
        arcs.push_back(v0);
        arcs.push_back(v1);
    }

    fclose(f);

    std::cout << coords.size() / 3 << " skel points read" << std::endl;
    std::cout << arcs.size()   / 2 << " skel arcs   read" << std::endl;
}

}
