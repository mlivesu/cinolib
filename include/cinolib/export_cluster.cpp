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
#include <cinolib/export_cluster.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P>  & m,
                    const int                             label,
                          AbstractPolygonMesh<M,V,E,P>  & subm,
                          std::map<uint,uint>           & m2subm_vmap,
                          std::map<uint,uint>           & subm2m_vmap)
{
    m2subm_vmap.clear();
    subm2m_vmap.clear();

    std::vector<vec3d>             verts;
    std::vector<std::vector<uint>> polys;

    uint fresh_vid = 0;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        if(m.poly_data(pid).label == label)
        {
            std::vector<uint> p;
            for(uint off=0; off<m.verts_per_poly(pid); ++off)
            {
                uint vid  = m.poly_vert_id(pid,off);
                uint vnew = fresh_vid++;

                auto query = m2subm_vmap.find(vid);
                if (query == m2subm_vmap.end())
                {
                    verts.push_back(m.vert(vid));

                    m2subm_vmap[vid] = vnew;
                    subm2m_vmap[vnew] = vid;
                }
                else
                {
                    vnew = query->second;
                    --fresh_vid;
                }

                p.push_back(vnew);
            }
            polys.push_back(p);
        }
    }

    switch (m.mesh_type())
    {
        case TRIMESH     : subm = Trimesh<M,V,E,P>(verts, polys);     break;
        case QUADMESH    : subm = Quadmesh<M,V,E,P>(verts, polys);    break;
        case POLYGONMESH : subm = Polygonmesh<M,V,E,P>(verts, polys); break;
        default          : assert(false);
    }

    // Bugfix (8 Mar 2018): this is necessary because in any code similar to:
    //
    // DrawablePolygonmesh<M,V,E,P> m;
    // func(m);
    //
    // with:
    //
    // void func(DrawablePolygonmesh<M,V,E,P> & m)
    // {
    //    m = Polygonmesh<M,V,E,P>(...);
    // }
    //
    // triangulated_polys is not copied in the drawable mesh, and any call to this method
    // would throw an exception std::out_of_range
    //
    if (m.mesh_type()==POLYGONMESH)
    {
        dynamic_cast<Polygonmesh<M,V,E,P>*>(&subm)->update_poly_tessellation();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P> & m,
                    const int                            label,
                          AbstractPolygonMesh<M,V,E,P> & subm)
{
    std::map<uint,uint> m2subm_vmap, subm2m_vmap;
    export_cluster(m, label, subm, m2subm_vmap, subm2m_vmap);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P>  & m,
                    const int                                  label,
                          AbstractPolyhedralMesh<M,V,E,F,P>  & subm,
                          std::map<uint,uint>                & m2subm_vmap,
                          std::map<uint,uint>                & subm2m_vmap)
{
    m2subm_vmap.clear();
    subm2m_vmap.clear();

    std::vector<vec3d>             verts;
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    std::vector<std::vector<bool>> polys_face_winding;

    // select the faces incident to polyhedra with legal label
    std::vector<uint> f_list;
    for(uint fid=0; fid<m.num_faces(); ++fid)
    {
        bool has_label = false;
        for(uint pid : m.adj_f2p(fid))
        {
            if (m.poly_data(pid).label == label) has_label = true;
        }
        if (has_label) f_list.push_back(fid);
    }

    std::map<uint,uint> fmap;
    uint fresh_vid = 0;
    for(uint fresh_fid=0; fresh_fid<f_list.size(); ++fresh_fid)
    {
        uint fid = f_list.at(fresh_fid);

        std::vector<uint> f;
        for(uint vid : m.adj_f2v(fid)) // note: ordered list!
        {
            uint vnew  = fresh_vid++;
            auto query = m2subm_vmap.find(vid);
            if (query == m2subm_vmap.end())
            {
                verts.push_back(m.vert(vid));
                m2subm_vmap[vid]  = vnew;
                subm2m_vmap[vnew] = vid;
            }
            else
            {
                vnew = query->second;
                --fresh_vid;
            }
            f.push_back(vnew);
        }
        faces.push_back(f);
        fmap[fid] = fresh_fid;        
    }

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        if(m.poly_data(pid).label == label)
        {
            std::vector<uint> p;
            std::vector<bool> w;
            for(uint fid : m.adj_p2f(pid))
            {
                auto query = fmap.find(fid);
                assert(query != fmap.end());
                p.push_back(query->second);
                w.push_back(m.poly_face_is_CW(pid,fid));
            }
            polys.push_back(p);
            polys_face_winding.push_back(w);
        }
    }

    switch (m.mesh_type())
    {
        case TETMESH        : subm = Tetmesh<M,V,E,F,P>(verts, faces, polys, polys_face_winding);        break;
        case HEXMESH        : subm = Hexmesh<M,V,E,F,P>(verts, faces, polys, polys_face_winding);        break;
        case POLYHEDRALMESH : subm = Polyhedralmesh<M,V,E,F,P>(verts, faces, polys, polys_face_winding); break;
        default             : assert(false);
    }

    // Bugfix (8 Mar 2018): this is necessary because in any code similar to:
    //
    // DrawablePolyhedralmesh<M,V,E,F,P> m;
    // func(m);
    //
    // with:
    //
    // void func(AbstractPolyhedralMesh<M,V,E,F,P> & m)
    // {
    //    m = Polyhedralmesh<M,V,E,F,P>(...);
    // }
    //
    // triangulated_faces is not copied in the drawable mesh, and any call to this method
    // would throw an exception std::out_of_range
    //
    if (m.mesh_type()==POLYHEDRALMESH)
    {
        dynamic_cast<Polyhedralmesh<M,V,E,F,P>*>(&subm)->update_face_tessellation();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                    const int                                 label,
                          AbstractPolyhedralMesh<M,V,E,F,P> & subm)
{
    std::map<uint,uint> m2subm_vmap, subm2m_vmap;
    export_cluster(m, label, subm, m2subm_vmap, subm2m_vmap);
}

}

