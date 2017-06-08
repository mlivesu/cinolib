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
#include <cinolib/io/read_OFF.h>

#include <iostream>

namespace cinolib
{

CINO_INLINE
void read_OFF(const char          * filename,
              std::vector<double> & xyz,
              std::vector<uint>   & tri,
              std::vector<uint>   & quad)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OFF() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    uint nv, npoly, dummy;

    fscanf(fp, "OFF\n");
    fscanf(fp, "%d %d %d\n", &nv, &npoly, &dummy);

    for(uint i=0; i<nv; ++i)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        double x, y, z;
        fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(uint i=0; i<npoly; ++i)
    {
        uint n_corners, v0, v1, v2, v3;
        fscanf(fp, "%d", &n_corners);

        if (n_corners == 3)
        {
            fscanf(fp, "%d %d %d\n", &v0, &v1, &v2);
            tri.push_back(v0);
            tri.push_back(v1);
            tri.push_back(v2);
        }
        else if (n_corners == 4)
        {
            fscanf(fp, "%d %d %d %d\n", &v0, &v1, &v2, &v3);
            quad.push_back(v0);
            quad.push_back(v1);
            quad.push_back(v2);
            quad.push_back(v3);
        }
        else
        {
            std::cerr << "read_OFF: polygons with " << n_corners << " corners are not supported!" << std::endl;
            assert("Unsupported polygon" && false);
        }
    }
    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void read_OFF(const char                     * filename,
              std::vector<double>            & xyz,
              std::vector<std::vector<uint>> & faces)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "r");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_OFF() : couldn't open input file " << filename << endl;
        exit(-1);
    }

    uint nv, nfaces, dummy;

    fscanf(fp, "OFF\n");
    fscanf(fp, "%d %d %d\n", &nv, &nfaces, &dummy);

    for(uint i=0; i<nv; ++i)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        double x, y, z;
        fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
        xyz.push_back(x);
        xyz.push_back(y);
        xyz.push_back(z);
    }

    for(uint i=0; i<nfaces; ++i)
    {
        uint n_corners;
        fscanf(fp, "%d", &n_corners);

        std::vector<uint> face;
        for(uint j=0; j<n_corners; ++j)
        {
            uint vid;
            fscanf(fp, "%d", &vid);
            face.push_back(vid);
        }
        faces.push_back(face);
    }
    fclose(fp);
}


}
