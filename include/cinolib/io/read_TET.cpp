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
#include <cinolib/io/read_TET.h>
#include <iostream>

namespace cinolib
{

CINO_INLINE
void read_TET(const char          * filename,
              std::vector<double> & xyz,
              std::vector<uint>  & tets)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_TET() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    uint  nv, nt;
    char line[1024];

    fgets(line,1024,fp);
    sscanf(line, "%d vertices", &nv);

    fgets(line,1024,fp);
    sscanf(line, "%d tets", &nt);

    for(uint vid=0; vid<nv; ++vid)
    {
        fgets(line, 1024, fp);

        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        double x,y,z;
        sscanf(line, "%lf %lf %lf ", &x, &y, &z);

        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(uint tid=0; tid<nt; ++tid)
    {
        fgets(line, 1024, fp);

        uint v0, v1, v2, v3;
        sscanf(line, "4 %d %d %d %d", &v0, &v1, &v2, &v3);

        tets.push_back(v0);
        tets.push_back(v3);
        tets.push_back(v2);
        tets.push_back(v1);
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_TET(const char                     * filename,
              std::vector<vec3d>             & verts,
              std::vector<std::vector<uint>> & polys)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_TET() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    uint  nv, np;
    char line[1024];

    fgets(line,1024,fp);
    sscanf(line, "%d vertices", &nv);

    fgets(line,1024,fp);
    sscanf(line, "%d tets", &np);

    verts.reserve(nv);
    polys.reserve(np);

    for(uint vid=0; vid<nv; ++vid)
    {
        fgets(line, 1024, fp);

        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        vec3d p;
        sscanf(line, "%lf %lf %lf ", &p.x(), &p.y(), &p.z());
        verts.push_back(p);
    }

    for(uint pid=0; pid<np; ++pid)
    {
        fgets(line, 1024, fp);

        uint v0, v1, v2, v3;
        sscanf(line, "4 %d %d %d %d", &v0, &v1, &v2, &v3);

        std::vector<uint> tet;
        tet.push_back(v0);
        tet.push_back(v3);
        tet.push_back(v2);
        tet.push_back(v1);
        polys.push_back(tet);
    }

    fclose(fp);
}

}
