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
Eigen::SparseMatrix<double> gradient_matrix_srf(const AbstractPolygonMesh<M,V,E,P> & m)
{
    Eigen::SparseMatrix<double> G(m.num_polys()*3, m.num_verts());
    std::vector<Entry> entries;

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        double area = std::max(m.poly_area(pid), 1e-5);
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

            vec3d per_edge_normal = u_90 * u.length() + v_90 * v.length();
            per_edge_normal /= area;

            uint row = 3 * pid;
            entries.push_back(Entry(row, curr, per_edge_normal.x())); ++row;
            entries.push_back(Entry(row, curr, per_edge_normal.y())); ++row;
            entries.push_back(Entry(row, curr, per_edge_normal.z()));
        }
    }

    G.setFromTriplets(entries.begin(), entries.end());
    return G;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
Eigen::SparseMatrix<double> gradient_matrix_vol(const AbstractPolyhedralMesh<M,V,E,F,P> & m)
{
    Eigen::SparseMatrix<double> G(m.num_polys()*3, m.num_verts());
    std::vector<Entry> entries;

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        double vol3 = std::max(m.poly_volume(pid), 1e-5) * 3.0;

        for(uint vid : m.adj_p2v(pid))
        {
            vec3d per_vert_sum_over_f_normals(0,0,0);
            for(uint fid : m.adj_p2f(pid))
            {
                if (m.face_contains_vert(fid,vid))
                {
                    vec3d n = (m.poly_face_is_CCW(pid,fid)) ? m.face_data(fid).normal : -m.face_data(fid).normal;
                    per_vert_sum_over_f_normals += n * m.face_area(fid);
                }
            }
            per_vert_sum_over_f_normals /= vol3;
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

//template <>
//CINO_INLINE
//Eigen::SparseMatrix<double> gradient<Trimesh<>>(const Trimesh<> & m)
//{
//    Eigen::SparseMatrix<double> G(m.num_polys()*3, m.num_verts());

//    std::vector<Entry> entries;

//    for(uint fid=0; fid<m.num_polys(); ++fid)
//    {
//        uint i = m.poly_vert_id(fid, 0);
//        uint j = m.poly_vert_id(fid, 1);
//        uint k = m.poly_vert_id(fid, 2);

//        vec3d vi = m.vert(i);
//        vec3d vj = m.vert(j);
//        vec3d vk = m.vert(k);
//        vec3d ik = vi - vk;
//        vec3d ji = vj - vi;

//        vec3d  axis  = m.poly_data(fid).normal;
//        double angle = M_PI * 0.5;

//        rotate(ik, axis, angle);
//        rotate(ji, axis, angle);

//        double dblA = m.poly_mass(fid);

//        ik /= dblA;
//        ji /= dblA;

//        uint row = 3 * fid;
//        entries.push_back(Entry(row, i, -(ik.x() + ji.x())));
//        entries.push_back(Entry(row, j, ik.x()));
//        entries.push_back(Entry(row, k, ji.x()));

//        row++;
//        entries.push_back(Entry(row, i, -(ik.y() + ji.y())));
//        entries.push_back(Entry(row, j, ik.y()));
//        entries.push_back(Entry(row, k, ji.y()));

//        row++;
//        entries.push_back(Entry(row, i, -(ik.z() + ji.z())));
//        entries.push_back(Entry(row, j, ik.z()));
//        entries.push_back(Entry(row, k, ji.z()));
//    }

//    G.setFromTriplets(entries.begin(), entries.end());

//    return G;
//}


//template<>
//CINO_INLINE
//Eigen::SparseMatrix<double> gradient<Tetmesh<>>(const Tetmesh<> &m)
//{
//    Eigen::SparseMatrix<double> G(m.num_polys()*3, m.num_verts());

//    std::vector<Entry> entries;

//    for(uint pid=0; pid<m.num_polys(); ++pid)
//    {
//        uint A = m.poly_vert_id(pid, 0);
//        uint B = m.poly_vert_id(pid, 1);
//        uint C = m.poly_vert_id(pid, 2);
//        uint D = m.poly_vert_id(pid, 3);

//        double vol3 = std::max(m.poly_volume(pid), 1e-5) * 3.0;

//        vec3d n_ABC = m.poly_face_normal(pid,0) * m.poly_face_area(pid,0) / vol3;
//        vec3d n_ABD = m.poly_face_normal(pid,1) * m.poly_face_area(pid,1) / vol3;
//        vec3d n_ACD = m.poly_face_normal(pid,2) * m.poly_face_area(pid,2) / vol3;
//        vec3d n_BCD = m.poly_face_normal(pid,3) * m.poly_face_area(pid,3) / vol3;

//        uint row = 3 * pid;
//        entries.push_back(Entry(row, A, n_ABC.x() + n_ABD.x() + n_ACD.x()));
//        entries.push_back(Entry(row, B, n_ABC.x() + n_ABD.x() + n_BCD.x()));
//        entries.push_back(Entry(row, C, n_ABC.x() + n_ACD.x() + n_BCD.x()));
//        entries.push_back(Entry(row, D, n_BCD.x() + n_ABD.x() + n_ACD.x()));

//        row++;
//        entries.push_back(Entry(row, A, n_ABC.y() + n_ABD.y() + n_ACD.y()));
//        entries.push_back(Entry(row, B, n_ABC.y() + n_ABD.y() + n_BCD.y()));
//        entries.push_back(Entry(row, C, n_ABC.y() + n_ACD.y() + n_BCD.y()));
//        entries.push_back(Entry(row, D, n_BCD.y() + n_ABD.y() + n_ACD.y()));

//        row++;
//        entries.push_back(Entry(row, A, n_ABC.z() + n_ABD.z() + n_ACD.z()));
//        entries.push_back(Entry(row, B, n_ABC.z() + n_ABD.z() + n_BCD.z()));
//        entries.push_back(Entry(row, C, n_ABC.z() + n_ACD.z() + n_BCD.z()));
//        entries.push_back(Entry(row, D, n_BCD.z() + n_ABD.z() + n_ACD.z()));
//    }

//    G.setFromTriplets(entries.begin(), entries.end());

//    return G;
//}
