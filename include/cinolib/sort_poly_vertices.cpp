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
#include <cinolib/sort_poly_vertices.h>
#include <cinolib/stl_container_utilities.h>
#include <cinolib/ipair.h>
#include <map>
#include <set>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sort_poly_vertices_as_hexa(const std::vector<std::vector<uint>> & poly_faces,
                                const std::vector<bool>              & poly_winding,
                                      std::vector<uint>              & hexa)
{
    assert(poly_faces.size()==6);
    assert(poly_winding.size()==6);

    // vertex 2 vertex adjacency
    std::map<uint,std::set<uint>> v2v;
    for(auto f : poly_faces)
    {
        assert(f.size()==4);
        v2v[f.at(0)].insert(f.at(1));
        v2v[f.at(1)].insert(f.at(0));
        v2v[f.at(1)].insert(f.at(2));
        v2v[f.at(2)].insert(f.at(1));
        v2v[f.at(2)].insert(f.at(3));
        v2v[f.at(3)].insert(f.at(2));
        v2v[f.at(3)].insert(f.at(0));
        v2v[f.at(0)].insert(f.at(3));
    }

    // pick a face
    hexa = poly_faces.at(0);
    if(poly_winding.at(0)) std::swap(hexa.at(1),hexa.at(3));

    // determine opposite face using v2v
    for(uint i=0; i<4; ++i)
    for(uint vid : v2v.at(hexa.at(i)))
    {
        if(!CONTAINS_VEC(hexa,vid)) hexa.push_back(vid);
    }
    assert(hexa.size()==8);
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sort_poly_vertices_as_hexa(const std::vector<std::vector<uint>> & faces,
                                const std::vector<std::vector<uint>> & polys,
                                const std::vector<std::vector<bool>> & winding,
                                      std::vector<std::vector<uint>> & hexas)
{
    hexas.resize(polys.size());
    for(uint pid=0; pid<polys.size(); ++pid)
    {
        std::vector<std::vector<uint>> f;
        for(uint fid : polys.at(pid)) f.push_back(faces.at(fid));
        sort_poly_vertices_as_hexa(f, winding.at(pid), hexas.at(pid));
    }
}

}
