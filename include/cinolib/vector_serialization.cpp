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
#include <cinolib/vector_serialization.h>

namespace cinolib
{

CINO_INLINE
std::vector<vec3d> vec3d_from_serialized_xyz(const std::vector<double> & coords)
{
    uint nv = coords.size()/3;
    std::vector<vec3d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec3d(coords.at(3*vid+0), coords.at(3*vid+1), coords.at(3*vid+2));
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> vec3d_from_serialized_xy(const std::vector<double> & coords, const double z)
{
    uint nv = coords.size()/2;
    std::vector<vec3d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec3d(coords.at(2*vid+0), coords.at(2*vid+1), z);
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d> vec3d_from_vec2d(const std::vector<vec2d> & verts, const double z)
{
    uint nv = verts.size();
    std::vector<vec3d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec3d(verts.at(vid).x(), verts.at(vid).y(), z);
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
std::vector<double> serialized_xyz_from_vec2d(const std::vector<vec2d> & verts, const double z)
{
    uint nv = verts.size();
    std::vector<double> tmp(3*nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        tmp.at(3*vid  ) = verts.at(vid).x();
        tmp.at(3*vid+1) = verts.at(vid).y();
        tmp.at(3*vid+2) = z;
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> serialized_xyz_from_serialized_xy(const std::vector<double> & coords, const double z)
{
    uint nv = coords.size()/2;
    std::vector<double> tmp(3*nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        tmp.at(3*vid  ) = coords.at(2*vid  );
        tmp.at(3*vid+1) = coords.at(2*vid+1);
        tmp.at(3*vid+2) = z;
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec2d> vec2d_from_serialized_xy(const std::vector<double> & coords)
{
    uint nv = coords.size()/2;
    std::vector<vec2d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec2d(coords.at(2*vid), coords.at(2*vid+1));
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec2d> vec2d_from_serialized_xyz(const std::vector<double> & coords)
{
    uint nv = coords.size()/3;
    std::vector<vec2d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec2d(coords.at(3*vid), coords.at(3*vid+1));
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec2d> vec2d_from_vec3d(const std::vector<vec3d>  & verts)
{
    uint nv = verts.size();
    std::vector<vec2d> tmp(nv);
    for(uint vid=0; vid<nv; ++vid) tmp.at(vid) = vec2d(verts.at(vid).x(), verts.at(vid).y());
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> serialized_xy_from_vec2d(const std::vector<vec2d> & verts)
{
    uint nv = verts.size();
    std::vector<double> tmp(2*nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        tmp.at(2*vid  ) = verts.at(vid).x();
        tmp.at(2*vid+1) = verts.at(vid).y();
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> serialized_xy_from_vec3d(const std::vector<vec3d> & verts)
{
    uint nv = verts.size();
    std::vector<double> tmp(2*nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        tmp.at(2*vid  ) = verts.at(vid).x();
        tmp.at(2*vid+1) = verts.at(vid).y();
    }
    return tmp;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<double> serialized_xy_from_serialized_xyz(const std::vector<double> & coords)
{
    uint nv = coords.size()/3;
    std::vector<double> tmp(2*nv);
    for(uint vid=0; vid<nv; ++vid)
    {
        tmp.at(2*vid  ) = coords.at(3*vid  );
        tmp.at(2*vid+1) = coords.at(3*vid+1);
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
