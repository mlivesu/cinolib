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
#include <cinolib/cut_along_seams.h>

namespace cinolib
{

// https://stackoverflow.com/questions/29867926/why-does-the-number-of-vt-and-v-elements-in-a-blender-obj-file-differ
//
CINO_INLINE
void cut_mesh_along_seams(const std::vector<vec3d>             & xyz,
                          const std::vector<vec3d>             & uvw,
                          const std::vector<std::vector<uint>> & xyz_poly,
                          const std::vector<std::vector<uint>> & uvw_poly,
                                std::vector<vec3d>             & unified_xyz,
                                std::vector<vec3d>             & unified_uvw,
                                std::vector<std::vector<uint>> & unified_poly)
{
    assert(xyz_poly.size() == uvw_poly.size());

    unified_xyz.clear();
    unified_uvw.clear();
    unified_poly.clear();
    unified_poly.reserve(xyz_poly.size());

    typedef std::pair<uint,uint> v_vt_pair;
    std::map<v_vt_pair,uint> v_map;

    for(uint pid=0; pid<xyz_poly.size(); ++pid)
    {
        assert(xyz_poly.at(pid).size() == uvw_poly.at(pid).size());
        std::vector<uint> poly;
        for(uint off=0; off<xyz_poly.at(pid).size(); ++off)
        {
            uint v  = xyz_poly.at(pid).at(off);
            uint vt = uvw_poly.at(pid).at(off);
            v_vt_pair key = std::make_pair(v,vt);

            auto query = v_map.find(key);
            if (query == v_map.end())
            {
                uint fresh_id = unified_xyz.size();
                v_map[key] = fresh_id;
                unified_xyz.push_back(xyz.at(v));
                unified_uvw.push_back(uvw.at(vt));
                poly.push_back(fresh_id);
            }
            else
            {
                poly.push_back(query->second);
            }
        }
        unified_poly.push_back(poly);
    }
}

}
