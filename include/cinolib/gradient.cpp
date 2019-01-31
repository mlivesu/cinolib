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
*                                                                               *
*                                                                               *
*     Claudio Mancinelli (claudio.f.mancinelli@gmail.com)                       *
*                                                                               *
*     University of Genoa                                                       *
*     Via Dodecaneso, 35                                                        *
*     16146 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/gradient.h>

namespace cinolib
{

typedef Eigen::Triplet<double> Entry;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix(const AbstractPolygonMesh<M,V,E,P> & m, const bool per_poly)
{
    if(per_poly)
    {
        Eigen::SparseMatrix<double> G(m.num_polys()*3, m.num_verts());
        std::vector<Entry> entries;

        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            double area = std::max(m.poly_area(pid), 1e-5) * 2.0; // (2 is the average term : two verts for each edge)
            vec3d n     = m.poly_data(pid).normal;

            for(uint off=0; off<m.verts_per_poly(pid); ++off)
            {
                uint  prev = m.poly_vert_id(pid,off);
                uint  curr = m.poly_vert_id(pid,(off+1)%m.verts_per_poly(pid));
                uint  next = m.poly_vert_id(pid,(off+2)%m.verts_per_poly(pid));
                vec3d u    = m.vert(next) - m.vert(curr);
                vec3d v    = m.vert(curr) - m.vert(prev);
                vec3d u_90 = u.cross(n); u_90.normalize();
                vec3d v_90 = v.cross(n); v_90.normalize();

                vec3d per_vert_sum_over_edge_normals = u_90 * u.length() + v_90 * v.length();
                per_vert_sum_over_edge_normals /= area;

                uint row = 3 * pid;
                entries.push_back(Entry(row, curr, per_vert_sum_over_edge_normals.x())); ++row;
                entries.push_back(Entry(row, curr, per_vert_sum_over_edge_normals.y())); ++row;
                entries.push_back(Entry(row, curr, per_vert_sum_over_edge_normals.z()));
            }
        }

        G.setFromTriplets(entries.begin(), entries.end());
        return G;
    }
    else // per vertex
    {
        Eigen::SparseMatrix<double> G(m.num_verts()*3, m.num_verts());
        std::vector<Entry> entries;

        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            std::vector<std::pair<uint,vec3d>> vert_contr;
            double area=0.f;
            for(uint pid : m.adj_v2p(vid))
            {
                area   += std::max(m.poly_area(pid), 1e-5) * 2.0;
                vec3d n = m.poly_data(pid).normal;

                for(uint off=0; off<m.verts_per_poly(pid); ++off)
                {
                    uint  prev = m.poly_vert_id(pid,off);
                    uint  curr = m.poly_vert_id(pid,(off+1)%m.verts_per_poly(pid));
                    uint  next = m.poly_vert_id(pid,(off+2)%m.verts_per_poly(pid));
                    vec3d u    = m.vert(next) - m.vert(curr);
                    vec3d v    = m.vert(curr) - m.vert(prev);
                    vec3d u_90 = u.cross(n); u_90.normalize();
                    vec3d v_90 = v.cross(n); v_90.normalize();

                    // note: Eigen::setFromTriplets will take care of summing contributs w.r.t. multiple polys
                    vert_contr.push_back(std::make_pair(curr, u_90*u.length()+v_90*v.length()));
                }
            }
            uint row = vid * 3;
            for(auto c : vert_contr)
            {
                entries.push_back(Entry(row  , c.first, c.second.x()/area));
                entries.push_back(Entry(row+1, c.first, c.second.y()/area));
                entries.push_back(Entry(row+2, c.first, c.second.z()/area));
            }
        }

        G.setFromTriplets(entries.begin(), entries.end());
        return G;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix(const AbstractPolyhedralMesh<M,V,E,F,P> & m, const bool per_poly)
{
    if(per_poly)
    {
        Eigen::SparseMatrix<double> G(m.num_polys()*3, m.num_verts());
        std::vector<Entry> entries;

        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            double vol = std::max(m.poly_volume(pid), 1e-5);

            for(uint vid : m.adj_p2v(pid))
            {
                vec3d per_vert_sum_over_f_normals(0,0,0);
                for(uint fid : m.adj_p2f(pid))
                {
                    if (m.face_contains_vert(fid,vid))
                    {
                        vec3d  n   = m.poly_face_normal(pid,fid);
                        double a   = m.face_area(fid);
                        double avg = static_cast<double>(m.verts_per_face(fid));
                        per_vert_sum_over_f_normals += (n*a)/avg;
                    }
                }
                per_vert_sum_over_f_normals /= vol;
                uint row = 3 * pid;
                entries.push_back(Entry(row, vid, per_vert_sum_over_f_normals.x())); ++row;
                entries.push_back(Entry(row, vid, per_vert_sum_over_f_normals.y())); ++row;
                entries.push_back(Entry(row, vid, per_vert_sum_over_f_normals.z()));
            }
        }

        G.setFromTriplets(entries.begin(), entries.end());
        return G;
    }
    else // per vert
    {
        Eigen::SparseMatrix<double> G(m.num_polys()*3, m.num_verts());
        std::vector<Entry> entries;

        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            double vol = std::max(m.poly_volume(pid), 1e-5);

            for(uint vid : m.adj_p2v(pid))
            {
                vec3d per_vert_sum_over_f_normals(0,0,0);
                for(uint fid : m.adj_p2f(pid))
                {
                    if (m.face_contains_vert(fid,vid))
                    {
                        vec3d  n   = m.poly_face_normal(pid,fid);
                        double a   = m.face_area(fid);
                        double avg = static_cast<double>(m.verts_per_face(fid));
                        per_vert_sum_over_f_normals += (n*a)/avg;
                    }
                }
                per_vert_sum_over_f_normals /= vol;
                uint row = 3 * pid;
                entries.push_back(Entry(row, vid, per_vert_sum_over_f_normals.x())); ++row;
                entries.push_back(Entry(row, vid, per_vert_sum_over_f_normals.y())); ++row;
                entries.push_back(Entry(row, vid, per_vert_sum_over_f_normals.z()));
            }
        }
        G.setFromTriplets(entries.begin(), entries.end());

        Eigen::SparseMatrix<double> A(m.num_verts()*3, m.num_polys()*3);
        entries.clear();

        for(uint vid=0;vid<m.num_verts();++vid)
        {
            double total_volume=0;
            for(uint pid : m.adj_v2p(vid))
            {
                total_volume += m.poly_volume(pid);
            }
            uint row = 3*vid;
            for(uint pid : m.adj_v2p(vid))
            {
                uint col=3*pid;
                entries.push_back(Entry(row,  col,   m.poly_volume(pid)/total_volume));
                entries.push_back(Entry(row+1,col+1, m.poly_volume(pid)/total_volume));
                entries.push_back(Entry(row+2,col+2, m.poly_volume(pid)/total_volume));
            }
        }
        A.setFromTriplets(entries.begin(), entries.end());
        return A*G;
    }
}

}
