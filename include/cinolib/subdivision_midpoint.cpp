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
#include <cinolib/subdivision_midpoint.h>
#include <cinolib/sort_poly_vertices.h>
#include <map>

namespace cinolib
{

template<class M, class V, class E, class F, class P>
CINO_INLINE
void subdivision_midpoint(const AbstractPolyhedralMesh<M,V,E,F,P> & m_in,
                                AbstractPolyhedralMesh<M,V,E,F,P> & m_out)
{
    std::unordered_map<uint,uint> edge_verts;
    std::unordered_map<uint,uint> face_verts;
    std::unordered_map<uint,uint> poly_verts;
    subdivision_midpoint(m_in, m_out, edge_verts, face_verts, poly_verts);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void subdivision_midpoint(const AbstractPolyhedralMesh<M,V,E,F,P> & m_in,
                                AbstractPolyhedralMesh<M,V,E,F,P> & m_out,
                                std::unordered_map<uint,uint>     & edge_verts,
                                std::unordered_map<uint,uint>     & face_verts,
                                std::unordered_map<uint,uint>     & poly_verts)
{
    edge_verts.clear();
    face_verts.clear();
    poly_verts.clear();

    std::vector<vec3d>             verts = m_in.vector_verts();
    std::vector<std::vector<uint>> faces;
    std::vector<std::vector<uint>> polys;
    std::vector<std::vector<bool>> polys_winding;

    // 1) add one new vert for each edge/face/poly
    //
    for(uint eid=0; eid<m_in.num_edges(); ++eid) { edge_verts[eid] = verts.size(); verts.push_back(m_in.edge_sample_at(eid,0.5)); }
    for(uint fid=0; fid<m_in.num_faces(); ++fid) { face_verts[fid] = verts.size(); verts.push_back(m_in.face_centroid(fid)); }
    for(uint pid=0; pid<m_in.num_polys(); ++pid) { poly_verts[pid] = verts.size(); verts.push_back(m_in.poly_centroid(pid)); }

    // 2) for each pair (edge,poly), make a quad with:
    //      - poly centroid
    //      - incident face centroids
    //      - edge midpoint
    //
    std::map<ipair,uint> eid_pid_fmap;
    for(uint pid=0; pid<m_in.num_polys(); ++pid)
    for(uint eid : m_in.adj_p2e(pid))
    {
        std::vector<uint> inc_f = m_in.poly_e2f(pid,eid);
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
    for(uint fid = 0; fid<m_in.num_faces(); ++fid)
    for(uint vid : m_in.adj_f2v(fid))
    {
        std::vector<uint> e = m_in.face_v2e(fid,vid);
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
    for(uint pid = 0; pid<m_in.num_polys(); ++pid)
    {
        for(uint vid : m_in.adj_p2v(pid))
        {
            std::vector<uint> f;
            std::vector<bool> w;
            for(uint fid : m_in.poly_v2f(pid,vid))
            {
                f.push_back(vid_fid_fmap.at(std::make_pair(vid,fid)));                                
                // TODO: fix winding order
                w.push_back(true);
            }
            for(uint eid : m_in.poly_v2e(pid,vid))
            {
                f.push_back(eid_pid_fmap.at(std::make_pair(eid,pid)));
                // TODO: check on what side the vertex stays w.r.t. oriented plane to assign correct winding
                w.push_back(true);
            }
            polys.push_back(f);
            polys_winding.push_back(w);
        }
    }

    switch(m_in.mesh_type())
    {
        case TETMESH :
        case HEXMESH :
        {
            std::vector<std::vector<uint>> hexas;
            sort_poly_vertices_as_hexa(faces, polys, polys_winding, hexas);
            m_out = Hexmesh<M,V,E,F,P>(verts, hexas);
            break;
        }
        case POLYHEDRALMESH : m_out = Polyhedralmesh<M,V,E,F,P>(verts, faces, polys, polys_winding);
                              break;

        default : assert(false);
    }
}

}
