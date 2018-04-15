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
#include <map>
#include <tuple>

namespace cinolib
{

// https://stackoverflow.com/questions/29867926/why-does-the-number-of-vt-and-v-elements-in-a-blender-obj-file-differ
//
CINO_INLINE
void cut_mesh_along_seams(const std::vector<vec3d>             & v_attr_0,          // xyz or uvw or nor
                          const std::vector<vec3d>             & v_attr_1,          // xyz or uvw or nor
                          const std::vector<std::vector<uint>> & v2v_v_attr_0,      // connectivity attr #0
                          const std::vector<std::vector<uint>> & v2v_v_attr_1,      // connectivity attr #1
                                std::vector<vec3d>             & unified_v_attr_0,
                                std::vector<vec3d>             & unified_v_attr_1,
                                std::vector<std::vector<uint>> & unified_v2v)
{
    assert(v2v_v_attr_0.size() == v2v_v_attr_1.size());

    unified_v_attr_0.clear();
    unified_v_attr_1.clear();
    unified_v2v.clear();
    unified_v2v.reserve(v2v_v_attr_0.size());

    typedef std::pair<uint,uint> v_vt_pair;
    std::map<v_vt_pair,uint> v_map;

    for(uint pid=0; pid<v2v_v_attr_0.size(); ++pid)
    {
        assert(v2v_v_attr_0.at(pid).size() == v2v_v_attr_1.at(pid).size());
        std::vector<uint> poly;
        for(uint off=0; off<v2v_v_attr_0.at(pid).size(); ++off)
        {
            uint v  = v2v_v_attr_0.at(pid).at(off);
            uint vt = v2v_v_attr_1.at(pid).at(off);
            v_vt_pair key = std::make_pair(v,vt);

            auto query = v_map.find(key);
            if (query == v_map.end())
            {
                uint fresh_id = unified_v_attr_0.size();
                v_map[key] = fresh_id;
                unified_v_attr_0.push_back(v_attr_0.at(v));
                unified_v_attr_1.push_back(v_attr_1.at(vt));
                poly.push_back(fresh_id);
            }
            else
            {
                poly.push_back(query->second);
            }
        }
        unified_v2v.push_back(poly);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void cut_mesh_along_seams(const std::vector<vec3d>             & v_attr_0,
                          const std::vector<vec3d>             & v_attr_1,
                          const std::vector<vec3d>             & v_attr_2,
                          const std::vector<std::vector<uint>> & v2v_attr_0,
                          const std::vector<std::vector<uint>> & v2v_attr_1,
                          const std::vector<std::vector<uint>> & v2v_attr_2,
                                std::vector<vec3d>             & unified_v_attr_0,
                                std::vector<vec3d>             & unified_v_attr_1,
                                std::vector<vec3d>             & unified_v_attr_2,
                                std::vector<std::vector<uint>> & unified_v2v)
{
    assert(v2v_attr_0.size() == v2v_attr_1.size());

    unified_v_attr_0.clear();
    unified_v_attr_1.clear();
    unified_v2v.clear();
    unified_v2v.reserve(v2v_attr_0.size());

    typedef std::tuple<uint,uint,uint> v_vt_vn;
    std::map<v_vt_vn,uint> v_map;

    for(uint pid=0; pid<v2v_attr_0.size(); ++pid)
    {
        assert(v2v_attr_0.at(pid).size() == v2v_attr_1.at(pid).size());
        assert(v2v_attr_0.at(pid).size() == v2v_attr_2.at(pid).size());

        std::vector<uint> poly;
        for(uint off=0; off<v2v_attr_0.at(pid).size(); ++off)
        {
            uint v  = v2v_attr_0.at(pid).at(off);
            uint vt = v2v_attr_1.at(pid).at(off);
            uint vn = v2v_attr_2.at(pid).at(off);
            v_vt_vn key = std::make_tuple(v,vt,vn);

            auto query = v_map.find(key);
            if (query == v_map.end())
            {
                uint fresh_id = unified_v_attr_0.size();
                v_map[key] = fresh_id;
                unified_v_attr_0.push_back(v_attr_0.at(v));
                unified_v_attr_1.push_back(v_attr_1.at(vt));
                unified_v_attr_2.push_back(v_attr_2.at(vn));
                poly.push_back(fresh_id);
            }
            else
            {
                poly.push_back(query->second);
            }
        }
        unified_v2v.push_back(poly);
    }
}

}
