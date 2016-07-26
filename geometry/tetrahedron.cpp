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
#include "tetrahedron.h"
#include "../geometry/vec3.h"
#include "../geometry/segment.h"

#include <set>

namespace cinolib
{

CINO_INLINE
void tet_closest_vertex(const vec3d  & A,
                        const vec3d  & B,
                        const vec3d  & C,
                        const vec3d  & D,
                        const vec3d  & query,
                              int    & id,
                              double & dist)
{
    vec3d tet[4] = { A, B, C, D };

    std::set< std::pair<double,int> > sorted_verts;
    for(int i=0; i<4; ++i)
    {
        sorted_verts.insert(std::make_pair(tet[i].dist(query),i));
    }

    dist = (*sorted_verts.begin()).first;
    id   = (*sorted_verts.begin()).second;
}


CINO_INLINE
void tet_closest_edge(const vec3d  & A,
                      const vec3d  & B,
                      const vec3d  & C,
                      const vec3d  & D,
                      const vec3d  & query,
                            int    & id,
                            double & dist)
{
    vec3d tet[4] = { A, B, C, D };

    std::set< std::pair<double,int> > sorted_segs;
    for(int i=0; i<6; ++i)
    {
        Segment s(tet[TET_EDGES[i][0]], tet[TET_EDGES[i][1]]);
        sorted_segs.insert(std::make_pair(s.dist_to_point(query),i));
    }

    dist = (*sorted_segs.begin()).first;
    id   = (*sorted_segs.begin()).second;
}


}

