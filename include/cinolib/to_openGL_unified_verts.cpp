/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/to_openGL_unified_verts.h>
#include <map>
#include <tuple>

namespace cinolib
{

// https://stackoverflow.com/questions/29867926/why-does-the-number-of-vt-and-v-elements-in-a-blender-obj-file-differ
//
CINO_INLINE
void to_openGL_unified_verts(const std::vector<vec3d>             & v_attr_0,          // xyz or uvw or nor
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
void to_openGL_unified_verts(const std::vector<vec3d>             & v_attr_0,
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
