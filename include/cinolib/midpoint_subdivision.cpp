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
#include <cinolib/midpoint_subdivision.h>
#include <map>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void midpoint_subdivision(AbstractPolyhedralMesh<M,V,E,F,P> & m)
{
    // 1) add one new vert for each edge/face/poly
    //
    std::map<uint,uint> edge_verts;
    std::map<uint,uint> face_verts;
    std::map<uint,uint> poly_verts;
    for(uint eid=0; eid<m.num_edges(); ++eid) edge_verts[eid] = m.vert_add(m.edge_sample_at(eid,0.5));
    for(uint fid=0; fid<m.num_faces(); ++fid) face_verts[fid] = m.vert_add(m.face_centroid(fid));
    for(uint pid=0; pid<m.num_polys(); ++pid) poly_verts[pid] = m.vert_add(m.poly_centroid(pid));

    std::vector<vec3d>             verts = m.vector_verts();
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    std::vector<std::vector<bool>> polys_winding;

    // 2) for each pair (edge,poly), make a quad with:
    //      - poly centroid
    //      - incident face centroids
    //      - edge midpoint
    //
    std::map<ipair,uint> eid_pid_fmap;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    for(uint eid : m.adj_p2e(pid))
    {
        std::vector<uint> inc_f = m.poly_e2f(pid,eid);
        std::vector<uint> f;
        f.push_back(poly_verts.at(pid));
        f.push_back(face_verts.at(inc_f.front()));
        f.push_back(edge_verts.at(eid));
        f.push_back(face_verts.at(inc_f.back()));
        eid_pid_fmap[std::make_pair(eid,pid)] = faces.size();
        faces.push_back(f);
    }

    // 3) for each pair (vert,face), make a quad with:
    //      - poly centroid
    //      - incident face centroids
    //      - edge midpoint
    //
    std::map<ipair,uint> vid_fid_fmap;
    for(uint fid = 0; fid<m.num_faces(); ++fid)
    for(uint vid : m.adj_f2v(fid))
    {
        std::vector<uint> e = m.face_v2e(fid,vid);
        std::vector<uint> f;
        f.push_back(face_verts.at(fid));
        f.push_back(edge_verts.at(e.front()));
        f.push_back(vid);
        f.push_back(edge_verts.at(e.back()));
        vid_fid_fmap[std::make_pair(vid,fid)] = faces.size();
        faces.push_back(f);
    }

    // 4) for each vertex of each poly, make a new polyhedron
    //    using the faces created at steps (2) and (3)
    //
    for(uint pid = 0; pid<m.num_polys(); ++pid)
    {
        for(uint vid : m.adj_p2v(pid))
        {
            std::vector<uint> f;
            std::vector<bool> w;
            for(uint fid : m.poly_v2f(pid,vid))
            {
                f.push_back(vid_fid_fmap.at(std::make_pair(vid,fid)));
                w.push_back(true);
            }
            for(uint eid : m.poly_v2e(pid,vid))
            {
                f.push_back(eid_pid_fmap.at(std::make_pair(eid,pid)));
                w.push_back(true);
            }
            polys.push_back(f);
            polys_winding.push_back(w);
        }
    }

    m = Polyhedralmesh<M,V,E,F,P>(verts, faces, polys, polys_winding);
}

}
