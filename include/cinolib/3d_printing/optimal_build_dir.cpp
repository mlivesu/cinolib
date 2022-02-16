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
#include <cinolib/3d_printing/optimal_build_dir.h>
#include <cinolib/3d_printing/overhangs.h>
#include <cinolib/3d_printing/height_along_build_dir.h>
#include <cinolib/3d_printing/supports_contact_area.h>
#include <cinolib/3d_printing/supports_volume.h>
#include <cinolib/3d_printing/shadow_on_build_platform.h>
#include <cinolib/sphere_coverage.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
vec3d optimal_build_dir(const DrawableTrimesh<M,V,E,P> & m,
                        const OptimalBuildDirOptions   & opt)
{
    // evenly sample the unit sphere to produce
    // a set of candidate build directions
    std::vector<vec3d> dirs;
    sphere_coverage(opt.n_dirs, dirs);

    // cache everything that can be cached to speed up computation
    GLFWwindow *GL_context = create_offline_GL_context(opt.buffer_size, opt.buffer_size);
    u_int8_t   *data       = new u_int8_t[opt.buffer_size*opt.buffer_size];
    Octree octree;
    octree.build_from_mesh_polys(m);

    // compute scores for all candidate directions. scores are stored separately because this will
    // allow to normalize them in the same range and combine them in a meaningful way...
    //
    std::vector<float> h(opt.n_dirs, inf_float); // height (along the build direction)
    std::vector<float> a(opt.n_dirs, inf_float); // area of the projection on the building platform
    std::vector<float> c(opt.n_dirs, inf_float); // area of the contacts between model and supports
    std::vector<float> v(opt.n_dirs, inf_float); // volume of the supports
    //
    for(uint i=0; i<opt.n_dirs; ++i)
    {
        bool skip = false;
        for(const vec3d & fd : opt.forb_dirs)
        {
            if(fd.angle_deg(dirs[i])<opt.forb_cone_angle)
            {
                skip = true;
                break;
            }
        }
        if(skip) continue;

        // NOTE: this call is 90% of the computational cost
        std::vector<std::pair<uint,uint>> polys_hanging;
        overhangs(m, opt.overhang_threshold, dirs[i], polys_hanging, octree);

        // projection of the "lowest" mesh vertex along the build direction
        // this is used further down to estimate the volume of support structures
        // which are supposed to expand from the overhang down to the floor
        float floor;

        h[i] = (opt.w_height         >0) ? height_along_build_dir(m, dirs[i], floor) : 0.f;
        a[i] = (opt.w_shadow_area    >0) ? shadow_on_build_platform(m, dirs[i], opt.buffer_size, data, GL_context) : 0.f;
        c[i] = (opt.w_support_contact>0) ? supports_contact_area(m, polys_hanging) : 0.f;
        v[i] = (opt.w_support_volume >0) ? supports_volume(m, dirs[i], floor, polys_hanging) : 0.f;

        // add penalty for critical surfaces
        if(opt.w_support_contact>0 &&
           opt.crit_srf.size()  >0)
        {
            for(auto & ov : polys_hanging)
            {
                // scale overhang area
                if(CONTAINS(opt.crit_srf,ov.first))
                {
                    c[i] += m.poly_area(ov.first) * opt.crit_srf_boost;
                }
                // scale area of poly vertically below overhang
                if(ov.second!=ov.first && CONTAINS(opt.crit_srf,ov.second))
                {
                    c[i] += m.poly_area(ov.second) * opt.crit_srf_boost;
                }
            }
        }
    }

    // release memory
    destroy_offline_GL_context(GL_context);
    delete[] data;

    // normalize all scores in [0,1]
    auto h_minmax = std::minmax_element(h.begin(), h.end());
    auto a_minmax = std::minmax_element(a.begin(), a.end());
    auto c_minmax = std::minmax_element(c.begin(), c.end());
    auto v_minmax = std::minmax_element(v.begin(), v.end());
    //
    float h_min = *h_minmax.first; float h_max = *h_minmax.second;
    float a_min = *a_minmax.first; float a_max = *a_minmax.second;
    float c_min = *c_minmax.first; float c_max = *c_minmax.second;
    float v_min = *v_minmax.first; float v_max = *v_minmax.second;
    //
    for(float & f : h) f = (h_max > h_min) ? (f - h_min)/(h_max - h_min) : 1;
    for(float & f : a) f = (a_max > a_min) ? (f - a_min)/(a_max - a_min) : 1;
    for(float & f : c) f = (c_max > c_min) ? (f - c_min)/(c_max - c_min) : 1;
    for(float & f : v) f = (v_max > v_min) ? (f - v_min)/(v_max - v_min) : 1;

    // compute global scores
    std::vector<float> scores(opt.n_dirs);
    for(uint i=0; i<opt.n_dirs; ++i)
    {
        scores[i] = opt.w_height          * h[i] +
                    opt.w_shadow_area     * a[i] +
                    opt.w_support_contact * c[i] +
                    opt.w_support_volume  * v[i];
    }

    // pick the best dir (lowest score)
    auto   it = std::min_element(scores.begin(), scores.end());
    return dirs.at(std::distance(scores.begin(),it));
}

}
