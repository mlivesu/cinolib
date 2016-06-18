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
#include "read_LIVESU2012.h"


namespace cinolib
{

typedef std::pair<int,int> arc;

static CINO_INLINE arc unique_arc(int v0, int v1)
{
    arc a;
    a.first  = std::min(v0,v1);
    a.second = std::max(v0,v1);
    return a;
}


CINO_INLINE
void read_Livesu2012(const char          * filename,
                     std::vector<double> & coords,
                     std::vector<int>    & arcs,
                     std::vector<double> & radius)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    coords.clear();
    arcs.clear();
    radius.clear();

    FILE *f = fopen(filename,"r");

    if (!f)
    {
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_Livesu2012() : error while reading file " << filename << endl;
        exit(-1);
    }

    fscanf(f, "ID Cx Cy Cz RADIUS #NEIGHBORS NEIGHBORS_LIST\n");

    int count;
    fscanf(f,"%d\n",&count);

    std::set<arc>     arc_set;
    std::map<int,int> id_map;

    for (int i=0; i<count; ++i)
    {
        double      x, y, z, r;
        int         id, n_arcs;

        // http://stackoverflow.com/questions/16839658/printf-width-specifier-to-maintain-precision-of-floating-point-value
        //
        fscanf(f, "%d %.17g %.17g %.17g %.17g %d ", &id, &x, &y, &z, &r, &n_arcs);

        for (int j=0; j<n_arcs; ++j)
        {
            int nbr;
            fscanf(f, "%d ", &nbr);
            arc_set.insert(unique_arc(id,nbr));
        }

        if (n_arcs > 0) // discard isolated points
        {
            id_map[id] = coords.size() / 3;
            coords.push_back(x);
            coords.push_back(y);
            coords.push_back(z);
            radius.push_back(r);
        }
    }

    fclose(f);

    //
    // assign progressive IDs, from 0 to size(skel)
    //

    for(std::set<arc>::iterator it=arc_set.begin(); it!=arc_set.end(); ++it)
    {
        arc a = *it;

        int id0 = id_map[a.first];
        int id1 = id_map[a.second];

        arcs.push_back(id0);
        arcs.push_back(id1);
    }

    logger << coords.size() / 3 << " skel points read" << endl;
    logger << arcs.size()   / 2 << " skel arcs   read" << endl;
}

}
