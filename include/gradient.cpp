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
#include <cinolib/gradient.h>

namespace cinolib
{

typedef Eigen::Triplet<double> Entry;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix(const AbstractPolygonMesh<M,V,E,P> & m)
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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix(const AbstractPolyhedralMesh<M,V,E,F,P> & m)
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

}
