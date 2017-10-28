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
#include <cinolib/export_surface.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/meshes/quadmesh.h>
#include <cinolib/meshes/polygonmesh.h>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_surface(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                          AbstractPolygonMesh<M,V,E,F>      & srf)
{
    std::map<uint,uint> m2srf_vmap, srf2m_vmap;
    export_surface(m, srf, m2srf_vmap, srf2m_vmap);
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_surface(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                          AbstractPolygonMesh<M,V,E,F>      & srf,
                          std::map<uint,uint>               & m2srf_vmap,
                          std::map<uint,uint>               & srf2m_vmap)
{
    m2srf_vmap.clear();
    srf2m_vmap.clear();

    std::vector<vec3d>             verts;
    std::vector<std::vector<uint>> polys;

    uint fresh_id = 0;

    for(uint fid=0; fid<m.num_faces(); ++fid)
    {
        if (m.face_is_on_srf(fid))
        {
            std::vector<uint> p;
            for(uint off=0; off<m.verts_per_face(fid); ++off)
            {
                uint vid   = m.face_vert_id(fid,off);
                uint vsrf  = fresh_id++;

                auto query = m2srf_vmap.find(vid);
                if (query == m2srf_vmap.end())
                {
                    verts.push_back(m.vert(vid));

                    m2srf_vmap[vid] = vsrf;
                    srf2m_vmap[vsrf] = vid;
                }
                else
                {
                    vsrf = query->second;
                    --fresh_id;
                }

                p.push_back(vsrf);
            }
            polys.push_back(p);
        }
    }

    switch (m.mesh_type())
    {
        case TETMESH        : srf = Trimesh<M,V,E,F>(verts, polys);     break;
        case HEXMESH        : srf = Quadmesh<M,V,E,F>(verts, polys);    break;
        case POLYHEDRALMESH : srf = Polygonmesh<M,V,E,F>(verts, polys); break;
        default             : assert(false);
    }
}


}

