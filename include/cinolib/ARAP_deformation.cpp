/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2022: Marco Livesu                                              *
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
#include <cinolib/ARAP_deformation.h>
#include <cinolib/parallel_for.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void ARAP_deformation(const Trimesh<M,V,E,P> & m, ARAP_deformation_data & data)
{
    auto init = [&]()
    {
        data.init = false; // don't init next time
        data.xyz_loc.resize(m.num_polys()*3);

        // per edge weights
        data.w.resize(m.num_edges());
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            data.w.at(eid) = m.edge_cotangent_weight(eid);
        }

        // compute a map between matrix columns and mesh vertices
        // (Dirichlet boundary conditions will map to -1)
        data.col_map.resize(m.num_verts(),0);
        for(const auto & bc : data.bcs)
        {
            data.col_map.at(bc.first) = -1;
        }
        uint fresh_id = 0;
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            if(data.col_map.at(vid)==0)
            {
                data.col_map.at(vid) = fresh_id++;
            }
        }

        // Compute the Laplacian matrix and pre-factorize it
        typedef Eigen::Triplet<double> Entry;
        std::vector<Entry> entries;
        uint size = m.num_verts()-data.bcs.size();
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            int col = data.col_map.at(vid);
            if(col==-1) continue; // skip, BC
            for(uint nbr : m.adj_v2v(vid))
            {
                int col_nbr = data.col_map.at(nbr);
                int eid = m.edge_id(vid,nbr);
                assert(eid>=0);
                entries.push_back(Entry(col, col, data.w.at(eid)));
                if(col_nbr>=0)
                {
                    // if BC will add this quantity times the known coordinates to the RHS
                    entries.push_back(Entry(col, col_nbr, -data.w.at(eid)));
                }
            }
        }
        Eigen::SparseMatrix<double> A(m.num_verts()-data.bcs.size(), m.num_verts()-data.bcs.size());
        A.setFromTriplets(entries.begin(), entries.end());
        data.cache.derived().compute(A);

        // warm start: initialize the solution as the minimizer or
        // | L*p - delta(p) |^2
        // where p are the xyz coordinates and delta the differential
        // coordinates of each mesh vertex
        Eigen::VectorXd rhs_x = Eigen::VectorXd::Zero(size);
        Eigen::VectorXd rhs_y = Eigen::VectorXd::Zero(size);
        Eigen::VectorXd rhs_z = Eigen::VectorXd::Zero(size);
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            int col = data.col_map.at(vid);
            if(col==-1) continue; // skip BC
            for(uint eid : m.adj_v2e(vid))
            {
                uint nbr   = m.vert_opposite_to(eid,vid);
                rhs_x[col] += data.w.at(eid) * (m.vert(vid).x() - m.vert(nbr).x());
                rhs_y[col] += data.w.at(eid) * (m.vert(vid).y() - m.vert(nbr).y());
                rhs_z[col] += data.w.at(eid) * (m.vert(vid).z() - m.vert(nbr).z());
                // move the contribution of BCs to the RHS
                if(data.col_map.at(nbr)==-1)
                {
                    vec3d p = data.bcs.at(nbr);
                    rhs_x[col] += data.w.at(eid) * p.x();
                    rhs_y[col] += data.w.at(eid) * p.y();
                    rhs_z[col] += data.w.at(eid) * p.z();
                }
            }
        }
        auto x = data.cache.solve(rhs_x).eval();
        auto y = data.cache.solve(rhs_y).eval();
        auto z = data.cache.solve(rhs_z).eval();
        data.xyz_out.resize(m.num_verts());
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            int col = data.col_map[vid];
            if(col>=0) data.xyz_out[vid] = vec3d(x[col],y[col],z[col]);
        }
        for(const auto & bc : data.bcs)
        {
            data.xyz_out[bc.first] = bc.second;
        }

        // old fashioned (brutal) warm start
        //data.xyz_out = m.vector_verts();
        //for(const auto & bc : data.bcs) data.xyz_out.at(bc.first) = bc.second;
    };

    auto local_step = [&]()
    {
        PARALLEL_FOR(0, m.num_polys(), 1000, [&](uint pid)
        {
            // per triangle covariance matrix
            uint  off = 3*pid;
            mat3d cov = mat3d::ZERO();
            for(int i=0; i<3; ++i)
            {
                uint v0  = m.poly_vert_id(pid,i);
                uint v1  = m.poly_vert_id(pid,(i+1)%3);
                int  eid = m.edge_id(v0,v1);
                assert(eid>=0);
                vec3d e_cur = data.xyz_out.at(v0) - data.xyz_out.at(v1);
                vec3d e_ref = m.vert(v0) - m.vert(v1);
                cov += data.w.at(eid) * (e_cur * e_ref.transpose());
            }

            // find closest rotation and store rotated point
            mat3d rot = cov.closest_orthogonal_matrix(true);
            data.xyz_loc.at(off  ) = rot * m.poly_vert(pid,0);
            data.xyz_loc.at(off+1) = rot * m.poly_vert(pid,1);
            data.xyz_loc.at(off+2) = rot * m.poly_vert(pid,2);
        });
    };

    auto global_step = [&]()
    {
        uint size = m.num_verts()-data.bcs.size();
        Eigen::VectorXd rhs_x = Eigen::VectorXd::Zero(size);
        Eigen::VectorXd rhs_y = Eigen::VectorXd::Zero(size);
        Eigen::VectorXd rhs_z = Eigen::VectorXd::Zero(size);
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            int col = data.col_map.at(vid);
            if(col==-1) continue; // skip, vert is BC
            for(uint nbr : m.adj_v2v(vid))
            {
                int eid = m.edge_id(vid,nbr);
                double w = 1.0/m.adj_e2p(eid).size();
                for(uint pid : m.adj_e2p(eid))
                {
                    uint i = m.poly_vert_offset(pid,vid);
                    uint j = m.poly_vert_offset(pid,nbr);
                    assert(i>=0 && i<3);
                    assert(j>=0 && j<3);
                    vec3d Re = data.xyz_loc.at(pid*3+i) - data.xyz_loc.at(pid*3+j);
                    rhs_x[col] += w * data.w.at(eid) * Re[0];
                    rhs_y[col] += w * data.w.at(eid) * Re[1];
                    rhs_z[col] += w * data.w.at(eid) * Re[2];
                }
                // if nbr is a BC sum its contibution to the Laplacian matrix to the rhs
                if(data.col_map.at(nbr)==-1)
                {
                    vec3d p = data.bcs.at(nbr);
                    rhs_x[col] += data.w.at(eid) * p.x();
                    rhs_y[col] += data.w.at(eid) * p.y();
                    rhs_z[col] += data.w.at(eid) * p.z();
                }
            }
        }        
        Eigen::VectorXd x = data.cache.solve(rhs_x).eval();
        Eigen::VectorXd y = data.cache.solve(rhs_y).eval();
        Eigen::VectorXd z = data.cache.solve(rhs_z).eval();
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            int col = data.col_map[vid];
            if(col>=0) data.xyz_out[vid] = vec3d(x[col],y[col],z[col]);
        }
        for(const auto & bc : data.bcs)
        {
            data.xyz_out[bc.first] = bc.second;
        }
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
