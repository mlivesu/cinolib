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
#include <cinolib/ambient_occlusion.h>
#include <cinolib/sphere_coverage.h>
#include <cinolib/parallel_for.h>
#include <cinolib/octree.h>

namespace cinolib
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void ambient_occlusion(AbstractPolygonMesh<M,V,E,P> & m, const uint n_samples)
{
    Octree o;
    o.build_from_mesh_polys(m);

    std::vector<vec3d> dirs;
    sphere_coverage(n_samples,dirs);

    std::vector<float> ao(m.num_polys(),1.0);
    float min = inf_float;
    float max = 0.f;
    PARALLEL_FOR(0,m.num_polys(),100,[&](const uint pid)
    {
        uint  total  = 0;
        uint  shadow = 0;
        vec3d c      = m.poly_centroid(pid);
        for(vec3d & d : dirs)
        {
            std::set<std::pair<double,uint>> hits;
            o.intersects_ray(c,d,hits);
            // discard intersections with the current element (they are due to lack of precision)
            uint real_hits = 0;
            for(auto & h : hits) if(h.second!=pid) ++real_hits;
            if(real_hits%2==0)
            {
                ++total;
                if(real_hits>0)++shadow;
            }
        }
        ao.at(pid) = (shadow>0) ? 1.f - float(shadow)/total : 1.f;
        min = std::min(min,ao.at(pid));
        max = std::max(max,ao.at(pid));
    });
    // normalize
    float delta = max-min;
    for(float & val : ao) val = (val-min)/delta;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        m.poly_data(pid).AO = ao.at(pid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void ambient_occlusion(AbstractPolyhedralMesh<M,V,E,F,P> & m, const uint n_samples)
{
    // extract the surface of the visible portion of the volume mesh and do a surface AO on it
    Polygonmesh<> srf(m.vector_verts(),{});
    std::vector<uint> fmap;
    for(uint fid=0; fid<m.num_faces(); ++fid)
    {
        uint pid_beneath;
        if(m.face_is_visible(fid,pid_beneath))
        {
            std::vector<uint> face = m.face_verts_id(fid);
            if(m.poly_face_is_CW(pid_beneath,fid)) std::reverse(face.begin(),face.end());
            srf.poly_add(face);
            fmap.push_back(fid);
        }
    }
    ambient_occlusion(srf);
    for(uint pid=0; pid<srf.num_polys(); ++pid)
    {
        m.face_data(fmap.at(pid)).AO = srf.poly_data(pid).AO;
    }
}

}
