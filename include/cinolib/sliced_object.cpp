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
#include <cinolib/sliced_object.h>
#include <cinolib/io/read_CLI.h>
#include <cinolib/triangle_wrap.h>
#include <cinolib/vector_serialization.h>
#include <cinolib/ANSI_color_codes.h>

namespace cinolib
{

template<class M, class V, class E, class P>
CINO_INLINE
SlicedObj<M,V,E,P>::SlicedObj(const char * filename, const double thick_radius)
    : Trimesh<M,V,E,P>()
    , thick_radius(thick_radius)
{
    std::vector<std::vector<std::vector<vec3d>>> slice_polys;
    std::vector<std::vector<std::vector<vec3d>>> slice_holes;
    std::vector<std::vector<std::vector<vec3d>>> supports;
    read_CLI(filename, slice_polys, slice_holes, supports, hatches);
    init(slice_polys, slice_holes, supports);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
SlicedObj<M,V,E,P>::SlicedObj(const std::vector<std::vector<std::vector<vec3d>>> & slice_polys,
                              const std::vector<std::vector<std::vector<vec3d>>> & slice_holes,
                              const std::vector<std::vector<std::vector<vec3d>>> & supports,
                              const std::vector<std::vector<std::vector<vec3d>>> & hatches,
                              const double thick_radius)
    : Trimesh<M,V,E,P>()
    , hatches(hatches)
    , thick_radius(thick_radius)
{
    init(slice_polys, slice_holes, supports);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
BoostMultiPolygon SlicedObj<M,V,E,P>::slice_as_boost_poly(const uint sid) const
{
    return slices.at(sid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::init(const std::vector<std::vector<std::vector<vec3d>>> & slice_polys,
                              const std::vector<std::vector<std::vector<vec3d>>> & slice_holes,
                              const std::vector<std::vector<std::vector<vec3d>>> & supports)
{
    uint num_slices = slice_polys.size();

    for(uint sid=0; sid<num_slices; ++sid)
    {
        uint np = slice_holes.at(sid).size();
        uint nh = slice_polys.at(sid).size();
        uint ns = (thick_radius>0) ? supports.at(sid).size() : 0;

        std::cout << "processing slice " << sid << " out of " << num_slices << "\t(" << np << " polys / " << nh << " holes / "  << ns << " supports)" << std::endl;

        if(np>0) z.push_back(slice_holes.at(sid).front().front().z()); else
        if(ns>0) z.push_back(supports.at(sid).front().front().z());    else
        continue; // empty slice, skip it

        std::vector<BoostPolygon> polys;
        std::vector<BoostPolygon> holes;
        for(auto p : slice_holes.at(sid)) polys.push_back(make_polygon(p));
        for(auto h : slice_polys.at(sid)) holes.push_back(make_polygon(h));
        if(thick_radius>0)
        {
            for(auto s : supports.at(sid)) polys.push_back(make_polygon(s, thick_radius));
        }

        BoostMultiPolygon mp;
        for(auto p : polys) mp = polygon_union(mp, p);
        for(auto p : holes) mp = polygon_difference(mp, p);
        mp = polygon_simplify(mp, 0.1*thick_radius);

        assert(mp.size()>0);
        slices.push_back(mp);
    }

    triangulate_slices();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::triangulate_slices()
{
    for(uint sid=0; sid<slices.size(); ++sid)
    {
        const BoostMultiPolygon & mp = slices.at(sid);

        std::vector<vec3d> verts;
        std::vector<uint>  tris;
        triangulate_polygon(mp, "Q", z.at(sid), verts, tris);

        uint base_addr = this->num_verts();
        uint n_tris = tris.size()/3;

        for(auto p : verts)
        {
            uint vid = this->vert_add(p);
            this->vert_data(vid).uvw[0] = static_cast<double>(sid)/static_cast<double>(num_slices());
            this->vert_data(vid).label  = sid;
        }
        for(uint i=0; i<n_tris; ++i)
        {
            uint pid = this->poly_add(base_addr + tris.at(3*i+0),
                                      base_addr + tris.at(3*i+1),
                                      base_addr + tris.at(3*i+2));
            this->poly_data(pid).label = sid;
            for(uint eid : this->adj_p2e(pid)) this->edge_data(eid).label = sid;
        }
    }
    std::cout << "new sliced object (" << num_slices() << " slices)" << std::endl;
    this->edge_mark_boundaries();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
void SlicedObj<M,V,E,P>::slice_segments(const uint           sid,
                                        std::vector<vec3d> & verts,
                                        std::vector<uint>  & segs) const
{
    // eventually substitute the whole thing with:
    //polygon_get_edges(slices.at(sid), z.at(sid), verts, segs);

    verts.clear();
    segs.clear();
    std::map<uint,uint> v_map;
    uint fresh_id = 0;
    for(uint eid=0; eid<this->num_edges(); ++eid)
    {
        if(this->edge_is_boundary(eid) && this->edge_data(eid).label == (int)sid)
        {
            for(uint off=0; off<2; ++off)
            {
                uint vid   = this->edge_vert_id(eid,off);
                auto query = v_map.find(vid);
                if(query == v_map.end())
                {
                    verts.push_back(this->vert(vid));
                    v_map[vid] = fresh_id;
                    vid = fresh_id;
                    fresh_id++;
                } else vid = query->second;
                segs.push_back(vid);
            }
        }
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
float SlicedObj<M,V,E,P>::slice_z(const uint sid) const
{
    return z.at(sid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
float SlicedObj<M,V,E,P>::slice_thickness(const uint sid) const
{
    return z.at(sid+1) - z.at(sid);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
float SlicedObj<M,V,E,P>::slice_avg_thickness() const
{
    float avg=0;
    for(uint sid=0; sid<z.size()-1; ++sid) avg += slice_thickness(sid);
    avg /= static_cast<float>(z.size()-1);
    return avg;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
bool SlicedObj<M,V,E,P>::slice_contains(const uint sid, const vec2d & p) const
{
    return polygon_contains(slices.at(sid), p, true);
}

}
