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
#include <cinolib/io/write_OFF.h>


#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_OFF(const char                * filename,
              const std::vector<double> & xyz,
              const std::vector<uint>  & tri,
              const std::vector<uint>  & quad)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OFF() : couldn't save file " << filename << std::endl;
        exit(-1);
    }

    int n_poly = tri.size()/3 + quad.size()/4;
    fprintf (fp, "OFF\n%zu %d 0\n", xyz.size()/3, n_poly);

    for(size_t i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "%.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(size_t i=0; i<tri.size(); i+=3)
    {
        fprintf(fp, "3 %d %d %d\n", tri[i], tri[i+1], tri[i+2]);
    }

    for(size_t i=0; i<quad.size(); i+=4)
    {
        fprintf(fp, "4 %d %d %d %d\n", quad[i], quad[i+1], quad[i+2], quad[i+3]);
    }

    fclose(fp);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void write_OFF(const char                           * filename,
               const std::vector<double>            & xyz,
               const std::vector<std::vector<uint>> & faces)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_OFF() : couldn't save file " << filename << std::endl;
        exit(-1);
    }

    uint n_faces = faces.size();
    fprintf (fp, "OFF\n%zu %d 0\n", xyz.size()/3, n_faces);

    for(uint i=0; i<xyz.size(); i+=3)
    {
        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fprintf(fp, "%.17g %.17g %.17g\n", xyz[i], xyz[i+1], xyz[i+2]);
    }

    for(uint i=0; i<faces.size(); ++i)
    {
        fprintf(fp, "%d ", static_cast<int>(faces.at(i).size()));
        for(uint j=0; j<faces.at(i).size(); ++j)
        {
            fprintf(fp, "%d ", faces.at(i).at(j));
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}


}
