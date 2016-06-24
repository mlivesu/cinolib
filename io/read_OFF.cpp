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
              std::vector<u_int>  & tri,
              std::vector<u_int>  & quad)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OFF() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    int nv, npoly, dummy;

    fscanf(fp, "OFF\n");
    fscanf(fp, "%d %d %d\n", &nv, &npoly, &dummy);

    for(int i=0; i<nv; ++i)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        double x, y, z;
        fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(int i=0; i<npoly; ++i)
    {
        int n_corners, v0, v1, v2, v3;
        fscanf(fp, "%d %d %d %d %d\n", &n_corners, &v0, &v1, &v2, &v3);
        if (n_corners == 3)
        {
            tri.push_back(v0);
            tri.push_back(v1);
            tri.push_back(v2);
        }
        else if (n_corners == 4)
        {
            quad.push_back(v0);
            quad.push_back(v1);
            quad.push_back(v2);
            quad.push_back(v3);
        }
        else
        {
            std::cerr << "read_OFF: polygons with " << n_corners << " are not supported!" << std::endl;
            assert("Unsupported polygon" && false);
        }
    }

    fclose(fp);
}

}
