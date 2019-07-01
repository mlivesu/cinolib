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
    std::unordered_map<uint,uint> m2srf_vmap, srf2m_vmap;
    export_surface(m, srf, m2srf_vmap, srf2m_vmap);
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_surface(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                          AbstractPolygonMesh<M,V,E,F>      & srf,
                          std::unordered_map<uint,uint>     & m2srf_vmap,
                          std::unordered_map<uint,uint>     & srf2m_vmap)
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

