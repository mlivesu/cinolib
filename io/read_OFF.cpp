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
#include "read_OFF.h"

#include <iostream>

namespace cinolib
{

CINO_INLINE
void read_OFF(const char          * filename,
              std::vector<double> & xyz,
              std::vector<u_int>  & tri)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OFF() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    int nv, nt, dummy;

    fscanf(fp, "OFF\n");
    fscanf(fp, "%d %d %d\n", &nv, &nt, &dummy);

    for(int i=0; i<nv; ++i)
    {
        double x, y, z;
        fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(int i=0; i<nt; ++i)
    {
        int v0, v1, v2;
        fscanf(fp, "%*d %d %d %d\n", &v0, &v1, &v2);
        tri.push_back(v0);
        tri.push_back(v1);
        tri.push_back(v2);
    }

    fclose(fp);
}

}
