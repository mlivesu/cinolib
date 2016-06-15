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
#include "write_TET.h"

#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_TET(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tets)
{
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
            fprintf(fp, "%lf %lf %lf\n", xyz[i], xyz[i+1], xyz[i+2]);
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
