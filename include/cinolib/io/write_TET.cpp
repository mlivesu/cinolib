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
#include <cinolib/io/write_TET.h>


#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_TET(const char                           * filename,
               const std::vector<vec3d>             & verts,
               const std::vector<std::vector<uint>> & tets)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_TET() : couldn't write output file " << filename << std::endl;
        exit(-1);
    }

    fprintf(fp, "%d vertices\n", (int)verts.size());
    fprintf(fp, "%d tets\n",     (int)tets.size());

    for(const vec3d & v : verts)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "%.17g %.17g %.17g\n", v.x(), v.y(), v.z());
    }

    for(const std::vector<uint> & tet : tets)
    {
        fprintf(fp, "4 %d %d %d %d\n", tet.at(0), tet.at(1), tet.at(2), tet.at(3));
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_TET(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<uint>   & tets)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_TET() : couldn't write output file " << filename << std::endl;
        exit(-1);
    }

    int nv = (int)xyz.size()/3;
    int nt = (int)tets.size()/4;

    fprintf(fp, "%d vertices\n", nv);
    fprintf(fp, "%d tets\n",     nt);

    if (nv > 0)
    {
        for(size_t i=0; i<xyz.size(); i+=3)
        {
            // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
            //
            fprintf(fp, "%.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
        }
    }

    if (nt > 0)
    {
        for(size_t i=0; i<tets.size(); i+=4)
        {
            fprintf(fp, "4 %d %d %d %d\n", tets[i+0], tets[i+3], tets[i+2], tets[i+1]);
        }
    }

    fclose(fp);
}

}
