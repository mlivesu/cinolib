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
#include <cinolib/vector_serialization.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> vec3d_from_serialized_xyz(const std::vector<double> & coords)
{
    uint nv = coords.size()/3;
    std::vector<vec3d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec3d(coords[3*vid+0], coords[3*vid+1], coords[3*vid+2]);
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> vec3d_from_serialized_xy(const std::vector<double> & coords, const double z)
{
    uint nv = coords.size()/2;
    std::vector<vec3d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec3d(coords[2*vid+0], coords[2*vid+1], z);
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> serialized_xyz_from_vec3d(const std::vector<vec3d> & verts)
{
    uint nv = verts.size();
    std::vector<double> tmp(3*nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        tmp.at(3*vid  ) = verts.at(vid).x();
        tmp.at(3*vid+1) = verts.at(vid).y();
        tmp.at(3*vid+2) = verts.at(vid).z();
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<std::vector<uint>> polys_from_serialized_vids(const std::vector<uint> & vids, const uint vids_per_poly)
{
    uint nf = vids.size()/vids_per_poly;
    std::vector<std::vector<uint>> tmp(nf);
    for(uint fid=0; fid<nf; ++fid)
    {
        for(uint off=0; off<vids_per_poly; ++off) tmp.at(fid).push_back(vids.at(fid*vids_per_poly+off));
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<uint> serialized_vids_from_polys(const std::vector<std::vector<uint>> & polys)
{
    std::vector<uint> tmp;
    for(const auto & f   : polys)
    for(const auto & vid : f    )
    {
        tmp.push_back(vid);
    }
    return tmp;
}

}
