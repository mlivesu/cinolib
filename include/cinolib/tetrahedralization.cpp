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
#include <cinolib/tetrahedralization.h>
#include <cinolib/ipair.h>
#include <set>

namespace cinolib
{

CINO_INLINE
void tetrahedralize_prism(uint prism[], std::vector<uint> & tets)
{
    std::set<ipair> ordered_vids;
    for(uint i=0; i<6; ++i) ordered_vids.insert(ipair(prism[i],i));

    // see Table 2 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    //
    uint ref_prism[6];
    switch ((*ordered_vids.begin()).second)
    {
        case 0: ref_prism[0] = prism[0];
                ref_prism[1] = prism[1];
                ref_prism[2] = prism[2];
                ref_prism[3] = prism[3];
                ref_prism[4] = prism[4];
                ref_prism[5] = prism[5];
                break;

        case 1: ref_prism[0] = prism[1];
                ref_prism[1] = prism[2];
                ref_prism[2] = prism[0];
                ref_prism[3] = prism[4];
                ref_prism[4] = prism[5];
                ref_prism[5] = prism[3];
                break;

        case 2: ref_prism[0] = prism[2];
                ref_prism[1] = prism[0];
                ref_prism[2] = prism[1];
                ref_prism[3] = prism[5];
                ref_prism[4] = prism[3];
                ref_prism[5] = prism[4];
                break;

        case 3: ref_prism[0] = prism[3];
                ref_prism[1] = prism[5];
                ref_prism[2] = prism[4];
                ref_prism[3] = prism[0];
                ref_prism[4] = prism[2];
                ref_prism[5] = prism[1];
                break;

        case 4: ref_prism[0] = prism[4];
                ref_prism[1] = prism[3];
                ref_prism[2] = prism[5];
                ref_prism[3] = prism[1];
                ref_prism[4] = prism[0];
                ref_prism[5] = prism[2];
                break;

        case 5: ref_prism[0] = prism[5];
                ref_prism[1] = prism[4];
                ref_prism[2] = prism[3];
                ref_prism[3] = prism[2];
                ref_prism[4] = prism[1];
                ref_prism[5] = prism[0];
                break;

        default: assert(false);
    }

    // see Table 3 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    //
    if (std::min(ref_prism[1],ref_prism[5]) < std::min(ref_prism[2],ref_prism[4]))
    {
        tets.push_back(ref_prism[0]); tets.push_back(ref_prism[1]); tets.push_back(ref_prism[2]); tets.push_back(ref_prism[5]);
        tets.push_back(ref_prism[0]); tets.push_back(ref_prism[1]); tets.push_back(ref_prism[5]); tets.push_back(ref_prism[4]);
    }
    else
    {
        tets.push_back(ref_prism[0]); tets.push_back(ref_prism[1]); tets.push_back(ref_prism[2]); tets.push_back(ref_prism[4]);
        tets.push_back(ref_prism[0]); tets.push_back(ref_prism[4]); tets.push_back(ref_prism[2]); tets.push_back(ref_prism[5]);
    }
    tets.push_back(ref_prism[0]); tets.push_back(ref_prism[4]); tets.push_back(ref_prism[5]); tets.push_back(ref_prism[3]);
}

}
