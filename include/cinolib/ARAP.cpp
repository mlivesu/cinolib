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
#include <cinolib/ARAP.h>
#include <cinolib/parallel_for.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void ARAP(AbstractMesh<M,V,E,P> & m, ARAP_data & data)
{
    auto init = [&]()
    {
        assert(m.mesh_type()==TRIMESH || m.mesh_type()==TETMESH);

        data.init = false; // don't init next time
        data.xyz_loc.resize(m.num_polys()*m.verts_per_poly(0));

        // per edge weights
        data.w.resize(m.num_edges());
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            data.w.at(eid) = m.edge_weight(eid,data.w_type);
        }

        // compute a map between matrix columns and mesh vertices
        // if hard constraints are used, boundary conditions will
        // map to -1, meaning that they do not correspond to any
        // column in the matrix
        data.col_map.resize(m.num_verts(),0);
        if(!data.use_soft_constraints)
        {
            for(const auto & bc : data.bcs)
            {
                data.col_map.at(bc.first) = -1;
            }
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
        uint size = m.num_verts() - uint(data.bcs.size());
        if(data.use_soft_constraints)
        {
            size = m.num_verts() + uint(data.bcs.size());
            data.W.resize(size);
        }
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            int col = data.col_map.at(vid);
            if(col==-1) continue; // skip, hard BC
            if(data.use_soft_constraints) data.W[col] = data.w_laplace;
            for(uint nbr : m.adj_v2v(vid))
            {
                int col_nbr = data.col_map.at(nbr);
                int eid = m.edge_id(vid,nbr);
                assert(eid>=0);
                entries.push_back(Entry(col, col, data.w.at(eid)));
                if(col_nbr==-1) continue; // skip, hard BC
                entries.push_back(Entry(col, col_nbr, -data.w.at(eid)));
            }
        }
        if(data.use_soft_constraints)
        {
            // models equation => x_bc = bc_value * w_constr
            uint new_row = m.num_verts();
            for(auto bc : data.bcs)
            {
                entries.push_back(Entry(new_row,bc.first,1));
                data.W[new_row] = data.w_constr;
                ++new_row;
            }
        }
        data.A = Eigen::SparseMatrix<double>(size, (data.use_soft_constraints) ? m.num_verts() : size);
        data.A.setFromTriplets(entries.begin(), entries.end());
        if(data.use_soft_constraints) data.cache.derived().compute(data.A.transpose()*data.W.asDiagonal()*data.A);
        else data.cache.derived().compute(data.A);

        if(data.warm_start_with_laplacian)
        {
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
            if(data.use_soft_constraints)
            {
                auto x = data.cache.solve(data.A.transpose()*data.W.asDiagonal()*rhs_x).eval();
                auto y = data.cache.solve(data.A.transpose()*data.W.asDiagonal()*rhs_y).eval();
                auto z = data.cache.solve(data.A.transpose()*data.W.asDiagonal()*rhs_z).eval();
                data.xyz_out.resize(m.num_verts());
                for(uint vid=0; vid<m.num_verts(); ++vid)
                {
                    data.xyz_out[vid] = vec3d(x[vid],y[vid],z[vid]);
                }
            }
            else
            {
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
            }
        }
        else
        {
            data.xyz_out = m.vector_verts();
            for(const auto & bc : data.bcs) data.xyz_out.at(bc.first) = bc.second;
        }
    };

    auto local_step = [&]()
    {
        PARALLEL_FOR(0, m.num_polys(), 1000, [&](uint pid)
        {
            mat3d cov = mat3d::ZERO();            
            for(uint eid : m.adj_p2e(pid))
            {
                uint  v0    = m.edge_vert_id(eid,0);
                uint  v1    = m.edge_vert_id(eid,1);
                vec3d e_cur = data.xyz_out.at(v0) - data.xyz_out.at(v1);
                vec3d e_ref = m.vert(v0) - m.vert(v1);
                cov += data.w.at(eid) * (e_cur * e_ref.transpose());
            }

            // find closest rotation and store rotated point
            uint  off = pid*m.verts_per_poly(pid);
            mat3d rot = cov.closest_orthogonal_matrix(true);
            for(uint i=0; i<m.verts_per_poly(pid); ++i)
            {
                data.xyz_loc.at(off+i) = rot * m.poly_vert(pid,i);
            }
        });
    };

    auto global_step = [&]()
    {
        uint size = (data.use_soft_constraints) ? m.num_verts() + uint(data.bcs.size())
                                                : m.num_verts() - uint(data.bcs.size());
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
                    vec3d Re = data.xyz_loc.at(pid*m.verts_per_poly(pid)+i) - data.xyz_loc.at(pid*m.verts_per_poly(pid)+j);
                    rhs_x[col] += w * data.w.at(eid) * Re[0];
                    rhs_y[col] += w * data.w.at(eid) * Re[1];
                    rhs_z[col] += w * data.w.at(eid) * Re[2];
                }
                // if nbr is a hard BC sum its contibution to the Laplacian matrix to the rhs
                if(data.col_map.at(nbr)==-1)
                {
                    vec3d p = data.bcs.at(nbr);
                    rhs_x[col] += data.w.at(eid) * p.x();
                    rhs_y[col] += data.w.at(eid) * p.y();
                    rhs_z[col] += data.w.at(eid) * p.z();
                }
            }
        }
        if(data.use_soft_constraints)
        {
            // models rhs of equation => x_bc = bc_value
            uint new_row = m.num_verts();
            for(auto bc : data.bcs)
            {
                rhs_x[new_row] = bc.second.x();
                rhs_y[new_row] = bc.second.y();
                rhs_z[new_row] = bc.second.z();
                ++new_row;
            }
            auto x = data.cache.solve(data.A.transpose()*data.W.asDiagonal()*rhs_x).eval();
            auto y = data.cache.solve(data.A.transpose()*data.W.asDiagonal()*rhs_y).eval();
            auto z = data.cache.solve(data.A.transpose()*data.W.asDiagonal()*rhs_z).eval();
            for(uint vid=0; vid<m.num_verts(); ++vid)
            {
                data.xyz_out[vid] = vec3d(x[vid],y[vid],z[vid]);
            }
        }
        else
        {
            auto x = data.cache.solve(rhs_x).eval();
            auto y = data.cache.solve(rhs_y).eval();
            auto z = data.cache.solve(rhs_z).eval();
            for(uint vid=0; vid<m.num_verts(); ++vid)
            {
                int col = data.col_map[vid];
                if(col>=0) data.xyz_out[vid] = vec3d(x[col],y[col],z[col]);
            }
            for(const auto & bc : data.bcs)
            {
                data.xyz_out[bc.first] = bc.second;
            }
        }
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    if(data.init) init();

    for(uint i=0; i<data.n_iters; ++i)
    {
        local_step();
        global_step();
    }

    m.vector_verts() = data.xyz_out;
    m.update_normals();
}

}
