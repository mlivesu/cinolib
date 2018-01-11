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
#include <cinolib/io/read_HEXEX.h>
#include <sstream>
#include <iostream>
#include <assert.h>

namespace cinolib
{

CINO_INLINE
void read_HEXEX(const char         * filename,
                std::vector<vec3d> & verts,
                std::vector<uint>  & tets,        // serialized tets (4 vids per tet)
                std::vector<vec3d> & tets_param) // tets param (4 points per tet)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    verts.clear();
    tets.clear();
    tets_param.clear();

    FILE *f = fopen(filename, "r");

    if(!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : read_HEXEX() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    int nv;
    fscanf(f,"%d\n", &nv);
    verts = std::vector<vec3d>(nv);
    for(int vid=0; vid<nv; ++vid)
    {
        fscanf(f,"%lf %lf %lf", &verts.at(vid).x(), &verts.at(vid).y(), &verts.at(vid).z());
    }

    int nt;
    fscanf(f,"%d\n", &nt);
    tets.reserve(4*nt);
    tets_param = std::vector<vec3d>(4*nt);
    for(int tid=0; tid<nt; ++tid)
    {
        uint v0,v1,v2,v3;
        fscanf(f,"%d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
               &v0, &v1, &v2, &v3,
               &tets_param.at(4*tid+0).x(),
               &tets_param.at(4*tid+0).y(),
               &tets_param.at(4*tid+0).z(),
               &tets_param.at(4*tid+1).x(),
               &tets_param.at(4*tid+1).y(),
               &tets_param.at(4*tid+1).z(),
               &tets_param.at(4*tid+2).x(),
               &tets_param.at(4*tid+2).y(),
               &tets_param.at(4*tid+2).z(),
               &tets_param.at(4*tid+3).x(),
               &tets_param.at(4*tid+3).y(),
               &tets_param.at(4*tid+3).z());

        tets.push_back(v0);
        tets.push_back(v1);
        tets.push_back(v2);
        tets.push_back(v3);
    }

    fclose(f);
}
}
