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

#include "tetrahedralization.h"

#include <set>

namespace cinolib
{

CINO_INLINE
void tetrahedralize_prism(int prism[], std::vector<u_int> & tets)
{
    std::set<ipair> ordered_vids;
    for(int i=0; i<6; ++i) ordered_vids.insert(ipair(prism[i],i));

    // see Table 2 in "How to Subdivide Pyramids, Prisms and Hexahedra into Tetrahedra"
    //
    int ref_prism[6];
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
