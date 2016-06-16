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
#include "read_DEYSUN2006.h"


namespace cinolib
{

CINO_INLINE
void read_DeyandSun2006(const char          * filename,
                        std::vector<double> & coords,
                        std::vector<int>    & arcs,
                        std::vector<double> & radius)
{
    coords.clear();
    arcs.clear();
    radius.clear();

    FILE *f = fopen(filename,"r");

    if (!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_Livesu2012() : error while reading file " << filename << std::endl;
        exit(-1);
    }

    int nv, ne;
    fscanf(f, "%d %d\n", &nv, &ne);

    for (int i=0; i<nv; ++i)
    {
        double x, y, z;
        fscanf(f, "%lf %lf %lf", &x, &y, &z);

        coords.push_back(x);
        coords.push_back(y);
        coords.push_back(z);
        radius.push_back(0); // not supported
    }

    for (int i=0; i<ne; ++i)
    {
        int v0, v1;

        fscanf(f, "%d %d", &v0, &v1);

        arcs.push_back(v0);
        arcs.push_back(v1);

        // some stuff we don't need at all
        //
        float dummyF;
        int dummyD1, dummyD2;
        fscanf(f, "%f", &dummyF);
        fscanf(f, "%f", &dummyF);
        fscanf(f, "%f", &dummyF);
        fscanf(f, "%d", &dummyD1);
        for(int j = 0; j < dummyD1; ++j) fscanf(f, "%d", &dummyD2);
    }

    fclose(f);

    std::cout << coords.size() / 3 << " skel points read" << std::endl;
    std::cout << arcs.size()   / 2 << " skel arcs   read" << std::endl;
}

}
