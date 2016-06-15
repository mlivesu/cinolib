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
#include "read_OBJ.h"

#include <iostream>

namespace cinolib
{

CINO_INLINE
uint read_point_id(char *s)
{
    int pos = 0;
    while(s[pos] != '\0')
    {
        if(s[pos] == '/')
        {
            s[pos] = ' ';
        }
        ++pos;
    }
    sscanf(s, "%d", &pos);
    return pos-1;
}


CINO_INLINE
void read_OBJ(const char           * filename,
              std::vector<double>  & xyz,
              std::vector<u_int>   & tri)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OBJ() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    char line[1024],
          s0[1024],
          s1[1024],
          s2[1024];

    while(fgets(line, 1024, fp))
    {
        switch(line[0])
        {
            case 'v':

                if(!isspace(line[1])) continue;
                if(line[1] != 't' && line[1] != 'n' )
                {
                    double x, y, z;
                    sscanf( line, "%*c%lf%lf%lf", &x, &y, &z );
                    xyz.push_back(x);
                    xyz.push_back(y);
                    xyz.push_back(z);
                }
                break;

            case 'f':
                sscanf(line, "%*c%s%s%s", s0, s1, s2);
                tri.push_back(read_point_id(s0));
                tri.push_back(read_point_id(s1));
                tri.push_back(read_point_id(s2));
                break;
        }
    }
    fclose(fp);
}

}
