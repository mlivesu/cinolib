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
#include "read_MESH.h"

#include <iostream>

namespace cinolib
{

CINO_INLINE
void read_MESH(const char          * filename,
               std::vector<double> & xyz,
               std::vector<u_int>  & tets,
               std::vector<u_int>  & hexa)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_MESH() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    char line[1024];

    fgets(line,1024,fp);
    fgets(line,1024,fp);

    while(fgets(line, 1024, fp))
    {
        // read vertices
        //
        if (line[0] == 'V')
        {
            int    nverts;
            double x,y,z;
            fgets(line,1024,fp);
            sscanf(line, "%d", &nverts);
            for(int i=0; i<nverts; ++i)
            {
                // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
                //
                fgets(line, 1024, fp);
                sscanf(line, "%lf %lf %lf", &x, &y, &z);
                xyz.push_back(x);
                xyz.push_back(y);
                xyz.push_back(z);
            }
        }

        // read tetrahedra
        //
        if (line[0] == 'T' && line[1] == 'e')
        {
            int ntets, v0, v1, v2, v3;
            fgets(line, 1024, fp);
            sscanf(line, "%d", &ntets);
            for( int i=0; i<ntets; i++ )
            {
                fgets(line,1024,fp);
                sscanf(line, "%d %d %d %d", &v0, &v1, &v2, &v3);
                tets.push_back(v0 - 1);
                tets.push_back(v1 - 1);
                tets.push_back(v2 - 1);
                tets.push_back(v3 - 1);
            }
        }

        // read hexahedra
        //
        if (line[0] == 'H' && line[1] == 'e')
        {
            int nhexa, v0, v1, v2, v3, v4, v5, v6, v7;
            fgets(line, 1024, fp);
            sscanf(line, "%d", &nhexa);
            for( int i=0; i<nhexa; i++ )
            {
                fgets(line,1024,fp);
                sscanf(line, "%d %d %d %d %d %d %d %d", &v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7);
                hexa.push_back(v0 - 1);
                hexa.push_back(v1 - 1);
                hexa.push_back(v2 - 1);
                hexa.push_back(v3 - 1);
                hexa.push_back(v4 - 1);
                hexa.push_back(v5 - 1);
                hexa.push_back(v6 - 1);
                hexa.push_back(v7 - 1);
            }
        }
    }

    fclose(fp);
}

}
