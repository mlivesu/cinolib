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
#include <cinolib/laplacian.h>
#include <cinolib/symbols.h>
#include <Eigen/Sparse>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<Eigen::Triplet<double>> laplacian_matrix_entries(const AbstractMesh<M,V,E,P> & m, const int mode)
{
    std::vector<Entry> entries;
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        std::vector<std::pair<uint,double>> wgts;
        m.vert_weights(vid, mode, wgts);

        double sum = 0.0;
        for(auto item : wgts)
        {
            entries.push_back(Entry(vid, item.first, item.second));
            sum -= item.second;
        }
        if (sum == 0.0)
        {
            std::cerr << "WARNING: null row in the matrix! (disconnected vertex? I put 1 in the diagonal)" << std::endl;
            sum = 1.0;
        }
        entries.push_back(Entry(vid, vid, sum));
    }

    return entries;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<Eigen::Triplet<double>> laplacian_3d_matrix_entries(const AbstractMesh<M,V,E,P> & m, const int mode)
{
    std::vector<Entry> entries;

    uint nv     = m.num_verts();
    uint base_x = nv * 0;
    uint base_y = nv * 1;
    uint base_z = nv * 2;

    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        std::vector<std::pair<uint,double>> wgts;
        m.vert_weights(vid, mode, wgts);
        double sum = 0.0;
        for(auto item : wgts)
        {
            entries.push_back(Entry(base_x + vid, base_x + item.first, item.second));
            entries.push_back(Entry(base_y + vid, base_y + item.first, item.second));
            entries.push_back(Entry(base_z + vid, base_z + item.first, item.second));
            sum -= item.second;
        }
        if (sum == 0.0)
        {
            std::cerr << "WARNING: null row in the matrix! (disconnected vertex? I put 1 in the diagonal)" << std::
                         endl;
            sum = 1.0;
        }
        entries.push_back(Entry(base_x + vid, base_x + vid, sum));
        entries.push_back(Entry(base_y + vid, base_y + vid, sum));
        entries.push_back(Entry(base_z + vid, base_z + vid, sum));
    }

    return entries;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::SparseMatrix<double> laplacian(const AbstractMesh<M,V,E,P> & m, const int mode)
{
    std::vector<Entry> entries = laplacian_matrix_entries(m, mode);

    Eigen::SparseMatrix<double> L(m.num_verts(), m.num_verts());
    L.setFromTriplets(entries.begin(), entries.end());

    return L;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::SparseMatrix<double> laplacian_3d(const AbstractMesh<M,V,E,P> & m, const int mode)
{
    std::vector<Entry> entries = laplacian_3d_matrix_entries(m, mode);

    Eigen::SparseMatrix<double> L(m.num_verts() * 3, m.num_verts() * 3);
    L.setFromTriplets(entries.begin(), entries.end());

    return L;
}

}
