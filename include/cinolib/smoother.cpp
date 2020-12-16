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

typedef struct
{
    vec3d p,dir; // point on feature curve + tangent to the curve
    uint  t;     // column id of the parameterization of p along line dir
}
FeatureLine;

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
void project_on_tangent_space(const AbstractPolygonMesh<M,V,E,P> & m,
                              const uint                           vid,
                              const AbstractPolygonMesh<M,V,E,P> & target,
                              const Octree                       & octree,
                              const double                         weight,
                                    uint                         & row,
                                    std::vector<double>          & w,       // weights matrix
                                    std::vector<Entry>           & entries, // coeff matrix
                                    std::vector<double>          & rhs)     // right hand side
{
    vec3d  p;
    double dist;
    uint   pid;
    octree.closest_point(m.vert(vid), pid, p, dist);
    vec3d n = target.poly_data(pid).normal;

    if(n.length_squared()==0)
    {
        std::cout << "WARNING: degenerate tangent space! Skip it" << std::endl;
        return;
        // NOTE: I might want to do something similar even if "dist" is too big
        // (e.g. when compared to local edge length).Alternatively, I might use
        // the tangent space of m...
    }

    uint  nv    = m.num_verts();
    uint  col_x = vid;
    uint  col_y = nv + vid;
    uint  col_z = nv + nv + vid;

    entries.push_back(Entry(row, col_x, n.x()));
    entries.push_back(Entry(row, col_y, n.y()));
    entries.push_back(Entry(row, col_z, n.z()));
    rhs.push_back(n.dot(p));
    w.push_back(weight);
    ++row;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void project_on_tangent_line(const AbstractPolygonMesh<M,V,E,P>         & m,
                             const uint                                   vid,
                             const AbstractPolygonMesh<M,V,E,P>         & target,
                             const Octree                               & octree,
                             const double                                 weight,
                                   uint                                 & row,
                                   std::vector<double>                  & w,       // weights matrix
                                   std::vector<Entry>                   & entries, // coeff matrix
                                   std::vector<double>                  & rhs,     // right hand side
                                   std::unordered_map<uint,FeatureLine> & feature_data)
{
    // set energy term: w * [p_new - (p + t*dir)]^2 + t^2

    vec3d  p;
    double dist;
    uint   eid;
    octree.closest_point(m.vert(vid), eid, p, dist);
    vec3d dir = target.edge_vec(eid,true);

    if(dir.length_squared()==0)
    {
        std::cout << "WARNING: zero length tangent curve! Skip it" << std::endl;
        return;
        // NOTE: I might want to do something similar even if "dist" is too big
        // (e.g. when compared to local edge length). Alternatively, I might use
        // the tangent line of m...
    }

    // better to update feature tangent space at each iteration?
    assert(DOES_NOT_CONTAIN(feature_data,vid));

    uint  nv    = m.num_verts();
    uint  col_x = vid;
    uint  col_y = nv + vid;
    uint  col_z = nv + nv + vid;
    uint  col_t = nv + nv + nv + feature_data.size();

    FeatureLine f;
    f.p = p;
    f.dir = dir;
    f.t = col_t;
    feature_data[vid] = f;

    entries.push_back(Entry(row, col_x, 1.0));
    entries.push_back(Entry(row, col_t, dir.x()));
    rhs.push_back(p.x());
    w.push_back(weight);
    ++row;

    entries.push_back(Entry(row, col_y, 1.0));
    entries.push_back(Entry(row, col_t, dir.y()));
    rhs.push_back(p.y());
    w.push_back(weight);
    ++row;

    entries.push_back(Entry(row, col_z, 1.0));
    entries.push_back(Entry(row, col_t, dir.z()));
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
void project_on_target(const AbstractPolygonMesh<M,V,E,P> & m,
                       const uint                           vid,
                       const Octree                       & octree,
                       const double                         weight,
                             uint                         & row,
                             std::vector<double>          & w,       // weights matrix
                             std::vector<Entry>           & entries, // coeff matrix
                             std::vector<double>          & rhs)     // right hand side
{
    vec3d  p;
    double dist;
    uint   pid;
    octree.closest_point(m.vert(vid), pid, p, dist);

    if(dist>m.bbox().diag()*0.15)
    {
        std::cout << "WARNING: per vert displacement seems too big! Skip it" << std::endl;
        return;
    }

    uint  nv    = m.num_verts();
    uint  col_x = vid;
    uint  col_y = nv + vid;
    uint  col_z = nv + nv + vid;

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
    Octree ref_srf;         // for general surface
    Octree ref_feat_line;   // for feature lines
    Octree ref_feat_corner; // for feature corners (i.e. points where feature lines end or group together)

    ref_srf.build_from_mesh_polys(target);
    // feature lines
    for(uint eid=0; eid<target.num_edges(); ++eid)
    {
        if(target.edge_data(eid).flags[MARKED]) // marked => flagged as a sharp feature
        {
            ref_feat_line.push_segment(eid, target.edge_verts(eid));
        }
    }
    ref_feat_line.build();
    // feature corners
    for(uint vid=0; vid<target.num_verts(); ++vid)
    {
        uint count = 0;
        for(uint eid : target.adj_v2e(vid))
        {
            if(target.edge_data(eid).flags[MARKED]) ++count;
        }
        if(count>0 && count!=2)
        {
            ref_feat_corner.push_point(vid, target.vert(vid));
        }
    }
    ref_feat_corner.build();

    label_features(m);

    for(uint i=0; i<opt.n_iters; ++i)
    {
        std::cout << "smooth iter #" << i << std::endl;

        std::vector<Entry>  entries; // coeff matrix
        std::vector<double> w;       // weights matrix
        std::vector<double> rhs;     // right hand side
        uint row = 0;

        laplacian_term(m, opt.laplacian_mode, opt.w_laplace, row, w, entries, rhs);

        // additional data used to parameterize verts along feature lines (one for each such vert)
        // each feature vertex is defined as P_new = P_curr + dir*t
        std::unordered_map<uint,FeatureLine> feature_data; // vid => p,dir,t

        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            switch(m.vert_data(vid).label)
            {
                case REGULAR:
                {
                    //project_on_target(m, vid, ref_srf, opt.w_curr_pos, row, w, entries, rhs);
                    project_on_tangent_space(m, vid, target, ref_srf, opt.w_regular, row, w, entries, rhs);
                    break;
                }

                case FEATURE:
                {
                    //project_on_target(m, vid, ref_feat_line, opt.w_curr_pos, row, w, entries, rhs);
                    project_on_tangent_line(m, vid, target, ref_feat_line, opt.w_feature, row, w, entries, rhs, feature_data);
                    break;
                }

                case CORNER:
                {
                    project_on_target(m, vid, ref_feat_corner, opt.w_corner, row, w, entries, rhs);
                    break;
                }
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
                {
                    vec3d p(res[vid], res[nv+vid], res[2*nv+vid]);
                    m.vert(vid) = (opt.reproject_on_target) ? ref_srf.closest_point(p) : p;
                    break;
                }

                case CORNER:
                {
                    vec3d p(res[vid], res[nv+vid], res[2*nv+vid]);
                    m.vert(vid) = (opt.reproject_on_target) ? ref_feat_corner.closest_point(p) : p;
                    break;
                }

                case FEATURE:
                {
                    vec3d p(res[vid], res[nv+vid], res[2*nv+vid]);
                    const FeatureLine & f = feature_data.at(vid);
                    p += f.dir * res[f.t];
                    m.vert(vid) = (opt.reproject_on_target) ? ref_feat_line.closest_point(p) : p;
                    break;
                }

                default: assert(false && "unknown vertex type");
            }
        }
    }
}

}
