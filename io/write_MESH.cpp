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
#include <cinolib/io/write_MESH.h>

#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_MESH(const char                           * filename,
                const std::vector<vec3d>             & verts,
                const std::vector<std::vector<uint>> & polys)
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

    uint nv = verts.size();
    uint nt = 0;
    uint nh = 0;
    for(auto p : polys)
    {
        if (p.size() == 4) ++nt; else
        if (p.size() == 8) ++nh;
    }

    if (nv > 0)
    {
        fprintf(fp, "Vertices\n" );
        fprintf(fp, "%d\n", nv);
        for(uint vid=0; vid<nv; ++vid)
        {
            // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
            //
            fprintf( fp, "%.17g %.17g %.17g 0\n", verts.at(vid).x(), verts.at(vid).y(), verts.at(vid).z());
        }
    }

    if (nt > 0)
    {
        fprintf(fp, "Tetrahedra\n" );
        fprintf(fp, "%d\n", nt );
        for(auto tet : polys)
        {
            if (tet.size() == 4)
            {
                fprintf(fp, "%d %d %d %d 0\n", tet.at(0)+1, tet.at(1)+1, tet.at(2)+1, tet.at(3)+1);
            }
        }
    }

    if (nh > 0)
    {
        fprintf(fp, "Hexahedra\n" );
        fprintf(fp, "%d\n", nh );
        for(auto hex : polys)
        {
            if (hex.size() == 8)
            {
                fprintf(fp, "%d %d %d %d %d %d %d %d 0\n", hex.at(0)+1, hex.at(1)+1, hex.at(2)+1, hex.at(3)+1,
                                                           hex.at(4)+1, hex.at(5)+1, hex.at(6)+1, hex.at(7)+1);
            }
        }
    }

    fprintf(fp, "End\n\n");
    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_MESH(const char                * filename,
                const std::vector<double> & xyz,
                const std::vector<uint>   & tets,
                const std::vector<uint>   & hexa)
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
