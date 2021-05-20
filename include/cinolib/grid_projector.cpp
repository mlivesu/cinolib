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
#include <cinolib/grid_projector.h>
#include <cinolib/octree.h>

namespace cinolib
{

template<class M1, class V1, class E1, class F1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
double grid_projector(      Hexmesh<M1,V1,E1,F1,P1> & m,
                      const Trimesh<M2,V2,E2,P2>    & srf,
                      const GridProjectorOptions    & opt)
{
    struct Proj
    {
        uint   vid;
        vec3d  target;
        double dist;
    };
    std::vector<Proj> targets;

    // prepare octrees for projection
    Octree o_srf;
    Octree o_corners;
    Octree o_lines;
    for(uint vid=0; vid<srf.num_verts(); ++vid)
    {
        uint count = 0;
        for(uint eid : srf.adj_v2e(vid))
        {
            if(srf.edge_data(eid).flags[CREASE]) ++count;
        }
        switch(count)
        {
            case 0  : break;
            case 2  : break;
            default : o_corners.push_point(vid, srf.vert(vid));
        }
    }
    for(uint eid=0; eid<srf.num_edges(); ++eid)
    {
        if(srf.edge_data(eid).flags[CREASE])
        {
            o_lines.push_segment(eid, srf.edge_verts(eid));
        }
    }
    o_srf.build_from_mesh_polys(srf);
    o_corners.build();
    o_lines.build();

    // lavel mesh elements to set the target octree for projection
    enum { CORNER, LINE, REGULAR };
    m.vert_apply_label(REGULAR);
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        if(!m.vert_is_on_srf(vid)) continue;
        uint count = 0;
        for(uint eid : m.adj_v2e(vid))
        {
            if(m.edge_data(eid).flags[CREASE]) ++count;
        }
        switch(count)
        {
            case 0  : m.vert_data(vid).label = REGULAR; break;
            case 1  : m.vert_data(vid).label = CORNER;  break;
            case 2  : m.vert_data(vid).label = LINE;    break;
            default : m.vert_data(vid).label = CORNER;
        }
    }

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //:::::::::::::::::::::::::   LAMBDA UTILITIES   :::::::::::::::::::::::::
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    // for each surface point, find the closest point on srf
    auto update_targets = [&](const uint smooth_iters, const bool sort_by_dist)
    {
        // pre smooth the surface
        std::vector<vec3d> verts = m.vector_verts();
        for(uint i=0; i<smooth_iters; ++i)
        {
            PARALLEL_FOR(0, m.num_verts(), 1000,[&](const uint vid)
            {
                vec3d p;
                if(m.vert_is_on_srf(vid))
                {
                    for(uint nbr : m.vert_adj_srf_verts(vid)) p += verts.at(nbr);
                    p /= static_cast<double>(m.vert_adj_srf_verts(vid).size());
                }
                else
                {
                    double sum = 0.0;
                    for(uint nbr : m.adj_v2v(vid))
                    {
                        double w = (m.vert_is_on_srf(nbr)) ? 2.0 : 0.5;
                        p   += w * verts.at(nbr);
                        sum += w;
                    }
                    p /= sum;
                }
                verts.at(vid) = p;
            });
        }

        targets.clear();
        for(uint vid=0; vid<m.num_verts(); ++vid)
        {
            Proj proj;
            proj.vid    = vid;
            switch(m.vert_data(vid).label)
            {
                case REGULAR : proj.target = (m.vert_is_on_srf(vid)) ? o_srf.closest_point(verts.at(vid)) : verts.at(vid); break;
                case CORNER  : proj.target = o_corners.closest_point(verts.at(vid)); break;
                case LINE    : proj.target = o_lines.closest_point(verts.at(vid)); break;
            }
            proj.dist   = (m.vert_is_on_srf(vid)) ? 1/verts.at(vid).dist(proj.target) : -verts.at(vid).dist(proj.target);
            targets.push_back(proj);
        }

        if(sort_by_dist)
        {
            std::sort(targets.begin(), targets.end(), [](const Proj & p1,const Proj & p2) -> bool { return p1.dist>p2.dist; } );
        }
    };

    // scaled jacobian
    auto SJ_OK = [&](const uint pid, const uint vid, const vec3d & pos) -> bool
    {
        std::vector<vec3d> h = m.poly_verts(pid);
        double SJ_bef = hex_scaled_jacobian(h[0],h[1],h[2],h[3],h[4],h[5],h[6],h[7]);
        h.at(m.poly_vert_offset(pid, vid)) = pos;
        double SJ_aft = hex_scaled_jacobian(h[0],h[1],h[2],h[3],h[4],h[5],h[6],h[7]);
        if(SJ_bef >  opt.SJ_thresh && SJ_aft > opt.SJ_thresh) return true;
        if(SJ_bef <= opt.SJ_thresh && SJ_aft >= SJ_bef)       return true; // if it was already bad, just don't make it worse
        return false;
    };

    // project a point on target, reverting with binary search if some element becomes degenerate
    auto binary_search = [&](const uint vid, const vec3d & target) -> vec3d // returns dist to target
    {
        float t       = 1.0;
        uint  i       = 0;
        vec3d new_pos = target;
        bool  all_good;
        do
        {
            all_good = true;
            for(uint pid : m.adj_v2p(vid))
            {
                if(!SJ_OK(pid,vid,new_pos))
                {
                    all_good = false;
                    break;
                }
            }
            if(!all_good)
            {
                t *=0.5;
                new_pos = target*t + m.vert(vid)*(1.0-t);
            }
        }
        while(++i<6 && !all_good);
        if(all_good) return new_pos;
        return m.vert(vid);
    };

    // compute average/Hausodrff distance
    auto distance = [&](const bool hausdorff) -> double
    {
        double d = (hausdorff) ? -inf_double : 0.0;
        for(auto t : targets) d = (hausdorff) ? std::max(d,t.dist) : d + t.dist;
        if(!hausdorff) d /= static_cast<double>(targets.size());
        return d/m.bbox().diag();
    };

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //::::::::::::::::::::::   BEGIN OF ACTUAL METHOD   ::::::::::::::::::::::
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    bool converged = false;
    for(uint i=0; i<opt.max_iter && !converged; ++i)
    {
        update_targets(3,true);

        // process points and store the new distance to target for next iteration
        for(auto & t : targets)
        {
            vec3d p = binary_search(t.vid, t.target);
            m.vert(t.vid) = p;
            t.dist = p.dist(t.target);
        }

        converged = distance(opt.use_H_dist) <= opt.conv_thresh;
    }

    return distance(opt.use_H_dist);
}

}
