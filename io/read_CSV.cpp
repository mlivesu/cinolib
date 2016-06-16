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
#include "read_CSV.h"

namespace cinolib
{

CINO_INLINE
void read_CSV(const char          * filename,
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
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load() : error while reading file " << filename << std::endl;
        exit(-1);
    }

    char line[1024];

    while(fgets(line, 1024, f))
    {
        double x, y, z;
        sscanf( line, "%lf %lf %lf %*d", &x, &y, &z);
        coords.push_back(x);
        coords.push_back(y);
        coords.push_back(z);
    }

    fclose(f);

    for(int i=1; i<(int)coords.size()/3; ++i)
    {
        arcs.push_back(i-1);
        arcs.push_back(i);
    }

    std::cout << std::endl;
    std::cout << "Gesture Loaded - " << coords.size()/3 << " points" << std::endl;
    std::cout << std::endl;
}

}
