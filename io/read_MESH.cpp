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
#include <cinolib/io/read_MESH.h>

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
            uint   nverts;
            double x,y,z;
            fgets(line,1024,fp);
            sscanf(line, "%d", &nverts);
            for(uint i=0; i<nverts; ++i)
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
            uint ntets, v0, v1, v2, v3;
            fgets(line, 1024, fp);
            sscanf(line, "%d", &ntets);
            for(uint i=0; i<ntets; i++ )
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
            uint nhexa, v0, v1, v2, v3, v4, v5, v6, v7;
            fgets(line, 1024, fp);
            sscanf(line, "%d", &nhexa);
            for(uint i=0; i<nhexa; i++ )
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
