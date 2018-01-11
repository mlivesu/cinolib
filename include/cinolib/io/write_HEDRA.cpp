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
#include <cinolib/io/write_HEDRA.h>
#include <iostream>

namespace cinolib
{

CINO_INLINE
void write_HEDRA(const char                           * filename,
                 const std::vector<vec3d>             & verts,
                 const std::vector<std::vector<uint>> & faces,
                 const std::vector<std::vector<uint>> & polys,
                 const std::vector<std::vector<bool>> & polys_winding)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *fp = fopen(filename, "w");

    if(!fp)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_HEDRA() : couldn't write output file " << filename << std::endl;
        exit(-1);
    }

    uint nv = verts.size();
    uint nf = faces.size();
    uint np = polys.size();

    fprintf(fp, "%d %d %d\n", nv, nf, np);

    // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
    //
    for(const vec3d & v : verts) fprintf(fp, "%.17g %.17g %.17g\n", v.x(), v.y(), v.z());

    for(const std::vector<uint> & f : faces)
    {
        fprintf(fp, "%d ", static_cast<int>(f.size()));
        for(uint vid : f) fprintf(fp, "%d ", vid+1);
        fprintf(fp, "\n");
    }

    for(uint pid=0; pid<np; ++pid)
    {
        fprintf(fp, "%d ", static_cast<int>(polys.at(pid).size()));

        for(uint off=0; off<polys.at(pid).size(); ++off)
        {
            if (polys_winding.at(pid).at(off)) fprintf(fp, "%d ",   polys.at(pid).at(off)+1);
            else                               fprintf(fp, "%d ", -(polys.at(pid).at(off)+1));
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

}
