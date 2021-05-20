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
#include <cinolib/smoother.h>
#include <cinolib/laplacian.h>
#include <cinolib/linear_solvers.h>
#include <cinolib/octree.h>

namespace cinolib
{

template<class M1, class V1, class E1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
void mesh_smoother(      AbstractPolygonMesh<M1,V1,E1,P1> & m,
                   const AbstractPolygonMesh<M2,V2,E2,P2> & target,
                   const SmootherOptions                  & opt)
{
    // BUILD OCTREES
    Octree o_srf;    // for general surface
    Octree o_line;   // for feature lines
    Octree o_corner; // for feature corners (i.e. points where feature lines meet or terminate)
    //
    for(uint eid=0; eid<target.num_edges(); ++eid)
    {
        if(target.edge_data(eid).flags[MARKED]) // marked => flagged as a sharp feature
        {
            o_line.push_segment(eid, target.edge_verts(eid));
        }
    }
    for(uint vid=0; vid<target.num_verts(); ++vid)
    {
        uint count = 0;
        for(uint eid : target.adj_v2e(vid))
        {
            if(target.edge_data(eid).flags[MARKED]) ++count;
        }
        if(count>0 && count!=2)
        {
            o_corner.push_point(vid, target.vert(vid));
        }
    }
    o_srf.build_from_mesh_polys(target);
    o_line.build();
    o_corner.build();

    // LABEL MESH VERTICES
    enum { REGULAR, CORNER, FEATURE };
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        uint count = 0;
        for(uint eid : m.adj_v2e(vid))
        {
            // marked => flagged as a sharp feature
            if(m.edge_data(eid).flags[MARKED]) ++count;
        }
        switch(count)
        {
            case 0  : m.vert_data(vid).label = REGULAR; break;
            case 2  : m.vert_data(vid).label = FEATURE; break;
            default : m.vert_data(vid).label = CORNER;  break;
        }
    }

    std::vector<Entry>  entries; // coeff matrix
    std::vector<double> w;       // weights matrix
    std::vector<double> rhs;     // right hand side
    uint row = 0;

    // additional data used to parameterize verts along feature lines (one for each such vert)
    // each feature vertex is defined as P_curr + dir*t
    std::unordered_map<uint,std::pair<vec3d,uint>> feature_data;

    // E_laplacian = \sum_{\forall i} \sum_{\forall j \in N(i)} (v_i - v_j)^2
    auto laplacian = [&]()
    {
        auto L = laplacian_matrix_entries(m, opt.laplacian_mode, 3); // TODO: add row and col offsets for laplacian...
        for(auto entry : L) entries.push_back(entry);
        uint extra_rows = m.num_verts()*3;
        w.reserve(w.size()+extra_rows);
        rhs.reserve(rhs.size()+extra_rows);
        for(uint i=0; i<extra_rows; ++i)
        {
            w.push_back(opt.w_laplace);
            rhs.push_back(0);
        }
        row += extra_rows;
    };

    // E_regular = \sum_{\forall i \in R} (n*v_i + d)^2,
    // where <n,d> is the plane tangent to the mesh at v_i
    auto tangent_space = [&](const uint vid)
    {
        vec3d  p;
        double dist;
        uint   pid;
        o_srf.closest_point(m.vert(vid), pid, p, dist);
        vec3d n = target.poly_data(pid).normal;

        // reduces energy for mapping to distant points
        // because they are likely to be wrong assignments
        double w_regular = opt.w_corner;
        if(dist>m.edge_avg_length(vid)*2) w_regular *= 0.01; // TODO: this should be a Gaussian....

        uint  nv    = m.num_verts();
        uint  col_x = vid;
        uint  col_y = nv + vid;
        uint  col_z = nv + nv + vid;

        entries.push_back(Entry(row, col_x, n.x()));
        entries.push_back(Entry(row, col_y, n.y()));
        entries.push_back(Entry(row, col_z, n.z()));
        rhs.push_back(n.dot(p));
        w.push_back(w_regular);
        ++row;
    };

