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
#include <cinolib/io/write_LIVESU2012.h>

#include <vector>

namespace cinolib
{

CINO_INLINE
void write_Livesu2012(const char                       * filename,
                      const std::vector<double>           coords,
                      const std::vector<double>           max_spheres,
                      const std::vector<std::vector<int>> adj_vtx2vtx)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    FILE *f = fopen(filename,"w");

    if (!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_Livesu2012() : error while opening file " << filename << std::endl;
        exit(-1);
    }

    uint nv = adj_vtx2vtx.size();

    fprintf( f, "ID Cx Cy Cz RADIUS #NEIGHBORS NEIGHBORS_LIST\n%d\n", nv);

    for(uint vid=0; vid<nv; ++vid)
    {
        vec3d pos(coords[3*vid+0],
                  coords[3*vid+1],
                  coords[3*vid+2]);

        std::vector<int> nbrs = adj_vtx2vtx.at(vid);

        fprintf(f,
                "%d %f %f %f %f %d ",
                vid,
                pos.x(),
                pos.y(),
                pos.z(),
                max_spheres.at(vid),
                (int)nbrs.size());

        for(uint i=0; i<nbrs.size(); ++i)
        {
            fprintf(f, "%d ", nbrs[i]);
        }

        fprintf(f, "\n");
    }

    fclose(f);
}

}
