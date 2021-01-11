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
#include <cinolib/feature_mapping.h>
#include <cinolib/feature_network.h>
#include <cinolib/octree.h>
#include <cinolib/clamp.h>
#include <cinolib/dijkstra.h>
#include <cinolib/export_surface.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M1, class V1, class E1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
bool feature_mapping(const AbstractPolygonMesh<M1,V1,E1,P1> & m_source,
                           AbstractPolygonMesh<M2,V2,E2,P2> & m_target)
{
    m_target.edge_set_flag(CREASE,false);

    std::vector<std::vector<uint>> f_source, f_target;
    feature_network(m_source, f_source);

    feature_mapping(m_source, f_source, m_target, f_target);

    for(auto f : f_target)
    {
        for(uint i=1; i<f.size(); ++i)
        {
            uint v0 = f.at(i);
            uint v1 = f.at(i-1);
            int eid = m_target.edge_id(v0,v1);
            assert(eid>=0);
            m_target.edge_data(eid).flags[CREASE] = true;
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M1, class V1, class E1, class P1,
         class M2, class V2, class E2, class P2>
CINO_INLINE
bool feature_mapping(const AbstractPolygonMesh<M1,V1,E1,P1> & m_source,
                     const std::vector<std::vector<uint>>   & f_source,
                           AbstractPolygonMesh<M2,V2,E2,P2> & m_target,
                           std::vector<std::vector<uint>>   & f_target)
{
    m_target.edge_set_flag(MARKED,false);
    m_target.edge_set_flag(CREASE,false);
    f_target.clear();

    // STEP 1: map corners from source to target

    Octree o_corners;
    for(uint vid=0; vid<m_target.num_verts(); ++vid)
    {
        o_corners.push_point(vid, m_target.vert(vid));
    }
    o_corners.build();
    //
    std::unordered_map<uint,uint> corners; // maps corners in verts of m_source to corners in verts of m_target
    for(auto f : f_source)
    {
        uint   vid;
        vec3d  p;
        double dist;
        o_corners.closest_point(m_source.vert(f.front()), vid, p, dist);
        corners[f.front()] = vid;
        o_corners.closest_point(m_source.vert(f.back()), vid, p, dist);
        corners[f.back()] = vid;
    }

    // STEP 2: map curves
    Octree o_curves;
    o_curves.build_from_mesh_polys(m_target);
    double L = m_target.edge_avg_length();
    std::vector<bool> mask(m_target.num_verts(),false);
    for(auto f : f_source)
    {
        std::vector<double> l;
        l.push_back(0);
        for(uint i=1; i<f.size(); ++i)
        {
            l.push_back(l.back() + m_source.vert(f.at(i)).dist(m_source.vert(f.at(i-1))));
        }
        uint num_samples = l.back()/L;
        std::vector<vec3d> samples; // corners are ignored, as they map directly to mesh vertices
        for(uint i=1; i<num_samples; ++i)
        {
            double t = i*L;
            uint beg=0;
            while(t>=l.at(beg)) ++beg;
            assert(beg>0);
            assert(beg<l.size());
            vec3d a = m_source.vert(f.at(beg-1));
            vec3d b = m_source.vert(f.at(beg  ));
            t = (t - l.at(beg-1))/(l.at(beg) - l.at(beg-1));
            t = clamp(t,0.0,1.0);
            vec3d p = a*(1-t) + b*(t);
            samples.push_back(o_curves.closest_point(p));
        }
        // compute a distance fied from the mapped point
        std::vector<double> w(m_target.num_verts(),inf_double);
        PARALLEL_FOR(0, m_target.num_verts(), 0,[&](const uint vid)
        {
            for(auto p : samples)
            {
                w.at(vid) = std::min(w.at(vid), m_target.vert(vid).dist(p));
            }
        });
        std::vector<uint> path;
        dijkstra(m_target, corners.at(f.front()), corners.at(f.back()), w, mask, path);
        if(!path.empty())
        {
            f_target.push_back(path);
            for(uint i=2; i<path.size()-2; ++i) mask.at(path.at(i)) = true;
        }
    }

    return f_source.size()==f_target.size();
}

}
