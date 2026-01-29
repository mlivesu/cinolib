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
float ambient_occlusion(AbstractPolygonMesh<M,V,E,P> & m,
                        const uint                     pid,
                        const Octree                 & o,
                        const std::vector<vec3d>     & dirs,
                        const float                    len)
{
    uint  total  = 0;
    uint  shadow = 0;
    vec3d c      = m.poly_centroid(pid);
    for(const vec3d & d : dirs)
    {
        std::set<std::pair<double,uint>> hits;
        o.intersects_ray(c,d,hits);

        // for numerical stability, discard intersections with the current element (if any)
        while(!hits.empty() && hits.begin()->second==pid) hits.erase(hits.begin());

        // interior ray, discard
        if(hits.size()%2!=0) continue;

        ++total;

        // first hit is beyond ray length, count as shadow
        if(!hits.empty() && hits.begin()->first<len) ++shadow;
    }
    if(shadow>0) return 1.f - float(shadow)/total;
    return 1.f;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void ambient_occlusion(AbstractPolygonMesh<M,V,E,P> & m, AO_data & data)
{
    float len = data.ray_length * m.bbox().diag();

    Octree o;
    o.build_from_mesh_polys(m);
    if(data.with_floor) o.build_from_mesh_polys(data.floor);

    std::vector<vec3d> dirs;
    sphere_coverage(data.n_samples,dirs);

    std::vector<float> ao_m(m.num_polys(),1.0);
    float min = inf_float;
    float max = 0.f;
    PARALLEL_FOR(0,m.num_polys(),100,[&](const uint pid)
    {
        ao_m.at(pid) = ambient_occlusion(m,pid,o,dirs,len);
        min = std::min(min,ao_m.at(pid));
        max = std::max(max,ao_m.at(pid));
    });
    // normalize
    float delta = max-min;
    if(delta!=0) for(float & val : ao_m) val = (val-min)/delta;
    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        // https://stats.stackexchange.com/questions/214877/is-there-a-formula-for-an-s-shaped-curve-with-domain-and-range-0-1
        m.poly_data(pid).AO = 1.f/(1.f+std::pow(ao_m.at(pid)/(1.f-ao_m.at(pid)),-data.contrast));
    }

    if(data.with_floor)
    {
        std::vector<float> ao_f(data.floor.num_polys(),1.0);
        PARALLEL_FOR(0,data.floor.num_polys(),100,[&](const uint pid)
        {
            ao_f.at(pid) = ambient_occlusion(data.floor,pid,o,dirs,len);
            min = std::min(min,ao_f.at(pid));
            max = std::max(max,ao_f.at(pid));
        });
        if(delta!=0) for(float & val : ao_f) val = (val-min)/delta;
        for(uint pid=0; pid<data.floor.num_polys(); ++pid)
        {
            // https://stats.stackexchange.com/questions/214877/is-there-a-formula-for-an-s-shaped-curve-with-domain-and-range-0-1
            data.floor.poly_data(pid).AO = 1.f/(1.f+std::pow(ao_f.at(pid)/(1.f-ao_f.at(pid)),-data.contrast_floor));
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class F, class P>
CINO_INLINE
void ambient_occlusion(AbstractPolyhedralMesh<M,V,E,F,P> & m, AO_data & data)
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
    ambient_occlusion(srf,data);
    for(uint pid=0; pid<srf.num_polys(); ++pid)
    {
        m.face_data(fmap.at(pid)).AO = srf.poly_data(pid).AO;
    }
}

}
