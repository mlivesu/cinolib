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
#include <cinolib/connected_components.h>
#include <cinolib/bfs.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
uint connected_components(const AbstractMesh<M,V,E,P> & m)
{
    std::vector<std::set<uint>> ccs;
    return connected_components(m, ccs);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
uint connected_components(const AbstractMesh<M,V,E,P> & m,
                          std::vector<std::set<uint>> & ccs)
{
    ccs.clear();
    uint seed = 0;
    std::vector<bool> visited(m.num_verts(), false);

    do
    {
        std::set<uint> cc;
        bfs_exahustive(m, seed, cc);

        ccs.push_back(cc);
        for(uint vid : cc) visited.at(vid) = true;

        seed = 0;
        while (seed < num_verts() && visited.at(seed)) ++seed;
    }
    while (seed < m.num_verts());

    return ccs.size();
}

}
