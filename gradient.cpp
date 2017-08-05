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

template <>
CINO_INLINE
Eigen::SparseMatrix<double> gradient<Trimesh<>>(const Trimesh<> & m)
{
    Eigen::SparseMatrix<double> G(m.num_faces()*3, m.num_verts());

    std::vector<Entry> entries;

    for(uint fid=0; fid<m.num_faces(); ++fid)
    {
        uint i = m.poly_vert_id(fid, 0);
        uint j = m.poly_vert_id(fid, 1);
        uint k = m.poly_vert_id(fid, 2);

        vec3d vi = m.vert(i);
        vec3d vj = m.vert(j);
        vec3d vk = m.vert(k);
        vec3d ik = vi - vk;
        vec3d ji = vj - vi;

        vec3d  axis  = m.poly_data(fid).normal;
        double angle = M_PI * 0.5;

        rotate(ik, axis, angle);
        rotate(ji, axis, angle);

        double dblA = m.poly_mass(fid);

        ik /= dblA;
        ji /= dblA;

        uint row = 3 * fid;
        entries.push_back(Entry(row, i, -(ik.x() + ji.x())));
        entries.push_back(Entry(row, j, ik.x()));
        entries.push_back(Entry(row, k, ji.x()));

        row++;
        entries.push_back(Entry(row, i, -(ik.y() + ji.y())));
        entries.push_back(Entry(row, j, ik.y()));
        entries.push_back(Entry(row, k, ji.y()));

        row++;
        entries.push_back(Entry(row, i, -(ik.z() + ji.z())));
        entries.push_back(Entry(row, j, ik.z()));
        entries.push_back(Entry(row, k, ji.z()));
    }

    G.setFromTriplets(entries.begin(), entries.end());

    return G;
}


template<>
CINO_INLINE
Eigen::SparseMatrix<double> gradient<Tetmesh<>>(const Tetmesh<> &m)
{
    Eigen::SparseMatrix<double> G(m.num_cells()*3, m.num_verts());

    std::vector<Entry> entries;

    for(uint cid=0; cid<m.num_cells(); ++cid)
    {
        int A = m.cell_vert_id(cid, 0);
        int B = m.cell_vert_id(cid, 1);
        int C = m.cell_vert_id(cid, 2);
        int D = m.cell_vert_id(cid, 3);

        double vol3 = std::max(m.cell_volume(cid), 1e-5) * 3.0;

        vec3d n_ABC = m.cell_face_normal(cid,0) * m.cell_face_area(cid,0) / vol3;
        vec3d n_ABD = m.cell_face_normal(cid,1) * m.cell_face_area(cid,1) / vol3;
        vec3d n_ACD = m.cell_face_normal(cid,2) * m.cell_face_area(cid,2) / vol3;
        vec3d n_BCD = m.cell_face_normal(cid,3) * m.cell_face_area(cid,3) / vol3;

        uint row = 3 * cid;
        entries.push_back(Entry(row, A, n_ABC.x() + n_ABD.x() + n_ACD.x()));
        entries.push_back(Entry(row, B, n_ABC.x() + n_ABD.x() + n_BCD.x()));
        entries.push_back(Entry(row, C, n_ABC.x() + n_ACD.x() + n_BCD.x()));
        entries.push_back(Entry(row, D, n_BCD.x() + n_ABD.x() + n_ACD.x()));

        row++;
        entries.push_back(Entry(row, A, n_ABC.y() + n_ABD.y() + n_ACD.y()));
        entries.push_back(Entry(row, B, n_ABC.y() + n_ABD.y() + n_BCD.y()));
        entries.push_back(Entry(row, C, n_ABC.y() + n_ACD.y() + n_BCD.y()));
        entries.push_back(Entry(row, D, n_BCD.y() + n_ABD.y() + n_ACD.y()));

        row++;
        entries.push_back(Entry(row, A, n_ABC.z() + n_ABD.z() + n_ACD.z()));
        entries.push_back(Entry(row, B, n_ABC.z() + n_ABD.z() + n_BCD.z()));
        entries.push_back(Entry(row, C, n_ABC.z() + n_ACD.z() + n_BCD.z()));
        entries.push_back(Entry(row, D, n_BCD.z() + n_ABD.z() + n_ACD.z()));
    }

    G.setFromTriplets(entries.begin(), entries.end());

    return G;
}

}
