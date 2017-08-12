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
#ifndef CINO_LAPLACIAN_TEMPLATED_H
#define CINO_LAPLACIAN_TEMPLATED_H

#include <cinolib/cinolib.h>
#include <cinolib/cotangent.h>
#include <eigen3/Eigen/Sparse>

namespace cinolib
{

typedef Eigen::Triplet<double> Entry;


template<typename Mesh>
CINO_INLINE
void uniform_weights(const Mesh          & m,
                     const uint            vid,
                     std::vector<uint>   & nbrs,
                     std::vector<double> & wgts)
{
    assert(nbrs.empty());
    assert(wgts.empty());
    nbrs = m.adj_v2v(vid);
    double w = 1.0; // / (double)nbrs.size(); // <= WARNING: makes the matrix non-symmetric!!!!!
    for(uint i=0; i<nbrs.size(); ++i)
    {
        wgts.push_back(w);
    }
}


template<typename Mesh>
CINO_INLINE
std::vector< Eigen::Triplet<double> > laplacian_matrix_entries(const Mesh & m, const int mode)
{
    std::vector<Entry> entries;

    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        std::vector<uint>   nbrs;
        std::vector<double> wgts;

        if (mode & UNIFORM)   uniform_weights<Mesh>  (m, vid, nbrs, wgts); else
        if (mode & COTANGENT) cotangent_weights<Mesh>(m, vid, nbrs, wgts); else
        assert(false);

        double sum = 0.0;
        for(uint i=0; i<nbrs.size(); ++i)
        {
            entries.push_back(Entry(vid, nbrs[i], wgts[i]));
            sum -= wgts[i];
        }
        if (sum == 0.0)
        {
            std::cerr << "WARNING: null row in the matrix! (disconnected vertex? I put 1 in the diagonal)" << endl;
            sum = 1.0;
        }
        entries.push_back(Entry(vid, vid, sum));
    }

    return entries;
}

template<typename Mesh>
CINO_INLINE
std::vector< Eigen::Triplet<double> > laplacian_matrix_entries_xyz(const Mesh & m, const int mode)
{
    std::vector<Entry> entries;

    uint nv     = m.num_verts();
    uint base_x = nv * 0;
    uint base_y = nv * 1;
    uint base_z = nv * 2;


    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        std::vector<uint>   nbrs;
        std::vector<double> wgts;

        if (mode & UNIFORM)   uniform_weights<Mesh>  (m, vid, nbrs, wgts); else
        if (mode & COTANGENT) cotangent_weights<Mesh>(m, vid, nbrs, wgts); else
        assert(false);

        double sum = 0.0;
        for(uint i=0; i<nbrs.size(); ++i)
        {
            entries.push_back(Entry(base_x + vid, base_x + nbrs[i], wgts[i]));
            entries.push_back(Entry(base_y + vid, base_y + nbrs[i], wgts[i]));
            entries.push_back(Entry(base_z + vid, base_z + nbrs[i], wgts[i]));
            sum -= wgts[i];
        }
        entries.push_back(Entry(base_x + vid, base_x + vid, sum));
        entries.push_back(Entry(base_y + vid, base_y + vid, sum));
        entries.push_back(Entry(base_z + vid, base_z + vid, sum));
    }

    return entries;
}


template<typename Mesh>
CINO_INLINE
Eigen::SparseMatrix<double> laplacian(const Mesh & m, const int mode)
{
    std::vector<Entry> entries = laplacian_matrix_entries<Mesh>(m, mode);

    Eigen::SparseMatrix<double> L(m.num_verts(), m.num_verts());
    L.setFromTriplets(entries.begin(), entries.end());

    return L;
}


template<typename Mesh>
CINO_INLINE
Eigen::SparseMatrix<double> laplacian_xyz(const Mesh & m, const int mode)
{
    std::vector<Entry> entries = laplacian_matrix_entries_xyz<Mesh>(m, mode);

    Eigen::SparseMatrix<double> L(m.num_vertices() * 3, m.num_vertices() * 3);
    L.setFromTriplets(entries.begin(), entries.end());

    return L;
}

}

#endif // CINO_LAPLACIAN_TEMPLATED_H
