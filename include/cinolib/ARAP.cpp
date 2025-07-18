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
#include <cinolib/laplacian.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
void ARAP(AbstractMesh<M,V,E,P> & m, ARAP_data & data)
{
    /////////////////////////////////////////////////////////////////////////

    auto init = [&]()
    {
        assert(m.mesh_type()==TRIMESH || m.mesh_type()==TETMESH);

        data.init = false;

        data.R.resize(m.num_verts());
        data.xyz_ref = m.vector_verts();

        data.w.resize(m.num_edges());
        for(uint eid=0; eid<m.num_edges(); ++eid)
        {
            data.w.at(eid) = m.edge_weight(eid,data.w_type);
        }

        if(data.hard_constrain_handles)
        {
            data.A = -laplacian(m,data.w_type,1);
        }
        else
        {
            uint nv = m.num_verts();
            uint nh = data.handles.size();
            std::vector<Eigen::Triplet<double>> entries;
            for(uint vid=0; vid<m.num_verts(); ++vid)
            {
                double diag = 0;
                for(uint nbr : m.adj_v2v(vid))
                {
                    int eid = m.edge_id(vid,nbr);
                    entries.emplace_back(vid,nbr,-data.w.at(eid));
                    diag += data.w.at(eid);
                }
                entries.emplace_back(vid,vid,diag);
            }
            uint off = 0;
            for(uint vid : data.handles)
            {
                entries.emplace_back(nv+off,vid,1.0);
                off++;
            }
            data.A.resize(nv+nh,nv);
            data.A.setFromTriplets(entries.begin(), entries.end());
            data.cache.derived().compute(data.A.transpose()*data.A);
        }
    };

    /////////////////////////////////////////////////////////////////////////

    auto local_step = [&]()
    {
        PARALLEL_FOR(0, m.num_verts(), 1000, [&](uint vid)
        {
            mat3d cov = mat3d::ZERO();
            for(uint nbr : m.adj_v2v(vid))
            {
                int   eid   = m.edge_id(vid,nbr);
                vec3d e_ref = (data.xyz_ref.at(vid) - data.xyz_ref.at(nbr));
                vec3d e_cur = (m.vert(vid) - m.vert(nbr));

                cov += data.w.at(eid) * (e_cur * e_ref.transpose());
            }

            mat3d UU,VV;
            vec3d S;
            cov.SVD(UU,S,VV);
            mat3d I = mat3d::DIAG(1);
            I(2,2) = (UU*VV.transpose()).det();
            data.R.at(vid) = UU*I*VV.transpose();
        });
    };

    /////////////////////////////////////////////////////////////////////////

    auto global_step = [&]()
    {
        uint nv = m.num_verts();
        uint nh = (data.hard_constrain_handles) ? 0 : data.handles.size();

        Eigen::VectorXd rhs_x(nv+nh);
        Eigen::VectorXd rhs_y(nv+nh);
        Eigen::VectorXd rhs_z(nv+nh);

        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            vec3d rhs(0,0,0);
            for(uint nbr : m.adj_v2v(vid))
            {
                mat3d Ravg = (data.R.at(vid)+data.R.at(nbr))/2.0;
                vec3d e    = (data.xyz_ref.at(vid) - data.xyz_ref.at(nbr));
                int eid    = m.edge_id(vid,nbr);

                rhs += data.w.at(eid) * Ravg * e;
            }
            rhs_x[vid] = rhs.x();
            rhs_y[vid] = rhs.y();
            rhs_z[vid] = rhs.z();
        }

        Eigen::VectorXd x,y,z;
        if(data.hard_constrain_handles)
        {
            // TODO: use caching also with hard constraints!
            solve_square_system_with_bc(data.A, rhs_x, x, data.handles_x);
            solve_square_system_with_bc(data.A, rhs_y, y, data.handles_y);
            solve_square_system_with_bc(data.A, rhs_z, z, data.handles_z);
        }
        else
        {
            uint off = 0;
            for(uint vid : data.handles)
            {
                rhs_x[nv+off] = data.handles_x.at(vid);
                rhs_y[nv+off] = data.handles_y.at(vid);
                rhs_z[nv+off] = data.handles_z.at(vid);
                ++off;
            }
            x = data.cache.solve(data.A.transpose()*rhs_x).eval();
            y = data.cache.solve(data.A.transpose()*rhs_y).eval();
            z = data.cache.solve(data.A.transpose()*rhs_z).eval();
        }

        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            m.vert(vid) = vec3d(x[vid],y[vid],z[vid]);
        }
    };

    //////////////////////////////////////////////////////////////////////////

    if(data.init) init();

    for(uint i=0; i<data.n_iters; ++i)
    {
        local_step();
        global_step();
    }
    m.update_normals();
}

}
