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
#include <cinolib/ARAP_2D_map.h>
#include <cinolib/parallel_for.h>
#include <cinolib/tangent_space.h>
#include <cinolib/lscm.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void ARAP_2D_mapping(const Trimesh<M,V,E,P> & m, ARAP_2D_map_data & data)
{
    uint bc = m.num_verts()-1;

    auto init = [&]()
    {
        data.init = false; // don't init next time

        data.uv_ref.resize(m.num_polys()*3);
        data.uv_loc.resize(m.num_polys()*3);
        data.uv.resize(m.num_verts());

        // compute reference local uv coords
        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            uint off = pid*3;
            tangent_space_2d_coords(m.poly_vert(pid,0),
                                    m.poly_vert(pid,1),
                                    m.poly_vert(pid,2),
                                    data.uv_ref.at(off),
                                    data.uv_ref.at(off+1),
                                    data.uv_ref.at(off+2));
        }

        // warm start current global uv's with some mapping
        ScalarField f = LSCM(m);
        uint nv = m.num_verts();
        for(uint vid=0; vid<nv; ++vid)
        {
            data.uv.at(vid) = vec2d(f[vid], f[vid+nv]);
        }

        // per edge weights
        data.w.resize(m.num_edges());
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            data.w.at(eid) = m.edge_cotangent_weight(eid);
        }

        // Pre-factorize Laplacian matrix.
        // The Laplacian matrix is VxV, and has rank V-1. For simplicity,
        // I am fixing the uv coords of the last vertex to (0,0). This
        // allows to work on a full rank matrix, and also to keep a direct
        // correspondence between rows/cols in the matrix and vertex ids
        typedef Eigen::Triplet<double> Entry;
        std::vector<Entry> entries;
        for(uint vid=0; vid<m.num_verts()-1; ++vid)
        {
            for(uint nbr : m.adj_v2v(vid))
            {
                int eid = m.edge_id(vid,nbr);
                assert(eid>=0);
                entries.push_back(Entry(vid, vid,  data.w.at(eid)));
                if(nbr!=bc) entries.push_back(Entry(vid, nbr, -data.w.at(eid)));
            }
        }
        Eigen::SparseMatrix<double> A(m.num_verts()-1, m.num_verts()-1);
        A.setFromTriplets(entries.begin(), entries.end());
        data.cache.derived().compute(A);
    };

    auto local_step = [&]()
    {
        PARALLEL_FOR(0, m.num_polys(), 1000, [&](uint pid)
        {
            // per triangle covariance matrix
            uint off = 3*pid;
            mat2d cov = mat2d::ZERO();
            for(int i=0; i<3; ++i)
            {
                uint v0  = m.poly_vert_id(pid,i);
                uint v1  = m.poly_vert_id(pid,(i+1)%3);
                int  eid = m.edge_id(v0,v1);
                assert(eid>=0);
                vec2d e_cur = data.uv.at(v0)    - data.uv.at(v1);
                vec2d e_ref = data.uv_ref.at(off+i) - data.uv_ref.at(off+((i+1)%3));
                cov += data.w.at(eid) * (e_cur * e_ref.transpose());
            }

            // find closest rotation and store rotated point
            mat2d u,v;
            vec2d  s;
            cov.SSVD(u,s,v);
            mat2d rot = u*v.transpose();
            data.uv_loc.at(off  ) = rot * data.uv_ref.at(off  );
            data.uv_loc.at(off+1) = rot * data.uv_ref.at(off+1);
            data.uv_loc.at(off+2) = rot * data.uv_ref.at(off+2);
        });
    };

    auto global_step = [&]()
    {
        Eigen::VectorXd rhs_u = Eigen::VectorXd::Zero(m.num_verts()-1);
        Eigen::VectorXd rhs_v = Eigen::VectorXd::Zero(m.num_verts()-1);
        for(uint vid=0; vid<m.num_verts()-1; ++vid)
        {
            for(uint nbr : m.adj_v2v(vid))
            {
                int eid = m.edge_id(vid,nbr);
                for(uint pid : m.adj_e2p(eid))
                {
                    uint i = m.poly_vert_offset(pid,vid);
                    uint j = m.poly_vert_offset(pid,nbr);
                    assert(i>=0 && i<3);
                    assert(j>=0 && j<3);
                    vec2d Re = data.uv_loc.at(pid*3+i) - data.uv_loc.at(pid*3+j);
                    rhs_u[vid] += data.w.at(eid) * Re[0];
                    rhs_v[vid] += data.w.at(eid) * Re[1];
                }
            }
        }
        Eigen::VectorXd u = data.cache.solve(rhs_u);
        Eigen::VectorXd v = data.cache.solve(rhs_v);
        for(uint vid=0; vid<m.num_verts()-1; ++vid)
        {
            data.uv[vid] = vec2d(u[vid],v[vid]);
        }
        data.uv[bc] = vec2d(0,0); // last vertex
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    if(data.init) init();

    for(uint i=0; i<data.n_iters; ++i)
    {
        local_step();
        global_step();
    }
}

}
