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
#include <cinolib/io/write_MESH.h>

#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_MESH(const char                * filename,
                const std::vector<double> & xyz,
                const std::vector<u_int>  & tets,
                const std::vector<u_int>  & hexa)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_MESH() : couldn't write output file " << filename << endl;
        exit(-1);
    }

    fprintf(fp, "MeshVersionFormatted 1\n" );
    fprintf(fp, "Dimension 3\n" );

    int nv = (int)xyz.size()/3;
    int nt = (int)tets.size()/4;
    int nh = (int)hexa.size()/8;

    if (nv > 0)
    {
        fprintf(fp, "Vertices\n" );
        fprintf(fp, "%d\n", nv);

        for(size_t i=0; i<xyz.size(); i+=3)
        {
            // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
            //
            fprintf( fp, "%.17g %.17g %.17g 0\n", xyz[i], xyz[i+1], xyz[i+2]);
        }
    }

    if (nt > 0)
    {
        fprintf(fp, "Tetrahedra\n" );
        fprintf(fp, "%d\n", nt );

        for(size_t i=0; i<tets.size(); i+=4)
        {
            fprintf(fp, "%d %d %d %d 0\n", tets[i+0]+1, tets[i+1]+1, tets[i+2]+1, tets[i+3]+1);
        }
    }

    if (nh > 0)
    {
        fprintf(fp, "Hexahedra\n" );
        fprintf(fp, "%d\n", nh );

        for(size_t i=0; i<hexa.size(); i+=8)
        {
            fprintf(fp, "%d %d %d %d %d %d %d %d 0\n", hexa[i+0]+1, hexa[i+1]+1, hexa[i+2]+1, hexa[i+3]+1,
                                                       hexa[i+4]+1, hexa[i+5]+1, hexa[i+6]+1, hexa[i+7]+1);
        }
    }

    fprintf(fp, "End\n\n");
    fclose(fp);
}

}
