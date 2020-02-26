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

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void laplacian_term(const AbstractPolygonMesh<M,V,E,P> & m,
                    const int                            mode, // UNIFORM, COTANGENT
                    const double                         weight,
                    uint                               & row,
                    std::vector<double>                & w,       // weights matrix
                    std::vector<Entry>                 & entries, // coeff matrix
                    std::vector<double>                & rhs)     // right hand side
{
    auto L = laplacian_matrix_entries(m, mode, 3); // TODO: add row and col offsets for laplacian...
    for(auto entry : L) entries.push_back(entry);
    uint extra_rows = m.num_verts()*3;
    w.reserve(w.size()+extra_rows);
    rhs.reserve(rhs.size()+extra_rows);
    for(uint i=0; i<extra_rows; ++i)
    {
        w.push_back(weight);
        rhs.push_back(0);
    }
    row += extra_rows;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void smooth_on_tangent_space(const AbstractPolygonMesh<M,V,E,P> & m,
                             const uint                           vid,
                             const double                         weight,
                                   uint                         & row,
                                   std::vector<double>          & w,       // weights matrix
                                   std::vector<Entry>           & entries, // coeff matrix
                                   std::vector<double>          & rhs)     // right hand side
{
    uint  nv    = m.num_verts();
    uint  col_x = vid;
    uint  col_y = nv + vid;
    uint  col_z = nv + nv + vid;
    vec3d p     = m.vert(vid);

    // to make this piece of code robust against inconsistent
    // per face winding, rather than using (possibly wrong) per
    // vertex normals, I define as many tangent space as the
    // number of incident faces. The gobal weight is distributed
    // across all incident faces
    //
    double norm_factor = m.adj_v2p(vid).size();
    for(uint pid : m.adj_v2p(vid))
    {
        vec3d n = m.poly_data(pid).normal;
        if(n.length_squared()==0)
        {
            std::cout << "WARNING: zero length face normal! Skip it" << std::endl;
            continue;
        }
        entries.push_back(Entry(row, col_x, n.x()));
        entries.push_back(Entry(row, col_y, n.y()));
        entries.push_back(Entry(row, col_z, n.z()));
        rhs.push_back(n.dot(p));
        w.push_back(weight/norm_factor);
        ++row;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void smooth_on_tangent_line(const AbstractPolygonMesh<M,V,E,P>                   & m,
                            const uint                                             vid,
                            const double                                           weight,
                                  uint                                           & row,
                                  std::vector<double>                            & w,       // weights matrix
                                  std::vector<Entry>                             & entries, // coeff matrix
                                  std::vector<double>                            & rhs,     // right hand side
                                  std::unordered_map<uint,std::pair<vec3d,uint>> & feature_data)
{
    // set energy term: w * [p_new - (p + t*dir))^2] + t^2

    std::vector<vec3d> nbrs;
    for(uint eid : m.adj_v2e(vid))
    {
        if(m.edge_data(eid).flags[MARKED]) nbrs.push_back(m.vert(m.vert_opposite_to(eid,vid)));
    }
    assert(nbrs.size()==2);
    vec3d dir = nbrs.front() - nbrs.back();
    dir.normalize();

    if(dir.length_squared()==0)
    {
        std::cout << "WARNING: zero length tangent curve! Skip it" << std::endl;
        return;
    }

    // better to update feature tangent space at each iteration?
    assert(DOES_NOT_CONTAIN(feature_data,vid));

    vec3d p     = m.vert(vid);
    uint  nv    = m.num_verts();
    uint  col_x = vid;
    uint  col_y = nv + vid;
    uint  col_z = nv + nv + vid;
    uint  col_t = nv + nv + nv + feature_data.size();
    feature_data[vid] = std::make_pair(dir,col_t);

    entries.push_back(Entry(row, col_x,  1.0));
    entries.push_back(Entry(row, col_t, -dir.x()));
    rhs.push_back(p.x());
    w.push_back(weight);
    ++row;

    entries.push_back(Entry(row, col_y,  1.0));
    entries.push_back(Entry(row, col_t, -dir.y()));
    rhs.push_back(p.y());
    w.push_back(weight);
    ++row;

    entries.push_back(Entry(row, col_z,  1.0));
    entries.push_back(Entry(row, col_t, -dir.z()));
    rhs.push_back(p.z());
    w.push_back(weight);
    ++row;

    entries.push_back(Entry(row, col_t, 1.0));
    rhs.push_back(0.0);
    w.push_back(1.0);
    ++row;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void hold_in_curr_pos(const AbstractPolygonMesh<M,V,E,P> & m,
                 const uint                           vid,
                 const double                         weight,
                       uint                         & row,
                       std::vector<double>          & w,       // weights matrix
                       std::vector<Entry>           & entries, // coeff matrix
                       std::vector<double>          & rhs)     // right hand side
{
    uint  nv    = m.num_verts();
    uint  col_x = vid;
    uint  col_y = nv + vid;
    uint  col_z = nv + nv + vid;
    vec3d p     = m.vert(vid);

    entries.push_back(Entry(row, col_x, 1.0));
    rhs.push_back(p.x());
    w.push_back(weight);
    ++row;

    entries.push_back(Entry(row, col_y, 1.0));
    rhs.push_back(p.y());
    w.push_back(weight);
    ++row;

    entries.push_back(Entry(row, col_z, 1.0));
    rhs.push_back(p.z());
    w.push_back(weight);
    ++row;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void label_features(AbstractPolygonMesh<M,V,E,P> & m)
{
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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M1, class V1, class E1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
void mesh_smoother(      AbstractPolygonMesh<M1,V1,E1,P1> & m,
                   const AbstractPolygonMesh<M2,V2,E2,P2> & target,
                   const SmootherOptions                  & opt)
{
    Octree ref_srf, ref_feat;

    // if reprojection is needed, make octrees
    if(opt.reproject_on_target)
    {
        ref_srf.build_from_mesh_polys(target);
        for(uint eid=0; eid<target.num_edges(); ++eid)
        {
            if(target.edge_data(eid).flags[MARKED]) // marked => flagged as a sharp feature
            {
                ref_feat.add_segment(eid, target.edge_verts(eid));
            }
        }
        ref_feat.build();
    }

    label_features(m);

    for(uint i=0; i<opt.n_iters; ++i)
    {
        //std::cout << "smooth iter #" << i << std::endl;

        std::vector<Entry>  entries; // coeff matrix
        std::vector<double> w;       // weights matrix
        std::vector<double> rhs;     // right hand side
        uint row = 0;

        laplacian_term(m, opt.laplacian_mode, opt.w_laplace, row, w, entries, rhs);

        // additional data used to parameterize verts along feature lines (one for each such vert)
        // each feature vertex is defined as P_new = P_curr + dir*t
        std::unordered_map<uint,std::pair<vec3d,uint>> feature_data; // vid => dir,t

        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            hold_in_curr_pos(m, vid, opt.w_curr_pos, row, w, entries, rhs);

            switch(m.vert_data(vid).label)
            {
                case REGULAR: smooth_on_tangent_space(m, vid, opt.w_regular, row, w, entries, rhs); break;
                case FEATURE: smooth_on_tangent_line(m, vid, opt.w_feature, row, w, entries, rhs, feature_data); break;
                case CORNER:  hold_in_curr_pos(m, vid, opt.w_corner, row, w, entries, rhs); break;
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
            switch(m.vert_data(vid).label)
            {
                case REGULAR:
                case CORNER:
                {
                    vec3d p(res[vid], res[nv+vid], res[2*nv+vid]);
                    m.vert(vid) = (opt.reproject_on_target) ? ref_srf.closest_point(p) : p;
                    break;
                }

                case FEATURE:
                {
                    vec3d p = m.vert(vid);
                    if(CONTAINS(feature_data, vid)) // degenerate features are removed, I need this check
                    {
                        p += feature_data.at(vid).first*res[feature_data.at(vid).second];
                    }
                    m.vert(vid) = (opt.reproject_on_target) ? ref_feat.closest_point(p) : p;
                    break;
                }

                default: assert(false && "unknown vertex type");
            }
        }
    }
}

}
