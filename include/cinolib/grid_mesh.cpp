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
#include <cinolib/grid_mesh.h>
#include <cinolib/serialize_2D.h>
#include <vector>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void grid_mesh(const uint                quads_per_row,
               const uint                quads_per_col,
                     Quadmesh<M,V,E,P> & m)
{
    std::vector<vec3d> points;
    std::vector<uint>  polys;
    for(uint r=0; r<=quads_per_row; ++r)
    for(uint c=0; c<=quads_per_col; ++c)
    {
        points.push_back(vec3d(c, r,0));

        if (r<quads_per_row && c<quads_per_col)
        {
            polys.push_back(serialize_2D_index(r  , c,   quads_per_col+1));
            polys.push_back(serialize_2D_index(r  , c+1, quads_per_col+1));
            polys.push_back(serialize_2D_index(r+1, c+1, quads_per_col+1));
            polys.push_back(serialize_2D_index(r+1, c,   quads_per_col+1));
        }
    }
    m = Quadmesh<M,V,E,P>(points, polys);
}

}
