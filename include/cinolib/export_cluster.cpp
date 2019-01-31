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
#include <cinolib/export_cluster.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P>  & m,
                    const std::unordered_set<int>       & labels,
                          AbstractPolygonMesh<M,V,E,P>  & subm,
                          std::unordered_map<uint,uint> & m2subm_vmap,
                          std::unordered_map<uint,uint> & subm2m_vmap)
{
    m2subm_vmap.clear();
    subm2m_vmap.clear();

    std::vector<vec3d>             verts;
    std::vector<std::vector<uint>> polys;

    uint fresh_vid = 0;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        if(CONTAINS(labels,m.poly_data(pid).label))
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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P> & m,
                    const std::unordered_set<int>      & labels,
                          AbstractPolygonMesh<M,V,E,P> & subm)
{
    std::unordered_map<uint,uint> m2subm_vmap, subm2m_vmap;
    export_cluster(m, labels, subm, m2subm_vmap, subm2m_vmap);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P>  & m,
                    const int                             label,
                          AbstractPolygonMesh<M,V,E,P>  & subm,
                          std::unordered_map<uint,uint> & m2subm_vmap,
                          std::unordered_map<uint,uint> & subm2m_vmap)
{
    std::unordered_set<int> s;
    s.insert(label);
    export_cluster(m, s, subm, m2subm_vmap, subm2m_vmap);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void export_cluster(const AbstractPolygonMesh<M,V,E,P> & m,
                    const int                            label,
                          AbstractPolygonMesh<M,V,E,P> & subm)
{
    std::unordered_set<int> s;
    s.insert(label);
    std::unordered_map<uint,uint> m2subm_vmap, subm2m_vmap;
    export_cluster(m, s, subm, m2subm_vmap, subm2m_vmap);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P>  & m,
                    const std::unordered_set<int>            & labels,
                          AbstractPolyhedralMesh<M,V,E,F,P>  & subm,
                          std::unordered_map<uint,uint>      & m2subm_vmap,
                          std::unordered_map<uint,uint>      & subm2m_vmap)
{
    m2subm_vmap.clear();
    subm2m_vmap.clear();

    std::vector<vec3d>             verts;
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    std::vector<std::vector<bool>> polys_face_winding;

    if (m.mesh_type()!=POLYHEDRALMESH)
    {
        // for TETMESH and HEXMESH, define polyhedra as list of 4 or 8 vertices
        // connectivity is instrinsically defined in vertex order

        uint fresh_vid = 0;
        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            if(CONTAINS(labels, m.poly_data(pid).label))
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
            case TETMESH : subm = Tetmesh<M,V,E,F,P>(verts, polys); break;
            case HEXMESH : subm = Hexmesh<M,V,E,F,P>(verts, polys); break;
            default      : assert(false);
        }
    }
    else
    {
        // for POLYHEDRALMESH, define polyhedra as list vertices,
        // faces and polyhedra (with per face winding)

        // select the faces incident to polyhedra with legal label
        std::vector<uint> f_list;
        for(uint fid=0; fid<m.num_faces(); ++fid)
        {
            bool has_label = false;
            for(uint pid : m.adj_f2p(fid))
            {
                if(CONTAINS(labels,m.poly_data(pid).label)) has_label = true;
            }
            if (has_label) f_list.push_back(fid);
        }

        std::unordered_map<uint,uint> fmap;
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
            if(CONTAINS(labels, m.poly_data(pid).label))
            {
                std::vector<uint> p;
                std::vector<bool> w;
                for(uint fid : m.adj_p2f(pid))
                {
                    auto query = fmap.find(fid);
                    assert(query != fmap.end());
                    p.push_back(query->second);
                    w.push_back(m.poly_face_is_CCW(pid,fid));
                }
                polys.push_back(p);
                polys_face_winding.push_back(w);
            }
        }

        subm = Polyhedralmesh<M,V,E,F,P>(verts, faces, polys, polys_face_winding);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                    const std::unordered_set<int>           & labels,
                          AbstractPolyhedralMesh<M,V,E,F,P> & subm)
{
    std::unordered_map<uint,uint> m2subm_vmap, subm2m_vmap;
    export_cluster(m, labels, subm, m2subm_vmap, subm2m_vmap);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P>  & m,
                    const int                                  label,
                          AbstractPolyhedralMesh<M,V,E,F,P>  & subm,
                          std::unordered_map<uint,uint>      & m2subm_vmap,
                          std::unordered_map<uint,uint>      & subm2m_vmap)
{
    std::unordered_set<int> s;
    s.insert(label);
    export_cluster(m, s, subm, m2subm_vmap, subm2m_vmap);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void export_cluster(const AbstractPolyhedralMesh<M,V,E,F,P> & m,
                    const int                                 label,
                          AbstractPolyhedralMesh<M,V,E,F,P> & subm)
{
    std::unordered_set<int> s;
    s.insert(label);
    std::unordered_map<uint,uint> m2subm_vmap, subm2m_vmap;
    export_cluster(m, s, subm, m2subm_vmap, subm2m_vmap);
}

}

