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
#include "read_TET.h"
#include <iostream>

namespace cinolib
{

CINO_INLINE
void read_TET(const char          * filename,
              std::vector<double> & xyz,
              std::vector<u_int>  & tets)
{
    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_TET() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    int  nv, nt;
    char line[1024];

    fgets(line,1024,fp);
    sscanf(line, "%d vertices", &nv);

    fgets(line,1024,fp);
    sscanf(line, "%d tets", &nt);

    for(int vid=0; vid<nv; ++vid)
    {
        fgets(line, 1024, fp);

        double x,y,z;
        sscanf(line, "%lf %lf %lf ", &x, &y, &z);

        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(int tid=0; tid<nt; ++tid)
    {
        fgets(line, 1024, fp);

        int v0, v1, v2, v3;
        sscanf(line, "4 %d %d %d %d", &v0, &v1, &v2, &v3);

        tets.push_back(v0);
        tets.push_back(v3);
        tets.push_back(v2);
        tets.push_back(v1);
    }

    fclose(fp);
}

}
