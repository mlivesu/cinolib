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
#include "write_OBJ.h"

#include <iostream>

namespace cinolib
{


CINO_INLINE
void write_OBJ(const char                * filename,
               const std::vector<double> & xyz,
               const std::vector<u_int>  & tri)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OBJ() : couldn't open input file " << filename << std::endl;
        exit(-1);
    }

    for(size_t i=0; i<xyz.size(); i+=3)
    {
        fprintf(fp, "v %lf %lf %lf\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(size_t i=0; i<tri.size(); i+=3)
    {
        fprintf(fp, "f %d %d %d\n", tri[i] + 1, tri[i+1] + 1, tri[i+2] + 1);
    }

    fclose(fp);
}

}
