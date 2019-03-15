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
#include <cinolib/vector_area_matrix.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
Eigen::SparseMatrix<double> vector_area_matrix(const AbstractPolygonMesh<M,V,E,P> & m)
{
    typedef Eigen::Triplet<double> Entry;
    std::vector<Entry> entries;

    uint vn      = m.num_verts();
    auto b_edges = m.get_boundary_edges();

    for(auto e : b_edges)
    {
        uint v0 = e.first;
        uint v1 = e.second;
        entries.push_back(Entry(v0+vn, v1,    -0.25));
        entries.push_back(Entry(v1,    v0+vn, -0.25));
        entries.push_back(Entry(v0,    v1+vn,  0.25));
        entries.push_back(Entry(v1+vn, v0,     0.25));
    }

    Eigen::SparseMatrix<double> A(vn*2, vn*2);
    A.setFromTriplets(entries.begin(), entries.end());

    return A;
}

}