    // E_feature = \sum_{\forall i \in F} (v_i - (v_i + t*d))^2 + t^2,
    // where <t,d> is the line L::= v_i + t*d tangent to the crease at v_i,
    // parameterized by the extra varaible t
    auto tangent_line = [&](const uint vid)
    {
        vec3d  p;
        double dist;
        uint   eid;
        o_line.closest_point(m.vert(vid), eid, p, dist);
        vec3d dir = target.edge_vec(eid,true);

        uint  nv    = m.num_verts();
        uint  col_x = vid;
        uint  col_y = nv + vid;
        uint  col_z = nv + nv + vid;
        uint  col_t = nv + nv + nv + feature_data.size();
        feature_data[vid] = std::make_pair(dir,col_t);

        entries.push_back(Entry(row, col_x, 1.0));
        entries.push_back(Entry(row, col_t, dir.x()));
        rhs.push_back(p.x());
        w.push_back(opt.w_feature);
        ++row;

        entries.push_back(Entry(row, col_y, 1.0));
        entries.push_back(Entry(row, col_t, dir.y()));
        rhs.push_back(p.y());
        w.push_back(opt.w_feature);
        ++row;

        entries.push_back(Entry(row, col_z, 1.0));
        entries.push_back(Entry(row, col_t, dir.z()));
        rhs.push_back(p.z());
        w.push_back(opt.w_feature);
        ++row;

        entries.push_back(Entry(row, col_t, 1.0));
        rhs.push_back(0.0);
        w.push_back(1.0);
        ++row;
    };

    // E_corner = \sum_{\forall i \in C} (v_i - v_i*)^2,
    // where v_i* is the current position of v_i
    auto corner = [&](const uint vid)
    {
        vec3d  p;
        double dist;
        uint   pid;
        o_corner.closest_point(m.vert(vid), pid, p, dist);

        // discards mappings to distant corners because they are likely to be wrong assignments
        // (e.g. if the feature networks of source and target meshes mismatch)
        if(dist>m.edge_avg_length(vid)*2) return;

        uint  nv    = m.num_verts();
        uint  col_x = vid;
        uint  col_y = nv + vid;
        uint  col_z = nv + nv + vid;

        entries.push_back(Entry(row, col_x, 1.0));
        rhs.push_back(p.x());
        w.push_back(opt.w_corner);
        ++row;

        entries.push_back(Entry(row, col_y, 1.0));
        rhs.push_back(p.y());
        w.push_back(opt.w_corner);
        ++row;

        entries.push_back(Entry(row, col_z, 1.0));
        rhs.push_back(p.z());
        w.push_back(opt.w_corner);
        ++row;
    };

    // SMOOTHING ITERATIONS
    for(uint i=0; i<opt.n_iters; ++i)
    {
        laplacian();
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            switch(m.vert_data(vid).label)
            {
                case REGULAR: tangent_space(vid); break;
                case FEATURE: tangent_line(vid);  break;
                case CORNER:  corner(vid);        break;
                default: assert(false && "unknown vertex type");
            }
        }

        Eigen::SparseMatrix<double> A(row,m.num_verts()*3+feature_data.size());
        A.setFromTriplets(entries.begin(), entries.end());
        Eigen::VectorXd RHS = Eigen::Map<Eigen::VectorXd>(rhs.data(), rhs.size());
        Eigen::VectorXd W   = Eigen::Map<Eigen::VectorXd>(w.data(), w.size());
        Eigen::VectorXd res;
        solve_weighted_least_squares(A, W, RHS, res);

        uint nv = m.num_verts();
        for(uint vid=0; vid<nv; ++vid)
        {
            vec3d p(res[vid], res[nv+vid], res[2*nv+vid]);

            switch(m.vert_data(vid).label)
            {
                case REGULAR: m.vert(vid) = (opt.reproject_on_target) ? o_srf.closest_point(p)    : p; break;
                case CORNER:  m.vert(vid) = (opt.reproject_on_target) ? o_corner.closest_point(p) : p; break;
                case FEATURE:
                {
                    const auto & line = feature_data.at(vid);
                    p += line.first * res[line.second];
                    m.vert(vid) = (opt.reproject_on_target) ? o_line.closest_point(p) : p;
                    break;
                }
                default: assert(false && "unknown vertex type");
            }
        }

        if(i<opt.n_iters)
        {
            feature_data.clear();
            entries.clear();
            w.clear();
            rhs.clear();
            row = 0;
        }
    }
}

}
