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
#include "write_LIVESU2012.h"

#include <vector>

namespace cinolib
{

CINO_INLINE
void write_Livesu2012(const char                          * filename,
                      const std::vector<double>             coords,
                      const std::vector<double>             max_spheres,
                      const std::vector< std::vector<int> > adj_vtx2vtx)
{
    FILE *f = fopen(filename,"w");

    if (!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : save_Livesu2012() : error while opening file " << filename << endl;
        exit(-1);
    }

    int nv = adj_vtx2vtx.size();

    fprintf( f, "ID Cx Cy Cz RADIUS #NEIGHBORS NEIGHBORS_LIST\n%d\n", nv);

    for(int vid=0; vid<nv; ++vid)
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

        for(int i=0; i<(int)nbrs.size(); ++i)
        {
            fprintf(f, "%d ", nbrs[i]);
        }

        fprintf(f, "\n");
    }

    fclose(f);
}

}
