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
#include <cinolib/io/read_LIVESU2012.h>

#include <map>
#include <set>

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
        std::cerr << "ERROR : " << __FILE__ << ", line " << __LINE__ << " : load_Livesu2012() : error while reading file " << filename << std::endl;
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
        fscanf(f, "%d %lf %lf %lf %lf %d ", &id, &x, &y, &z, &r, &n_arcs);

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

    std::cout << coords.size() / 3 << " skel points read" << std::endl;
    std::cout << arcs.size()   / 2 << " skel arcs   read" << std::endl;
}

}
